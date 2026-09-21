// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#define NATIVE_TARGET_test_vpopcntdq "avx512f,avx512vpopcntdq"
#define NATIVE_TARGET_test_vpopcntdq_vl "avx512f,avx512vpopcntdq,avx512vl"
inline constexpr auto requirements_512 = NATIVE_TARGET_ISA(test_vpopcntdq);
inline constexpr auto requirements_vl = NATIVE_TARGET_ISA(test_vpopcntdq_vl);
inline constexpr auto exact_512 = native::x86_feature::avx512f & native::x86_feature::avx512vpopcntdq;
inline constexpr auto exact_vl = exact_512 & native::x86_feature::avx512vl;

template<native::isa A, class V, class M, bool Expected>
consteval bool dword_availability() {
  return (requires(V v) {
    { native::vpopcntd<A>(v) } noexcept -> std::same_as<V>;
  }) == Expected && (requires(V src, M mask, V v) {
    { native::mask_vpopcntd<A>(src, mask, v) } noexcept -> std::same_as<V>;
  }) == Expected && (requires(M mask, V v) {
    { native::maskz_vpopcntd<A>(mask, v) } noexcept -> std::same_as<V>;
  }) == Expected;
}

template<native::isa A, class V, bool Expected>
consteval bool qword_availability() {
  return (requires(V v) {
    { native::vpopcntq<A>(v) } noexcept -> std::same_as<V>;
  }) == Expected && (requires(V src, __mmask8 mask, V v) {
    { native::mask_vpopcntq<A>(src, mask, v) } noexcept -> std::same_as<V>;
  }) == Expected && (requires(__mmask8 mask, V v) {
    { native::maskz_vpopcntq<A>(mask, v) } noexcept -> std::same_as<V>;
  }) == Expected;
}

template<native::isa A, bool Zmm, bool Vl>
consteval bool availability() {
  return dword_availability<A, __m512i, __mmask16, Zmm>() &&
    qword_availability<A, __m512i, Zmm>() &&
    dword_availability<A, __m256i, __mmask8, Vl>() &&
    qword_availability<A, __m256i, Vl>() &&
    dword_availability<A, __m128i, __mmask8, Vl>() &&
    qword_availability<A, __m128i, Vl>();
}

static_assert(availability<exact_512, true, false>());
static_assert(availability<exact_vl, true, true>());
static_assert(availability<requirements_512, true, false>());
static_assert(availability<requirements_vl, true, true>());
static_assert(availability<native::scalar, false, false>());
static_assert(availability<native::isa(native::x86_feature::avx512vpopcntdq), false, false>());
static_assert(availability<native::x86_feature::avx512vpopcntdq & native::x86_feature::avx512vl, false, false>());
static_assert(availability<native::isa(native::x86_feature::avx512f), false, false>());
static_assert(availability<native::x86_feature::avx512f & native::x86_feature::avx512vl, false, false>());
static_assert(availability<native::avx512, false, false>());
static_assert(availability<native::avx512 & native::x86_feature::popcnt, false, false>());

// Only scalar pointers and an integer mask cross the target boundary.
// A separate instantiation handles each lane width; no vector ABI reaches main.
#define NATIVE_TEST_VPOPCNT_WRAPPER(Name, Vector, Requirements) \
  template<class T> native_noinline void Name( \
      T* plain, T* merged, T* zeroed, T const* source, T const* input, unsigned mask) noexcept { \
    using M = std::conditional_t<sizeof(Vector) == 64 && sizeof(T) == 4, __mmask16, __mmask8>; \
    Vector src, value; \
    __builtin_memcpy(&src, source, sizeof(src)); \
    __builtin_memcpy(&value, input, sizeof(value)); \
    if constexpr (std::same_as<T, std::uint32_t>) { \
      auto a = native::vpopcntd<Requirements>(value); \
      auto b = native::mask_vpopcntd<Requirements>(src, static_cast<M>(mask), value); \
      auto c = native::maskz_vpopcntd<Requirements>(static_cast<M>(mask), value); \
      __builtin_memcpy(plain, &a, sizeof(a)); \
      __builtin_memcpy(merged, &b, sizeof(b)); \
      __builtin_memcpy(zeroed, &c, sizeof(c)); \
    } else { \
      static_assert(std::same_as<T, std::uint64_t>); \
      auto a = native::vpopcntq<Requirements>(value); \
      auto b = native::mask_vpopcntq<Requirements>(src, static_cast<M>(mask), value); \
      auto c = native::maskz_vpopcntq<Requirements>(static_cast<M>(mask), value); \
      __builtin_memcpy(plain, &a, sizeof(a)); \
      __builtin_memcpy(merged, &b, sizeof(b)); \
      __builtin_memcpy(zeroed, &c, sizeof(c)); \
    } \
  }

NATIVE_TARGET_PUSH(test_vpopcntdq)
NATIVE_TEST_VPOPCNT_WRAPPER(invoke_512, __m512i, exact_512)
NATIVE_TARGET_POP()
NATIVE_TARGET_PUSH(test_vpopcntdq_vl)
NATIVE_TEST_VPOPCNT_WRAPPER(invoke_128, __m128i, exact_vl)
NATIVE_TEST_VPOPCNT_WRAPPER(invoke_256, __m256i, exact_vl)
NATIVE_TARGET_POP()
#undef NATIVE_TEST_VPOPCNT_WRAPPER

