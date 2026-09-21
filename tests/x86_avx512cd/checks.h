// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <immintrin.h>
#include <native/attributes.h>

inline constexpr auto cd512 = native::target_features<native::x86>("avx512f,avx512cd");
inline constexpr auto cdvl = native::target_features<native::x86>("avx512f,avx512cd,avx512vl");

template<native::isa<native::x86> A, class T, std::size_t N>
concept cd_shape = requires(native::simd<T, N, A> v, native::predicate<N, A> m) {
  { native::vpconflictd<A>(v) } noexcept -> std::same_as<decltype(v)>;
  { native::mask_vpconflictd<A>(v, m, v) } noexcept -> std::same_as<decltype(v)>;
  { native::maskz_vpconflictd<A>(m, v) } noexcept -> std::same_as<decltype(v)>;
  { native::vplzcntd<A>(v) } noexcept -> std::same_as<decltype(v)>;
};
static_assert(cd_shape<cdvl, std::uint32_t, 4> && cd_shape<cdvl, std::uint32_t, 8> &&
              cd_shape<cd512, std::uint32_t, 16>);
static_assert(!cd_shape<cdvl, std::int32_t, 4> && !cd_shape<cdvl, std::uint32_t, 2>);
static_assert(!cd_shape<native::isa<native::x86>{}, std::uint32_t, 4>);
template<class V>
concept cd_raw = requires(V v) { native::vpconflictd<cdvl>(v); };
static_assert(!cd_raw<__m128i> && !cd_raw<__m256i> && !cd_raw<__m512i>);
template<class M>
concept cd_mask = requires(native::simd<std::uint32_t, 4, cdvl> v, M m) {
  native::mask_vpconflictd<cdvl>(v, m, v);
};
static_assert(!cd_mask<unsigned> && !cd_mask<native::predicate<8, cdvl>> &&
              !cd_mask<native::predicate<4, cd512>>);

template<class T, std::size_t N>
constexpr std::array<std::array<T, N>, 6>
reference(std::array<T, N> const & input, std::array<T, N> const & source, std::uint64_t mask) {
  std::array<std::array<T, N>, 6> result{};
  for (std::size_t lane = 0; lane < N; ++lane) {
    T conflicts = 0;
    T zeros = 0;
    // Earlier lanes are considered even when their destination mask bit is zero.
    for (std::size_t earlier = 0; earlier < lane; ++earlier) {
      if (input[earlier] == input[lane]) {
        conflicts += T{1} << earlier;
      }
    }
    for (unsigned bit = sizeof(T) * 8; bit && !((input[lane] >> (bit - 1)) & T{1}); --bit) {
      ++zeros;
    }
    bool active = (mask >> lane) & 1;
    result[0][lane] = conflicts;
    result[1][lane] = active ? conflicts : source[lane];
    result[2][lane] = active ? conflicts : 0;
    result[3][lane] = zeros;
    result[4][lane] = active ? zeros : source[lane];
    result[5][lane] = active ? zeros : 0;
  }
  return result;
}

// Keep both the native runtime calls and constant evaluation on the same public API.
template<native::isa<native::x86> A, class T, std::size_t N>
consteval auto calculate(std::array<T, N> input, std::array<T, N> source, std::uint64_t mask) {
  using vector_type = native::simd<T, N, A>;
  auto value = vector_type::load(input.data());
  auto source_value = vector_type::load(source.data());
  auto lane_mask = native::predicate<N, A>::from_bitset(mask);
  std::array<std::array<T, N>, 6> result{};
  if constexpr (sizeof(T) == 4) {
    native::vpconflictd<A>(value).store(result[0].data());
    native::mask_vpconflictd<A>(source_value, lane_mask, value).store(result[1].data());
    native::maskz_vpconflictd<A>(lane_mask, value).store(result[2].data());
    native::vplzcntd<A>(value).store(result[3].data());
    native::mask_vplzcntd<A>(source_value, lane_mask, value).store(result[4].data());
    native::maskz_vplzcntd<A>(lane_mask, value).store(result[5].data());
  } else {
    native::vpconflictq<A>(value).store(result[0].data());
    native::mask_vpconflictq<A>(source_value, lane_mask, value).store(result[1].data());
    native::maskz_vpconflictq<A>(lane_mask, value).store(result[2].data());
    native::vplzcntq<A>(value).store(result[3].data());
    native::mask_vplzcntq<A>(source_value, lane_mask, value).store(result[4].data());
    native::maskz_vplzcntq<A>(lane_mask, value).store(result[5].data());
  }
  return result;
}

