// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#define NATIVE_TARGET_test_crc32c "crc32"

namespace crc32c_fixture {
  inline constexpr auto requirements = NATIVE_TARGET_ISA(test_crc32c);
  static_assert(requirements == native::isa(native::x86_feature::crc32));
  static_assert(native::feature_closure(requirements) == requirements);
  static_assert(!requirements.has(native::x86_feature::popcnt));
  static_assert(!requirements.has(native::x86_feature::sse42));
  static_assert(!requirements.has(native::x86_feature::avx));
  static_assert(native::target_features("sse4.2").has(native::x86_feature::crc32));

  template<native::isa A, class T>
  concept has_crc32c = requires(std::uint32_t accumulator, T value) {
    { native::crc32c<A>(accumulator, value) } noexcept -> std::same_as<std::uint32_t>;
  };

  template<class T> consteval bool feature_contract() {
    return has_crc32c<requirements, T> &&
      !has_crc32c<native::scalar, T> &&
      !has_crc32c<native::isa(native::x86_feature::popcnt), T> &&
      !has_crc32c<native::isa(native::x86_feature::sse42), T> &&
      !has_crc32c<native::isa(native::x86_feature::bmi1), T> &&
      !has_crc32c<native::isa(native::x86_feature::bmi2), T>;
  }
  static_assert(feature_contract<std::uint8_t>());
  static_assert(feature_contract<std::uint16_t>());
  static_assert(feature_contract<std::uint32_t>());
#if defined(__x86_64__) || defined(_M_X64)
  static_assert(feature_contract<std::uint64_t>());
#endif

  // This raw fixture is independent of the feature registry under test.
  struct raw_snapshot {
    std::uint32_t max_basic_leaf = 0, leaf1_ecx = 0, leaf1_edx = 0;
    std::uint32_t leaf7_ebx = 0, max_leaf7_subleaf = 0;
    std::uint32_t leaf7_1_eax = 0, leaf7_edx = 0;
    std::uint64_t xcr0 = 0;
    bool xcr0_observed = false;
  };

  consteval bool admission_contract() {
    native::x86_capabilities cpu{};
    cpu.present.set(native::x86_feature::crc32, true);
    cpu.observed.set(native::x86_feature::crc32, true);
    // The instruction needs no SIMD bits, XSAVE, OSXSAVE or XCR0 observation.
    auto result = native::classify_isa(cpu, requirements);
    if (!result.admitted() || result.missing_xcr0 || result.missing_xcr0_observation)
      return false;
    cpu.present.set(native::x86_feature::crc32, false);
    if (native::classify_isa(cpu, requirements).admitted()) return false;
    cpu.present.set(native::x86_feature::crc32, true);
    cpu.observed.set(native::x86_feature::crc32, false);
    cpu.raw.max_basic_leaf = 1;
    cpu.raw.leaf1_ecx = 1u << 20;
    if (native::classify_isa(cpu, requirements).admitted()) return false;

    // CPUID.1:ECX[20] is the entire raw CRC32 hardware requirement.
    raw_snapshot raw{};
    raw.max_basic_leaf = 1;
    for (unsigned bit = 0; bit != 32; ++bit) {
      raw.leaf1_ecx = std::uint32_t{1} << bit;
      result = native::classify_isa(raw, requirements);
      if (result.admitted() != (bit == 20)) return false;
      if (result.missing_xcr0 || result.missing_xcr0_observation) return false;
    }
    raw.leaf1_ecx = 1u << 20;
    // SSE4.2's compiler closure has additional requirements, unlike CRC32.
    if (native::classify_isa(raw, native::x86_feature::sse42).admitted()) return false;
    raw.max_basic_leaf = 0;
    if (native::classify_isa(raw, requirements).admitted()) return false;
    raw.max_basic_leaf = 7;
    raw.leaf1_ecx = ~(1u << 20);
    raw.leaf1_edx = ~0u;
    raw.leaf7_ebx = ~0u;
    raw.leaf7_1_eax = ~0u;
    raw.leaf7_edx = ~0u;
    raw.max_leaf7_subleaf = 1;
    raw.xcr0 = ~std::uint64_t{0};
    raw.xcr0_observed = true;
    return !native::classify_isa(raw, requirements).admitted();
  }
  static_assert(admission_contract());

  // Polynomial division one input bit at a time, least significant bit first.
  // No lookup table, intrinsic, host byte order, or implicit complement is used.
  template<class T>
  constexpr std::uint32_t oracle(std::uint32_t accumulator, T value) noexcept {
    for (unsigned bit = 0; bit != std::numeric_limits<T>::digits; ++bit) {
      bool feedback = ((accumulator ^ (value >> bit)) & 1u) != 0;
      accumulator >>= 1;
      if (feedback) accumulator ^= 0x82f63b78u;
    }
    return accumulator;
  }

