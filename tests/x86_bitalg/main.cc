// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 1
import native.x86.bitalg;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
#error Select the module or hub interface
#endif
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/attributes.h>

inline constexpr auto full_width = native::target_features<native::x86>("avx512bitalg");
inline constexpr auto short_width = native::target_features<native::x86>("avx512bitalg,avx512vl");
inline constexpr auto broad = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512bitalg");
inline constexpr auto sse2_storage = native::target_features<native::x86>("sse2");
inline constexpr auto avx_storage = native::target_features<native::x86>("avx");
inline constexpr auto avx512_storage = native::target_features<native::x86>("avx512f");

// The oracle counts individual bits instead of using the implementation's popcount.
template<class T, std::size_t N>
constexpr std::array<std::array<T, N>, 3> population_reference(
  std::array<T, N> const & input, std::array<T, N> const & source, std::uint64_t mask) {
  std::array<std::array<T, N>, 3> result{};
  for (std::size_t lane = 0; lane < N; ++lane) {
    T count = 0;
    for (unsigned bit = 0; bit < sizeof(T) * 8; ++bit) {
      count += (input[lane] >> bit) & 1;
    }
    bool active = (mask >> lane) & 1;
    result[0][lane] = count;
    result[1][lane] = active ? count : source[lane];
    result[2][lane] = active ? count : 0;
  }
  return result;
}

// Traverse source qwords and their eight outputs independently of the polyfill.
template<std::size_t Q>
constexpr std::array<std::uint64_t, 2> shuffle_reference(
  std::array<std::uint64_t, Q> const & words,
  std::array<std::uint8_t, Q * 8> const & control, std::uint64_t mask) {
  std::uint64_t result = 0;
  for (std::size_t qword = 0; qword < Q; ++qword) {
    for (std::size_t byte = 0; byte < 8; ++byte) {
      auto output = qword * 8 + byte;
      auto selected = std::uint64_t{1} << (control[output] % 64);
      if (words[qword] & selected) {
        result |= std::uint64_t{1} << output;
      }
    }
  }
  return {result, result & mask};
}

template<native::isa<native::x86> A, class T, std::size_t N>
consteval auto population_constant(
  std::array<T, N> input, std::array<T, N> source, std::uint64_t mask) {
  using vector_type = native::simd<T, N, A>;
  auto value = vector_type::load(input.data());
  auto prior = vector_type::load(source.data());
  auto active = native::predicate<N, A>::from_bitset(mask);
  std::array<std::array<T, N>, 3> result{};
  if constexpr (sizeof(T) == 1) {
    native::vpopcntb<A>(value).store(result[0].data());
    native::mask_vpopcntb<A>(prior, active, value).store(result[1].data());
    native::maskz_vpopcntb<A>(active, value).store(result[2].data());
  } else {
    native::vpopcntw<A>(value).store(result[0].data());
    native::mask_vpopcntw<A>(prior, active, value).store(result[1].data());
    native::maskz_vpopcntw<A>(active, value).store(result[2].data());
  }
  return result;
}

template<native::isa<native::x86> A, std::size_t Q>
consteval auto shuffle_constant(
  std::array<std::uint64_t, Q> words,
  std::array<std::uint8_t, Q * 8> control, std::uint64_t mask) {
  auto value = native::simd<std::uint64_t, Q, A>::load(words.data());
  auto selectors = native::simd<std::uint8_t, Q * 8, A>::load(control.data());
  auto active = native::predicate<Q * 8, A>::from_bitset(mask);
  using result_type = native::predicate<Q * 8, A>;
  static_assert(std::same_as<decltype(native::vpshufbitqmb<A>(value, selectors)), result_type>);
  static_assert(noexcept(native::vpshufbitqmb<A>(value, selectors)));
  static_assert(noexcept(native::mask_vpshufbitqmb<A>(active, value, selectors)));
  return std::array<std::uint64_t, 2>{
    native::vpshufbitqmb<A>(value, selectors).to_bitset(),
    native::mask_vpshufbitqmb<A>(active, value, selectors).to_bitset()};
}

