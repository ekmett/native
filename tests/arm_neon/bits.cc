// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <type_traits>
#if defined(NATIVE_BITS_OMNIBUS)
import native;
#elif defined(NATIVE_BITS_ARM)
import native.arm;
#else
import native.arm.neon;
#endif

#if defined(NATIVE_BITS_ARM) || defined(NATIVE_BITS_OMNIBUS)
constexpr auto architecture = native::neon & native::arm_feature::rdm;
#else
constexpr auto architecture = native::neon;
#endif
constexpr native::isa<native::arm> scalar{};
template<class T, std::size_t N, native::isa<native::arm> A = architecture>
using vector = native::simd<T, N, A>;

enum class operation { clz, cls, rbit, rev16, rev32, rev64 };

template<operation Op, class V>
__attribute__((target("neon"))) constexpr auto apply(V value) {
  if constexpr (Op == operation::clz) return native::clz(value);
  if constexpr (Op == operation::cls) return native::cls(value);
  if constexpr (Op == operation::rbit) return native::rbit(value);
  if constexpr (Op == operation::rev16) return native::rev16(value);
  if constexpr (Op == operation::rev32) return native::rev32(value);
  if constexpr (Op == operation::rev64) return native::rev64(value);
}

// Independent specification: scan original bit positions and copy blocks.
// The implementation uses countl_zero and xor indexing instead.
template<operation Op, class T, std::size_t N>
constexpr auto reference(std::array<T, N> input) {
  using U = std::make_unsigned_t<T>;
  constexpr unsigned width = 8 * sizeof(T);
  std::array<T, N> output{};
  for (unsigned lane = 0; lane < N; ++lane) {
    U bits = U(input[lane]);
    if constexpr (Op == operation::clz || Op == operation::cls) {
      unsigned count = 0;
      unsigned skip = Op == operation::cls ? 1 : 0;
      unsigned wanted = Op == operation::cls ? unsigned(bits >> (width - 1)) : 0;
      for (unsigned i = skip; i < width; ++i) {
        if (((bits >> (width - i - 1)) & 1) != wanted) break;
        ++count;
      }
      output[lane] = T(count);
    } else if constexpr (Op == operation::rbit) {
      U result = 0;
      for (unsigned i = 0; i < width; ++i)
        if (bits & (1u << i)) result = U(result | (1u << (width - i - 1)));
      output[lane] = std::bit_cast<T>(result);
    }
  }
  if constexpr (Op == operation::rev16 || Op == operation::rev32 || Op == operation::rev64) {
    constexpr unsigned block = (Op == operation::rev16 ? 16 : Op == operation::rev32 ? 32 : 64) / width;
    for (unsigned base = 0; base < N; base += block)
      for (unsigned offset = 0; offset < block; ++offset)
        output[base + offset] = input[base + block - offset - 1];
  }
  return output;
}

template<operation Op, class T, std::size_t N>
__attribute__((target("neon"))) constexpr bool check(std::array<T, N> input) {
  using V = vector<T, N>;
  auto value = V::load(input.data());
  auto result = apply<Op>(value);
  static_assert(std::same_as<decltype(result), V>);
  std::array<T, N> actual{};
  result.store(actual.data());
  return actual == reference<Op>(input);
}

template<operation Op, class T, std::size_t N>
constexpr bool constant_cases() {
  using U = std::make_unsigned_t<T>;
  for (unsigned trial = 0; trial < 8 * sizeof(T) * 2 + 3; ++trial) {
    std::array<T, N> input{};
    for (unsigned lane = 0; lane < N; ++lane) {
      unsigned bit = (trial + lane) % (8 * sizeof(T));
      U bits = trial == 0 ? U(0) : U(U(1) << bit);
      if (trial > 8 * sizeof(T)) bits = U(~bits);
      input[lane] = std::bit_cast<T>(bits);
    }
    if (!check<Op>(input)) return false;
  }
  return true;
}

template<operation Op, class T, std::size_t N>
__attribute__((noinline, target("neon"))) bool runtime_cases(std::uint32_t &state) {
  static_assert(constant_cases<Op, T, N>());
  for (unsigned trial = 0; trial < 4096; ++trial) {
    std::array<T, N> input{};
    for (unsigned lane = 0; lane < N; ++lane) {
      state ^= state << 13;
      state ^= state >> 17;
      state ^= state << 5;
      using U = std::make_unsigned_t<T>;
      // Exhaust every byte encoding; larger lanes receive distinct PRNG words.
      U bits = sizeof(T) == 1 ? U(trial + lane) : U(state);
      input[lane] = std::bit_cast<T>(bits);
    }
    if (!check<Op>(input)) return false;
  }
  return true;
}

template<operation Op, class T, std::size_t N>
consteval bool known(std::array<T, N> input, std::array<T, N> expected) {
  auto value = apply<Op>(vector<T, N>::load(input.data()));
  std::array<T, N> actual{};
  value.store(actual.data());
  return actual == expected;
}
static_assert(known<operation::clz, std::uint8_t, 8>(
  {0, 1, 2, 3, 0x7f, 0x80, 0xaa, 0xff}, {8, 7, 6, 6, 1, 0, 0, 0}));
static_assert(known<operation::cls, std::int8_t, 8>(
  {0, 1, 2, 3, 127, -128, -86, -1}, {7, 6, 5, 5, 0, 0, 0, 7}));
