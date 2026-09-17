// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cfenv>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "../core_regression/support/fp_environment.h"
#if SIMD_ROUNDING_HEADER
#define SIMD_PROFILE SIMD_ROUNDING_PROFILE
#include <simd/vec.h>
import simd.wide;
#else
import simd.scalar;
#if SIMD_ROUNDING_PROFILE == 256
import simd.avx2;
#elif SIMD_ROUNDING_PROFILE == 512
import simd.avx512;
#elif SIMD_ROUNDING_PROFILE == 128
import simd.neon;
#endif
#endif

#if SIMD_ROUNDING_PROFILE == 256
using arch = simd::avx2;
#elif SIMD_ROUNDING_PROFILE == 512
using arch = simd::avx512;
#elif SIMD_ROUNDING_PROFILE == 128
using arch = simd::neon;
#else
using arch = simd::scalar;
#endif
template<std::size_t N> using V = simd::vec<float,N,arch>;
enum class direction { down, up, zero };
static std::size_t checks;

// Integer-only oracle: remove fractional significand bits, then increment the
// retained magnitude only when the directed rounding crosses an integer.
static std::uint32_t oracle(std::uint32_t u,direction d) {
  auto const sign = u & 0x80000000u;
  auto const magnitude = u & 0x7fffffffu;
  if (magnitude >= 0x4b000000u || magnitude == 0) return u;
  bool const away = (d == direction::down && sign) || (d == direction::up && !sign);
  if (magnitude < 0x3f800000u) return sign | (away ? 0x3f800000u : 0u);
  auto const shift = 150u - (magnitude >> 23);
  auto const unit = 1u << shift;
  auto const fraction = unit - 1u;
  auto result = u & ~fraction;
  if (away && (u & fraction)) result += unit;
  return result;
}
static bool nan_word(std::uint32_t u) { return (u & 0x7fffffffu) > 0x7f800000u; }
static void check(std::uint32_t input,float value,direction d) {
  auto const actual = std::bit_cast<std::uint32_t>(value), expected = oracle(input,d);
  if (actual != expected && !(nan_word(actual) && nan_word(expected))) {
    std::fprintf(stderr,"profile=%d op=%u input=%08x got=%08x expected=%08x rounding=%d\n",
      SIMD_ROUNDING_PROFILE,unsigned(d),input,actual,expected,std::fegetround());
    std::abort();
  }
  ++checks;
}
template<std::size_t N> static void check_vector(std::array<std::uint32_t,N> const & words,V<N> value,direction d) {
  std::array<float,N> result{};
  simd::store_simd(result.data(),value);
  for (std::size_t i=0;i<N;++i) check(words[i],result[i],d);
  if constexpr (N == 2 || N == 3) {
    auto const storage = value.to_storage();
    std::array<float,4> padding{};
    simd::store_simd(padding.data(),storage);
    for (std::size_t i=N;i<4;++i) if (std::bit_cast<std::uint32_t>(padding[i]) != 0) std::abort();
  }
}
template<std::size_t N> static void shape(std::vector<std::uint32_t> const & words) {
  static_assert(std::same_as<decltype(simd::floor(V<N>{})),V<N>>);
  static_assert(std::same_as<decltype(simd::ceil(V<N>{})),V<N>>);
  static_assert(std::same_as<decltype(simd::trunc(V<N>{})),V<N>>);
  static_assert(noexcept(simd::floor(V<N>{})) && noexcept(simd::ceil(V<N>{})) && noexcept(simd::trunc(V<N>{})));
  std::array<V<N>,0> empty{};
  (void)simd::floor(empty); (void)simd::ceil(empty); (void)simd::trunc(empty);
  simd::wide<V<N>,0> empty_wide{};
  (void)simd::floor(empty_wide); (void)simd::ceil(empty_wide); (void)simd::trunc(empty_wide);
  for (std::size_t offset=0;offset<words.size();offset+=N) {
    std::array<std::uint32_t,N> u{};
    std::array<float,N> input{};
    for (std::size_t i=0;i<N;++i) input[i]=std::bit_cast<float>(u[i]=words[(offset+i)%words.size()]);
    auto const x = simd::load_simd<V<N>>(input.data());
    check_vector(u,simd::floor(x),direction::down);
    check_vector(u,simd::ceil(x),direction::up);
    check_vector(u,simd::trunc(x),direction::zero);
    // One and several registers exercise the direct array packs and generic
    // module-owned lift, including a scalar element wide separately below.
    std::array<V<N>,1> one{x};
    check_vector(u,simd::floor(one)[0],direction::down);
    check_vector(u,simd::ceil(one)[0],direction::up);
    check_vector(u,simd::trunc(one)[0],direction::zero);
    std::array<V<N>,3> batch{x,x,x};
    auto const a=simd::floor(batch), b=simd::ceil(batch), c=simd::trunc(batch);
    auto const wide=simd::wide<V<N>,3>{batch};
    auto const d=simd::floor(wide), e=simd::ceil(wide), f=simd::trunc(wide);
    for (std::size_t i=0;i<3;++i) {
      check_vector(u,a[i],direction::down); check_vector(u,b[i],direction::up); check_vector(u,c[i],direction::zero);
      check_vector(u,d.registers[i],direction::down); check_vector(u,e.registers[i],direction::up); check_vector(u,f.registers[i],direction::zero);
    }
  }
}
template<class T> concept has_rounding = requires(T x) { simd::floor(x); simd::ceil(x); simd::trunc(x); };
static_assert(!has_rounding<simd::vec<std::int32_t,1,arch>>);
static_assert(!has_rounding<simd::wide<int,2>>);