template<native::isa<native::x86> A, class T, std::size_t N>
consteval bool population_constants() {
  std::array<T, N> input{};
  std::array<T, N> source{};
  for (unsigned pattern = 0; pattern < 256; ++pattern) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input[lane] = static_cast<T>((pattern + lane) * (sizeof(T) == 1 ? 1 : 257));
      source[lane] = static_cast<T>(0x81 + lane * 17);
    }
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{1} << (N - 1)}) {
      if (population_constant<A>(input, source, mask) != population_reference(input, source, mask)) {
        return false;
      }
    }
  }
  for (T value : {T{0}, static_cast<T>(~T{0})}) {
    input.fill(value);
    if (population_constant<A>(input, source, ~std::uint64_t{0}) !=
        population_reference(input, source, ~std::uint64_t{0})) {
      return false;
    }
  }
  for (unsigned bit = 0; bit < sizeof(T) * 8; ++bit) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input[lane] = static_cast<T>(T{1} << ((bit + lane) % (sizeof(T) * 8)));
    }
    if (population_constant<A>(input, source, ~std::uint64_t{0}) !=
        population_reference(input, source, ~std::uint64_t{0})) {
      return false;
    }
  }
  return true;
}

template<native::isa<native::x86> A, std::size_t Q>
consteval bool shuffle_constants() {
  std::array<std::uint64_t, Q> words{};
  std::array<std::uint8_t, Q * 8> control{};
  for (unsigned pattern = 0; pattern < 256; ++pattern) {
    for (std::size_t qword = 0; qword < Q; ++qword) {
      words[qword] = 0x8000000102040810ull ^ (0x0123456789abcdefull * qword);
    }
    for (std::size_t lane = 0; lane < Q * 8; ++lane) {
      control[lane] = static_cast<std::uint8_t>(pattern + lane * 7);
    }
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{1} << (Q * 8 - 1)}) {
      if (shuffle_constant<A>(words, control, mask) != shuffle_reference(words, control, mask)) {
        return false;
      }
    }
  }
  for (auto word : {std::uint64_t{0}, ~std::uint64_t{0}}) {
    words.fill(word);
    if (shuffle_constant<A>(words, control, ~std::uint64_t{0}) !=
        shuffle_reference(words, control, ~std::uint64_t{0})) {
      return false;
    }
  }
  // Each one-hot input bit is selectable in every qword; high control bits do not matter.
  for (unsigned bit = 0; bit < 64; ++bit) {
    words.fill(std::uint64_t{1} << bit);
    control.fill(static_cast<std::uint8_t>(bit | 0xc0));
    if (shuffle_constant<A>(words, control, ~std::uint64_t{0}) !=
        shuffle_reference(words, control, ~std::uint64_t{0})) {
      return false;
    }
  }
  return true;
}

static_assert(population_constants<sse2_storage, std::uint8_t, 16>());
static_assert(population_constants<sse2_storage, std::uint16_t, 8>());
static_assert(shuffle_constants<sse2_storage, 2>());

static_assert(population_constants<avx_storage, std::uint8_t, 32>());
static_assert(population_constants<avx_storage, std::uint16_t, 16>());
static_assert(shuffle_constants<avx_storage, 4>());

static_assert(population_constants<avx512_storage, std::uint8_t, 64>());
static_assert(population_constants<avx512_storage, std::uint16_t, 32>());
static_assert(shuffle_constants<avx512_storage, 8>());

static_assert(population_constants<short_width, std::uint8_t, 16>());
static_assert(population_constants<short_width, std::uint16_t, 8>());
static_assert(shuffle_constants<short_width, 2>());
static_assert(population_constants<short_width, std::uint8_t, 32>());
static_assert(population_constants<short_width, std::uint16_t, 16>());
static_assert(shuffle_constants<short_width, 4>());

static_assert(population_constants<full_width, std::uint8_t, 64>());
static_assert(population_constants<full_width, std::uint16_t, 32>());
static_assert(shuffle_constants<full_width, 8>());

static_assert(population_constants<broad, std::uint8_t, 16>());
static_assert(population_constants<broad, std::uint16_t, 8>());
static_assert(shuffle_constants<broad, 2>());
static_assert(population_constants<broad, std::uint8_t, 32>());
static_assert(population_constants<broad, std::uint16_t, 16>());
static_assert(shuffle_constants<broad, 4>());
static_assert(population_constants<broad, std::uint8_t, 64>());
static_assert(population_constants<broad, std::uint16_t, 32>());
static_assert(shuffle_constants<broad, 8>());