  inline constexpr std::array<std::uint8_t, 9> check_string{
    '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  static_assert([] {
    std::uint32_t accumulator = 0xffffffffu;
    for (auto byte : check_string) accumulator = oracle(accumulator, byte);
    return accumulator == 0x1cf96d7cu && (accumulator ^ 0xffffffffu) == 0xe3069283u;
  }());

  NATIVE_TARGET_PUSH(test_crc32c)
  template<class T>
  native_noinline std::uint32_t invoke(std::uint32_t accumulator, T value) noexcept {
    return native::crc32c<requirements>(accumulator, value);
  }
  NATIVE_TARGET_POP()

  inline std::uint64_t next_random(std::uint64_t& state) noexcept {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
  }

  template<class T>
  bool check_pair(std::uint32_t accumulator, T value) {
    auto expected = oracle(accumulator, value);
    auto actual = invoke(accumulator, value);
    if (actual != expected) {
      std::printf("CRC32C width=%u accumulator=%08x value=%016llx got=%08x expected=%08x\n",
                  unsigned(std::numeric_limits<T>::digits), unsigned(accumulator),
                  static_cast<unsigned long long>(value), unsigned(actual), unsigned(expected));
      return false;
    }
    // Numeric shifts specify the byte order without reading a wider buffer.
    auto bytes = accumulator;
    for (unsigned shift = 0; shift != std::numeric_limits<T>::digits; shift += 8)
      bytes = invoke(bytes, static_cast<std::uint8_t>(value >> shift));
    if (bytes != actual) return false;
    if constexpr (sizeof(T) >= 4) {
      auto words = accumulator;
      for (unsigned shift = 0; shift != std::numeric_limits<T>::digits; shift += 16)
        words = invoke(words, static_cast<std::uint16_t>(value >> shift));
      if (words != actual) return false;
    }
    if constexpr (sizeof(T) == 8) {
      auto dwords = invoke(accumulator, static_cast<std::uint32_t>(value));
      dwords = invoke(dwords, static_cast<std::uint32_t>(value >> 32));
      if (dwords != actual) return false;
    }
    return true;
  }

  template<class T> bool check_width(std::uint64_t& state) {
    constexpr unsigned width = std::numeric_limits<T>::digits;
    constexpr T all = std::numeric_limits<T>::max();
    constexpr T high = T{1} << (width - 1);
    constexpr std::array<std::uint32_t, 8> accumulators{
      0u, 1u, 0xffffffffu, 0x80000000u, 0x7fffffffu,
      0xaaaaaaaau, 0x55555555u, 0x12345678u};
    constexpr std::array<T, 10> values{
      0, 1, 2, all, T(all - 1), high, T(high - 1),
      T(0xaaaaaaaaaaaaaaaaull), T(0x5555555555555555ull), T(0x0123456789abcdefull)};
    for (auto accumulator : accumulators) {
      for (T value : values)
        if (!check_pair(accumulator, value)) return false;
      for (unsigned bit = 0; bit != width; ++bit) {
        T single = static_cast<T>(T{1} << bit);
        if (!check_pair(accumulator, single) || !check_pair(accumulator, T(all ^ single)))
          return false;
      }
    }
    // Every bit of the raw accumulator matters, including its high bit.
    for (unsigned bit = 0; bit != 32; ++bit)
      for (T value : values)
        if (!check_pair(std::uint32_t{1} << bit, value) ||
            !check_pair(~(std::uint32_t{1} << bit), value)) return false;
    if constexpr (width <= 16) {
      for (auto accumulator : std::array<std::uint32_t, 4>{0, 0xffffffffu, 0x80000000u, 0x12345678u})
        for (std::uint32_t value = 0; value != (std::uint32_t{1} << width); ++value)
          if (!check_pair(accumulator, static_cast<T>(value))) return false;
    }
    for (unsigned i = 0; i != 8192; ++i) {
      auto accumulator = static_cast<std::uint32_t>(next_random(state));
      auto value = static_cast<T>(next_random(state));
      if (!check_pair(accumulator, value)) return false;
    }
    if (invoke(0u, T{0}) != 0u) return false;
    std::printf("CRC32C width=%u: directed edges, %s8192 randomized updates and width order passed\n",
                width, width <= 16 ? "exhaustive operands at four seeds, " : "");
    return true;
  }

  template<class T, std::size_t N>
  bool check_buffer(std::array<std::uint8_t, N> const& bytes, std::size_t length,
                    std::uint32_t seed) {
    std::uint32_t expected = seed;
    for (std::size_t i = 0; i != length; ++i) expected = oracle(expected, bytes[i]);
    std::uint32_t actual = seed;
    std::size_t offset = 0;
    while (length - offset >= sizeof(T)) {
      T value = 0;
      for (unsigned i = 0; i != sizeof(T); ++i)
        value |= static_cast<T>(T{bytes[offset + i]} << (8 * i));
      actual = invoke(actual, value);
      offset += sizeof(T);
    }
    while (offset != length) actual = invoke(actual, bytes[offset++]);
    return actual == expected;
  }

  inline bool check_vectors(std::uint64_t& state) {
    std::uint32_t accumulator = 0xffffffffu;
    for (auto byte : check_string) accumulator = invoke(accumulator, byte);
    if (accumulator != 0x1cf96d7cu || (accumulator ^ 0xffffffffu) != 0xe3069283u)
      return false;
    if (!check_buffer<std::uint16_t>(check_string, check_string.size(), 0xffffffffu) ||
        !check_buffer<std::uint32_t>(check_string, check_string.size(), 0xffffffffu)) return false;
#if defined(__x86_64__) || defined(_M_X64)
    if (!check_buffer<std::uint64_t>(check_string, check_string.size(), 0xffffffffu)) return false;
#endif
    // Every tail length uses explicit byte assembly, including empty messages.
    std::array<std::uint8_t, 257> bytes{};
    for (auto& byte : bytes) byte = static_cast<std::uint8_t>(next_random(state));
    for (std::size_t length = 0; length <= bytes.size(); ++length) {
      auto seed = static_cast<std::uint32_t>(next_random(state));
      if (!check_buffer<std::uint8_t>(bytes, length, seed) ||
          !check_buffer<std::uint16_t>(bytes, length, seed) ||
          !check_buffer<std::uint32_t>(bytes, length, seed)) return false;
#if defined(__x86_64__) || defined(_M_X64)
      if (!check_buffer<std::uint64_t>(bytes, length, seed)) return false;
#endif
    }
    std::puts("CRC32C known check string, explicit complements, and lengths 0..257 passed");
    return true;
  }
}