// This oracle is compiled at the baseline outside every optional target scope.
template<class T> native_noinline T scalar_popcount(T value) noexcept {
  T count = 0;
  for (unsigned bit = 0; bit != std::numeric_limits<T>::digits; ++bit)
    count += static_cast<T>((value >> bit) & T{1});
  return count;
}

template<class T> bool check_scalar_width() {
  constexpr auto width = std::numeric_limits<T>::digits;
  constexpr T all = std::numeric_limits<T>::max();
  if (scalar_popcount(T{0}) != T{0} || scalar_popcount(all) != T{width}) return false;
  for (unsigned bit = 0; bit != width; ++bit) {
    T one = T{1} << bit;
    if (scalar_popcount(one) != T{1} || scalar_popcount(static_cast<T>(all ^ one)) != T{width - 1})
      return false;
  }
  return true;
}

inline bool check_scalar_reference() {
  return check_scalar_width<std::uint32_t>() && check_scalar_width<std::uint64_t>();
}

inline std::uint64_t next_random(std::uint64_t& state) noexcept {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

template<class T, std::size_t N>
bool check_vector(std::array<T, N> const& input, std::array<T, N> const& source, unsigned mask) {
  std::array<T, N> plain{}, merged{}, zeroed{};
  if constexpr (sizeof(T) * N == 64)
    invoke_512(plain.data(), merged.data(), zeroed.data(), source.data(), input.data(), mask);
  else if constexpr (sizeof(T) * N == 32)
    invoke_256(plain.data(), merged.data(), zeroed.data(), source.data(), input.data(), mask);
  else {
    static_assert(sizeof(T) * N == 16);
    invoke_128(plain.data(), merged.data(), zeroed.data(), source.data(), input.data(), mask);
  }
  for (std::size_t lane = 0; lane != N; ++lane) {
    T expected = scalar_popcount(input[lane]);
    bool active = (mask >> lane) & 1u;
    if (plain[lane] != expected || merged[lane] != (active ? expected : source[lane]) ||
        zeroed[lane] != (active ? expected : T{0})) {
      std::printf("VPOPCNT failure: register=%zu lane-width=%zu lane=%zu mask=0x%x input=0x%llx\n",
        sizeof(T) * N * 8, sizeof(T) * 8, lane, mask, static_cast<unsigned long long>(input[lane]));
      return false;
    }
  }
  return true;
}

template<class T, std::size_t N>
bool check_masks(std::array<T, N> const& input, std::array<T, N> const& source) {
  constexpr unsigned mask_width = N == 16 ? 16 : 8;
  constexpr unsigned all_masks = (1u << mask_width) - 1;
  constexpr unsigned active_lanes = (1u << N) - 1;
  for (unsigned mask : {0u, all_masks, 0xaaaau & all_masks, 0x5555u & all_masks,
                        all_masks ^ active_lanes})
    if (!check_vector(input, source, mask)) return false;
  // Includes the ignored high mask bits in 128-bit dword and 128/256-bit qword forms.
  for (unsigned bit = 0; bit != mask_width; ++bit) {
    unsigned one = 1u << bit;
    if (!check_vector(input, source, one) || !check_vector(input, source, all_masks ^ one))
      return false;
  }
  return true;
}

template<class T, std::size_t N> bool check_vectors(std::uint64_t& state) {
  constexpr unsigned width = std::numeric_limits<T>::digits;
  constexpr T all = std::numeric_limits<T>::max();
  constexpr std::array<T, 8> edges{0, all, 1, T{1} << (width - 1),
    static_cast<T>(all / 3), static_cast<T>(all / 3 * 2),
    static_cast<T>((T{1} << (width / 2)) - 1), static_cast<T>(all << (width / 2))};
  std::array<T, N> input{}, source{};
  for (std::size_t lane = 0; lane != N; ++lane)
    source[lane] = static_cast<T>(all - T{257} * static_cast<T>(lane));
  for (T edge : edges) {
    input.fill(edge);
    if (!check_masks(input, source)) return false;
  }
  // Rotating unequal populations detects lane reversal and lane mixing.
  for (std::size_t rotation = 0; rotation != N; ++rotation) {
    for (std::size_t lane = 0; lane != N; ++lane)
      input[lane] = static_cast<T>((T{1} << ((lane + rotation) % N)) - 1);
    if (!check_masks(input, source)) return false;
  }
  for (unsigned bit = 0; bit != width; ++bit) {
    for (std::size_t lane = 0; lane != N; ++lane)
      input[lane] = T{1} << ((bit + lane) % width);
    if (!check_masks(input, source)) return false;
    for (T& value : input) value = static_cast<T>(all ^ value);
    if (!check_masks(input, source)) return false;
  }
  for (unsigned iteration = 0; iteration != 4096; ++iteration) {
    for (std::size_t lane = 0; lane != N; ++lane) {
      input[lane] = static_cast<T>(next_random(state));
      source[lane] = static_cast<T>(next_random(state));
    }
    if (!check_vector(input, source, static_cast<unsigned>(next_random(state)))) return false;
  }
  return true;
}