// Literal anchors verify qword routing, low/high source bits and ignored control bits.
constexpr std::array<std::uint64_t, 2> anchor_words{1, 0x8000000000000000ull};
constexpr std::array<std::uint8_t, 16> anchor_controls{
  0, 64, 128, 192, 1, 63, 127, 255, 63, 127, 191, 255, 0, 64, 128, 192};
static_assert(shuffle_constant<sse2_storage>(anchor_words, anchor_controls, 0x8105) ==
  std::array<std::uint64_t, 2>{0x0f0f, 0x0105});

// The literal target stays visible at each runtime entry from baseline code.
template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512bitalg")
void population_512(T * result, T const * input, T const * source, std::uint64_t mask) noexcept {
  using vector_type = native::simd<T, N, full_width>;
  auto value = vector_type::load(input);
  auto prior = vector_type::load(source);
  auto active = native::predicate<N, full_width>::from_bitset(mask);
  if constexpr (sizeof(T) == 1) {
    native::vpopcntb<full_width>(value).store(result);
    native::mask_vpopcntb<full_width>(prior, active, value).store(result + N);
    native::maskz_vpopcntb<full_width>(active, value).store(result + 2 * N);
  } else {
    native::vpopcntw<full_width>(value).store(result);
    native::mask_vpopcntw<full_width>(prior, active, value).store(result + N);
    native::maskz_vpopcntw<full_width>(active, value).store(result + 2 * N);
  }
}

template<std::size_t Q>
native_noinline native_target("avx512f,avx512bw,avx512bitalg")
void shuffle_512(std::uint64_t * result, std::uint64_t const * input,
  std::uint8_t const * control, std::uint64_t mask) noexcept {
  auto value = native::simd<std::uint64_t, Q, full_width>::load(input);
  auto selectors = native::simd<std::uint8_t, Q * 8, full_width>::load(control);
  auto active = native::predicate<Q * 8, full_width>::from_bitset(mask);
  result[0] = native::vpshufbitqmb<full_width>(value, selectors).to_bitset();
  result[1] = native::mask_vpshufbitqmb<full_width>(active, value, selectors).to_bitset();
}

// The literal target stays visible at each runtime entry from baseline code.
template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
void population_vl(T * result, T const * input, T const * source, std::uint64_t mask) noexcept {
  using vector_type = native::simd<T, N, short_width>;
  auto value = vector_type::load(input);
  auto prior = vector_type::load(source);
  auto active = native::predicate<N, short_width>::from_bitset(mask);
  if constexpr (sizeof(T) == 1) {
    native::vpopcntb<short_width>(value).store(result);
    native::mask_vpopcntb<short_width>(prior, active, value).store(result + N);
    native::maskz_vpopcntb<short_width>(active, value).store(result + 2 * N);
  } else {
    native::vpopcntw<short_width>(value).store(result);
    native::mask_vpopcntw<short_width>(prior, active, value).store(result + N);
    native::maskz_vpopcntw<short_width>(active, value).store(result + 2 * N);
  }
}

template<std::size_t Q>
native_noinline native_target("avx512f,avx512bw,avx512bitalg,avx512vl")
void shuffle_vl(std::uint64_t * result, std::uint64_t const * input,
  std::uint8_t const * control, std::uint64_t mask) noexcept {
  auto value = native::simd<std::uint64_t, Q, short_width>::load(input);
  auto selectors = native::simd<std::uint8_t, Q * 8, short_width>::load(control);
  auto active = native::predicate<Q * 8, short_width>::from_bitset(mask);
  result[0] = native::vpshufbitqmb<short_width>(value, selectors).to_bitset();
  result[1] = native::mask_vpshufbitqmb<short_width>(active, value, selectors).to_bitset();
}