// Ordinary native object witnesses: input/output memory avoids scalar-wrapper
// ABI attribution. The emitted leaves must contain rounding instructions, not
// calls to a scalar lane loop. The scalar profile remains baseline-compatible.
#define ROUND_LEAF(name, op, lanes) \
extern "C" __declspec(noinline) void name(float * output,float const * input) { \
  simd::store_simd(output,simd::op(simd::load_simd<V<lanes>>(input))); \
}
ROUND_LEAF(round_floor_1,floor,1)
ROUND_LEAF(round_ceil_1,ceil,1)
ROUND_LEAF(round_trunc_1,trunc,1)
#if SIMD_ROUNDING_PROFILE != 0
ROUND_LEAF(round_floor_3,floor,3)
ROUND_LEAF(round_ceil_3,ceil,3)
ROUND_LEAF(round_trunc_3,trunc,3)
ROUND_LEAF(round_floor_4,floor,4)
ROUND_LEAF(round_ceil_4,ceil,4)
ROUND_LEAF(round_trunc_4,trunc,4)
#endif
#if SIMD_ROUNDING_PROFILE == 256 || SIMD_ROUNDING_PROFILE == 512
ROUND_LEAF(round_floor_8,floor,8)
ROUND_LEAF(round_ceil_8,ceil,8)
ROUND_LEAF(round_trunc_8,trunc,8)
#endif
#if SIMD_ROUNDING_PROFILE == 512
ROUND_LEAF(round_floor_16,floor,16)
ROUND_LEAF(round_ceil_16,ceil,16)
ROUND_LEAF(round_trunc_16,trunc,16)
#endif
#undef ROUND_LEAF

int main() {
  std::vector<std::uint32_t> words;
  for (auto u : {0u,1u,0x007fffffu,0x00800000u,0x3effffffu,0x3f000000u,0x3f7fffffu,
      0x3f800000u,0x3f800001u,0x3fc00000u,0x4afffffeu,0x4affffffu,0x4b000000u,
      0x4b000001u,0x7f7fffffu,0x7f800000u,0x7f800001u,0x7fa12345u,0x7fc12345u})
    for (auto sign : {0u,0x80000000u}) words.push_back(u|sign);
  for (std::uint32_t e=127;e<=150;++e)
    for (std::uint32_t delta : {0u,1u,0x7fffffu})
      for (auto sign : {0u,0x80000000u}) words.push_back(sign|(e<<23)|delta);
  std::uint32_t state=0x12345678u;
  for (unsigned i=0;i<4096;++i) {
    state^=state<<13;state^=state>>17;state^=state<<5;words.push_back(state);
  }
  auto const before=simd::test::read_fp_state();
  {
    simd::test::fp_scope scope(simd::test::fp_mode::gradual);
    for (int rounding : {FE_TONEAREST,FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}) {
      if(std::fesetround(rounding)) std::abort();
      auto const control=simd::test::read_fp_state().control;
      shape<1>(words);
#if SIMD_ROUNDING_PROFILE != 0
      shape<2>(words); shape<3>(words); shape<4>(words);
#endif
#if SIMD_ROUNDING_PROFILE == 256 || SIMD_ROUNDING_PROFILE == 512
      shape<8>(words);
#endif
#if SIMD_ROUNDING_PROFILE == 512
      shape<16>(words);
#endif
      for (auto u : words) {
        simd::wide<float,1> x{std::array{std::bit_cast<float>(u)}};
        check(u,simd::floor(x).registers[0],direction::down);
        check(u,simd::ceil(x).registers[0],direction::up);
        check(u,simd::trunc(x).registers[0],direction::zero);
      }
      if(simd::test::read_fp_state().control != control) std::abort();
    }
  }
  if(simd::test::read_fp_state()!=before) std::abort();
  std::printf("profile=%d checks=%zu rounding_modes=4\n",SIMD_ROUNDING_PROFILE,checks);
}