template<native::isa<native::x86> A, class T, std::size_t N>
consteval bool constants() {
  std::array<T, N> input{};
  std::array<T, N> source{};
  for (std::size_t lane = 0; lane < N; ++lane) {
    source[lane] = static_cast<T>(99 + lane);
  }
  for (unsigned pattern = 0; pattern < sizeof(T) * 8 + 4; ++pattern) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      if (pattern == 0) {
        input[lane] = 0;
      } else if (pattern == 1) {
        input[lane] = static_cast<T>(lane);
      } else if (pattern == 2) {
        input[lane] = static_cast<T>(lane % 3);
      } else if (pattern == 3) {
        input[lane] = ~T{0};
      } else {
        input[lane] = T{1} << ((pattern - 4 + lane) % (sizeof(T) * 8));
      }
    }
    for (auto mask : {std::uint64_t{0},
                      ~std::uint64_t{0},
                      std::uint64_t{0xaaaa},
                      std::uint64_t{1} << (N - 1)}) {
      if (calculate<A>(input, source, mask) != reference(input, source, mask)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr auto sse2_storage = native::target_features<native::x86>("sse2");
inline constexpr auto avx_storage = native::target_features<native::x86>("avx");
inline constexpr auto avx512_storage = native::target_features<native::x86>("avx512f");

static_assert(constants<sse2_storage, std::uint32_t, 4>());
static_assert(constants<sse2_storage, std::uint64_t, 2>());
static_assert(constants<avx_storage, std::uint32_t, 8>());
static_assert(constants<avx_storage, std::uint64_t, 4>());
static_assert(constants<avx512_storage, std::uint32_t, 16>());
static_assert(constants<avx512_storage, std::uint64_t, 8>());
static_assert(constants<cdvl, std::uint32_t, 4>());
static_assert(constants<cdvl, std::uint32_t, 8>());
static_assert(constants<cdvl, std::uint32_t, 16>());
static_assert(constants<cdvl, std::uint64_t, 2>());
static_assert(constants<cdvl, std::uint64_t, 4>());
static_assert(constants<cdvl, std::uint64_t, 8>());
static_assert(constants<cd512, std::uint32_t, 16>());
static_assert(constants<cd512, std::uint64_t, 8>());

// Each native entry keeps its literal target visible at the baseline call boundary.
template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512cd")
void invoke_512(T * result, T const * input, T const * source, std::uint64_t mask) noexcept {
  using vector_type = native::simd<T, N, cd512>;
  auto value = vector_type::load(input);
  auto source_value = vector_type::load(source);
  auto lane_mask = native::predicate<N, cd512>::from_bitset(mask);
  if constexpr (sizeof(T) == 4) {
    native::vpconflictd<cd512>(value).store(result);
    native::mask_vpconflictd<cd512>(source_value, lane_mask, value).store(result + N);
    native::maskz_vpconflictd<cd512>(lane_mask, value).store(result + 2 * N);
    native::vplzcntd<cd512>(value).store(result + 3 * N);
    native::mask_vplzcntd<cd512>(source_value, lane_mask, value).store(result + 4 * N);
    native::maskz_vplzcntd<cd512>(lane_mask, value).store(result + 5 * N);
  } else {
    native::vpconflictq<cd512>(value).store(result);
    native::mask_vpconflictq<cd512>(source_value, lane_mask, value).store(result + N);
    native::maskz_vpconflictq<cd512>(lane_mask, value).store(result + 2 * N);
    native::vplzcntq<cd512>(value).store(result + 3 * N);
    native::mask_vplzcntq<cd512>(source_value, lane_mask, value).store(result + 4 * N);
    native::maskz_vplzcntq<cd512>(lane_mask, value).store(result + 5 * N);
  }
}

template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512cd,avx512vl")
void invoke_vl(T * result, T const * input, T const * source, std::uint64_t mask) noexcept {
  using vector_type = native::simd<T, N, cdvl>;
  auto value = vector_type::load(input);
  auto source_value = vector_type::load(source);
  auto lane_mask = native::predicate<N, cdvl>::from_bitset(mask);
  if constexpr (sizeof(T) == 4) {
    native::vpconflictd<cdvl>(value).store(result);
    native::mask_vpconflictd<cdvl>(source_value, lane_mask, value).store(result + N);
    native::maskz_vpconflictd<cdvl>(lane_mask, value).store(result + 2 * N);
    native::vplzcntd<cdvl>(value).store(result + 3 * N);
    native::mask_vplzcntd<cdvl>(source_value, lane_mask, value).store(result + 4 * N);
    native::maskz_vplzcntd<cdvl>(lane_mask, value).store(result + 5 * N);
  } else {
    native::vpconflictq<cdvl>(value).store(result);
    native::mask_vpconflictq<cdvl>(source_value, lane_mask, value).store(result + N);
    native::maskz_vpconflictq<cdvl>(lane_mask, value).store(result + 2 * N);
    native::vplzcntq<cdvl>(value).store(result + 3 * N);
    native::mask_vplzcntq<cdvl>(source_value, lane_mask, value).store(result + 4 * N);
    native::maskz_vplzcntq<cdvl>(lane_mask, value).store(result + 5 * N);
  }
}

inline std::uint64_t random_bits(std::uint64_t & state) {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

template<class T, std::size_t N>
bool check_case(std::array<T, N> const & input, std::array<T, N> const & source,
                std::uint64_t mask) {
  std::array<T, 6 * N> actual{};
  if constexpr (sizeof(T) * N == 64) {
    invoke_512<T, N>(actual.data(), input.data(), source.data(), mask);
  } else {
    invoke_vl<T, N>(actual.data(), input.data(), source.data(), mask);
  }
  auto expected = reference(input, source, mask);
  for (std::size_t form = 0; form < 6; ++form) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      if (actual[form * N + lane] != expected[form][lane]) {
        std::printf("AVX512CD mismatch width=%zu form=%zu lane=%zu mask=%llx\n",
                    sizeof(T) * N * 8,
                    form,
                    lane,
                    static_cast<unsigned long long>(mask));
        return false;
      }
    }
  }
  return true;
}

template<class T, std::size_t N>
bool check_vectors(std::uint64_t & state) {
  std::array<T, N> input{};
  std::array<T, N> source{};
  for (auto & lane : source) {
    lane = static_cast<T>(random_bits(state));
  }
  for (unsigned pattern = 0; pattern < sizeof(T) * 8 + 4; ++pattern) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      if (pattern == 0) {
        input[lane] = 0;
      } else if (pattern == 1) {
        input[lane] = static_cast<T>(lane);
      } else if (pattern == 2) {
        input[lane] = static_cast<T>(lane % 3);
      } else if (pattern == 3) {
        input[lane] = ~T{0};
      } else {
        input[lane] = T{1} << ((pattern - 4 + lane) % (sizeof(T) * 8));
      }
    }
    if (!check_case(input, source, 0) || !check_case(input, source, ~std::uint64_t{0}) ||
        !check_case(input, source, 0xaaaa) || !check_case(input, source, 0x5555)) {
      return false;
    }
    for (unsigned bit = 0; bit < 16; ++bit) {
      if (!check_case(input, source, std::uint64_t{1} << bit) ||
          !check_case(input, source, ~(std::uint64_t{1} << bit))) {
        return false;
      }
    }
  }
  // Small-domain inputs force duplicates across every 128-bit boundary.
  for (unsigned trial = 0; trial < 2048; ++trial) {
    for (auto & lane : input) {
      lane = static_cast<T>(random_bits(state) & (trial & 1 ? 7 : ~std::uint64_t{0}));
    }
    for (auto & lane : source) {
      lane = static_cast<T>(random_bits(state));
    }
    if (!check_case(input, source, random_bits(state))) {
      return false;
    }
  }
  return true;
}