// The literal target stays visible at each runtime entry from baseline code.
template<class T, std::size_t N>
native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512bitalg")
void population_broad(T * result, T const * input, T const * source, std::uint64_t mask) noexcept {
  using vector_type = native::simd<T, N, broad>;
  auto value = vector_type::load(input);
  auto prior = vector_type::load(source);
  auto active = native::predicate<N, broad>::from_bitset(mask);
  if constexpr (sizeof(T) == 1) {
    native::vpopcntb<broad>(value).store(result);
    native::mask_vpopcntb<broad>(prior, active, value).store(result + N);
    native::maskz_vpopcntb<broad>(active, value).store(result + 2 * N);
  } else {
    native::vpopcntw<broad>(value).store(result);
    native::mask_vpopcntw<broad>(prior, active, value).store(result + N);
    native::maskz_vpopcntw<broad>(active, value).store(result + 2 * N);
  }
}

template<std::size_t Q>
native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512bitalg")
void shuffle_broad(std::uint64_t * result, std::uint64_t const * input,
  std::uint8_t const * control, std::uint64_t mask) noexcept {
  auto value = native::simd<std::uint64_t, Q, broad>::load(input);
  auto selectors = native::simd<std::uint8_t, Q * 8, broad>::load(control);
  auto active = native::predicate<Q * 8, broad>::from_bitset(mask);
  result[0] = native::vpshufbitqmb<broad>(value, selectors).to_bitset();
  result[1] = native::mask_vpshufbitqmb<broad>(active, value, selectors).to_bitset();
}

inline std::uint64_t random_word(std::uint64_t & state) noexcept {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

template<class T, std::size_t N, bool Broad = false>
bool check_population_case(std::array<T, N> const & input, std::array<T, N> const & source,
  std::uint64_t mask) {
  std::array<T, N * 3> actual{};
  if constexpr (Broad) {
    population_broad<T, N>(actual.data(), input.data(), source.data(), mask);
  } else if constexpr (sizeof(T) * N == 64) {
    population_512<T, N>(actual.data(), input.data(), source.data(), mask);
  } else {
    population_vl<T, N>(actual.data(), input.data(), source.data(), mask);
  }
  auto expected = population_reference(input, source, mask);
  for (unsigned form = 0; form < 3; ++form) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      if (actual[form * N + lane] != expected[form][lane]) {
        std::printf("BITALG population failure: bytes=%zu lane-bits=%zu form=%u lane=%zu mask=%llx\n",
          sizeof(T) * N, sizeof(T) * 8, form, lane, static_cast<unsigned long long>(mask));
        return false;
      }
    }
  }
  return true;
}

template<class T, std::size_t N, bool Broad = false>
bool check_population(std::uint64_t & state) {
  std::array<T, N> input{};
  std::array<T, N> source{};
  // Exhaust every byte/word input value at each width. Lane rotation and masks
  // exercise the byte/word association independently of the source pattern.
  for (unsigned base = 0; base < (1u << (sizeof(T) * 8)); base += N) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input[lane] = static_cast<T>(base + lane);
      source[lane] = static_cast<T>(0x81 + lane * 17);
    }
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{0x5555555555555555}}) {
      if (!check_population_case<T, N, Broad>(input, source, mask)) {
        return false;
      }
    }
  }
  input.fill(static_cast<T>(~T{0}));
  for (std::size_t lane = 0; lane < N; ++lane) {
    auto mask = std::uint64_t{1} << lane;
    if (!check_population_case<T, N, Broad>(input, source, mask) ||
        !check_population_case<T, N, Broad>(input, source, ~mask)) {
      return false;
    }
  }
  for (unsigned trial = 0; trial < 2048; ++trial) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input[lane] = static_cast<T>(random_word(state));
      source[lane] = static_cast<T>(random_word(state));
    }
    if (!check_population_case<T, N, Broad>(input, source, random_word(state))) {
      return false;
    }
  }
  return true;
}

template<std::size_t Q, bool Broad = false>
bool check_shuffle_case(std::array<std::uint64_t, Q> const & input,
  std::array<std::uint8_t, Q * 8> const & control, std::uint64_t mask) {
  std::array<std::uint64_t, 2> actual{};
  if constexpr (Broad) {
    shuffle_broad<Q>(actual.data(), input.data(), control.data(), mask);
  } else if constexpr (Q == 8) {
    shuffle_512<Q>(actual.data(), input.data(), control.data(), mask);
  } else {
    shuffle_vl<Q>(actual.data(), input.data(), control.data(), mask);
  }
  if (actual != shuffle_reference(input, control, mask)) {
    std::printf("BITALG shuffle failure: qwords=%zu mask=%llx plain=%llx masked=%llx\n",
      Q, static_cast<unsigned long long>(mask), static_cast<unsigned long long>(actual[0]),
      static_cast<unsigned long long>(actual[1]));
    return false;
  }
  return true;
}