static_assert(known<operation::rbit, std::uint8_t, 8>(
  {0, 1, 2, 3, 0x7f, 0x80, 0xaa, 0xff}, {0, 0x80, 0x40, 0xc0, 0xfe, 1, 0x55, 0xff}));
static_assert(known<operation::rev16, std::uint8_t, 8>(
  {0, 1, 2, 3, 4, 5, 6, 7}, {1, 0, 3, 2, 5, 4, 7, 6}));
static_assert(known<operation::rev32, std::uint16_t, 4>(
  {0x0102, 0x0304, 0x0506, 0x0708}, {0x0304, 0x0102, 0x0708, 0x0506}));
static_assert(known<operation::rev64, std::uint32_t, 4>(
  {0x01020304, 0x05060708, 0x090a0b0c, 0x0d0e0f10},
  {0x05060708, 0x01020304, 0x0d0e0f10, 0x090a0b0c}));

template<class V> concept has_clz = requires(V x) { native::clz(x); };
template<class V> concept has_cls = requires(V x) { native::cls(x); };
template<class V> concept has_rbit = requires(V x) { native::rbit(x); };
template<class V> concept has_rev16 = requires(V x) { native::rev16(x); };
template<class V> concept has_rev32 = requires(V x) { native::rev32(x); };
template<class V> concept has_rev64 = requires(V x) { native::rev64(x); };
static_assert(!has_clz<vector<std::uint32_t, 4, scalar>>);
static_assert(!has_clz<vector<std::uint64_t, 2>>);
static_assert(!has_clz<vector<std::uint32_t, 3>>);
static_assert(!has_clz<vector<float, 4>>);
static_assert(!has_cls<vector<std::uint32_t, 4>>);
static_assert(!has_cls<vector<std::int64_t, 2>>);
static_assert(!has_rbit<vector<std::uint16_t, 8>>);
static_assert(!has_rev16<vector<std::uint16_t, 8>>);
static_assert(!has_rev32<vector<std::uint32_t, 4>>);
static_assert(!has_rev64<vector<std::uint64_t, 2>>);
static_assert(noexcept(native::clz(vector<std::uint32_t, 4>{})));
static_assert(noexcept(native::cls(vector<std::int32_t, 4>{})));
static_assert(noexcept(native::rbit(vector<std::uint8_t, 16>{})));
static_assert(noexcept(native::rev16(vector<std::uint8_t, 16>{})));
static_assert(noexcept(native::rev32(vector<std::uint16_t, 8>{})));
static_assert(noexcept(native::rev64(vector<std::uint32_t, 4>{})));

std::uint64_t read_status() {
  std::uint64_t result;
  asm volatile("mrs %0, fpsr" : "=r"(result));
  return result;
}
void write_status(std::uint64_t value) { asm volatile("msr fpsr, %0" : : "r"(value)); }

bool all_shapes(std::uint32_t &state) {
  if (!runtime_cases<operation::clz, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::clz, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::clz, std::int16_t, 4>(state)) return false;
  if (!runtime_cases<operation::clz, std::int16_t, 8>(state)) return false;
  if (!runtime_cases<operation::clz, std::int32_t, 2>(state)) return false;
  if (!runtime_cases<operation::clz, std::int32_t, 4>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint8_t, 8>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint8_t, 16>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint16_t, 4>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint16_t, 8>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint32_t, 2>(state)) return false;
  if (!runtime_cases<operation::clz, std::uint32_t, 4>(state)) return false;
  if (!runtime_cases<operation::cls, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::cls, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::cls, std::int16_t, 4>(state)) return false;
  if (!runtime_cases<operation::cls, std::int16_t, 8>(state)) return false;
  if (!runtime_cases<operation::cls, std::int32_t, 2>(state)) return false;
  if (!runtime_cases<operation::cls, std::int32_t, 4>(state)) return false;
  if (!runtime_cases<operation::rbit, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rbit, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rbit, std::uint8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rbit, std::uint8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev16, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev16, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev16, std::uint8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev16, std::uint8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev32, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev32, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev32, std::int16_t, 4>(state)) return false;
  if (!runtime_cases<operation::rev32, std::int16_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev32, std::uint8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev32, std::uint8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev32, std::uint16_t, 4>(state)) return false;
  if (!runtime_cases<operation::rev32, std::uint16_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int16_t, 4>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int16_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int32_t, 2>(state)) return false;
  if (!runtime_cases<operation::rev64, std::int32_t, 4>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint8_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint8_t, 16>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint16_t, 4>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint16_t, 8>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint32_t, 2>(state)) return false;
  if (!runtime_cases<operation::rev64, std::uint32_t, 4>(state)) return false;
  return true;
}

int main() {
  if (!native::classify_isa(native::observe_arm_capabilities(), native::neon).admitted()) return 77;
  auto saved = read_status();
  std::uint32_t state = 0x31415927;
  bool ok = true;
  for (auto flags : {std::uint64_t(0), std::uint64_t(0x0800009f)}) {
    write_status(flags);
    ok = all_shapes(state) && ok;
    ok = (read_status() == flags) && ok;
  }
  write_status(saved);
  std::puts(ok ? "46 NEON bit shapes: constexpr, exact native values and preserved FPSR" : "NEON bit check failed");
  return ok ? 0 : 1;
}