template<std::size_t Q, bool Broad = false>
bool check_shuffle(std::uint64_t & state) {
  std::array<std::uint64_t, Q> input{};
  std::array<std::uint8_t, Q * 8> control{};
  for (unsigned pattern = 0; pattern < 256; ++pattern) {
    for (std::size_t qword = 0; qword < Q; ++qword) {
      input[qword] = 0x8000000102040810ull ^ (0x0123456789abcdefull * qword);
    }
    for (std::size_t lane = 0; lane < Q * 8; ++lane) {
      control[lane] = static_cast<std::uint8_t>(pattern + lane * 7);
    }
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{0x5555555555555555}}) {
      if (!check_shuffle_case<Q, Broad>(input, control, mask)) {
        return false;
      }
    }
  }
  // Distinct qwords and walking output masks expose 64/128/256-bit routing errors.
  for (unsigned bit = 0; bit < 64; ++bit) {
    for (std::size_t qword = 0; qword < Q; ++qword) {
      input[qword] = std::uint64_t{1} << ((bit + qword) % 64);
      for (unsigned byte = 0; byte < 8; ++byte) {
        control[qword * 8 + byte] = static_cast<std::uint8_t>(((bit + qword) % 64) | 0xc0);
      }
    }
    for (std::size_t lane = 0; lane < Q * 8; ++lane) {
      auto mask = std::uint64_t{1} << lane;
      if (!check_shuffle_case<Q, Broad>(input, control, mask) ||
          !check_shuffle_case<Q, Broad>(input, control, ~mask)) {
        return false;
      }
    }
  }
  for (unsigned trial = 0; trial < 2048; ++trial) {
    for (auto & word : input) {
      word = random_word(state);
    }
    for (auto & selector : control) {
      selector = static_cast<std::uint8_t>(random_word(state));
    }
    if (!check_shuffle_case<Q, Broad>(input, control, random_word(state))) {
      return false;
    }
  }
  return true;
}

extern "C" native_noinline unsigned long long
native_bitalg_baseline_import(unsigned long long value) noexcept {
  return (value >> 3) ^ (value + 17);
}

int main(int argc, char **) {
  auto probe = static_cast<unsigned long long>(argc);
  if (native_bitalg_baseline_import(probe) != ((probe >> 3) ^ (probe + 17))) {
    return 1;
  }
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, full_width).admitted()) {
    std::puts("SKIP BITALG: CPU features or OS ZMM state unavailable; constexpr checks passed.");
    return 77;
  }
  std::uint64_t state = 0x9e3779b97f4a7c15ull;
  if (!check_population<std::uint8_t, 64>(state) ||
      !check_population<std::uint16_t, 32>(state) || !check_shuffle<8>(state)) {
    return 2;
  }
  if (native::classify_isa(cpu, short_width).admitted()) {
    if (!check_population<std::uint8_t, 16>(state) ||
        !check_population<std::uint8_t, 32>(state) ||
        !check_population<std::uint16_t, 8>(state) ||
        !check_population<std::uint16_t, 16>(state) ||
        !check_shuffle<2>(state) || !check_shuffle<4>(state)) {
      return 3;
    }
  } else {
    std::puts("SKIP BITALG narrow forms: AVX512VL unavailable.");
  }
  if (native::classify_isa(cpu, broad).admitted()) {
    if (!check_population<std::uint8_t, 16, true>(state) ||
        !check_population<std::uint8_t, 32, true>(state) ||
        !check_population<std::uint8_t, 64, true>(state) ||
        !check_population<std::uint16_t, 8, true>(state) ||
        !check_population<std::uint16_t, 16, true>(state) ||
        !check_population<std::uint16_t, 32, true>(state) ||
        !check_shuffle<2, true>(state) || !check_shuffle<4, true>(state) ||
        !check_shuffle<8, true>(state)) {
      return 4;
    }
  } else {
    std::puts("SKIP BITALG broader-tag forms: AVX512DQ unavailable.");
  }
  std::puts("BITALG runtime lane, bit-selection and mask checks passed.");
  return 0;
}
