// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <memory>
#include "support/guarded_pages.h"

#define SIMD_TEST_IMPORT SIMD_TEST_INTERFACE
#include "support/profile.h"
import simd.wide;

namespace {
  constexpr std::size_t lanes = SIMD_TEST_PROFILE / 32;
  using F = test_vec<float, lanes>;
  static_assert(sizeof(F) == sizeof(float) * lanes);
  static_assert(std::is_trivially_copyable_v<F>);
  using packed_values = simd::wide<F,3>;
  static_assert(std::same_as<decltype(simd::exp(std::declval<packed_values const &>())), packed_values>);
  template<std::size_t N> bool tails() {
    using R = test_vec<float, N>;
    simd::test::guarded_pages input_page, output_page;
    for (std::size_t n = 0; n <= N; ++n) {
      auto * input = reinterpret_cast<float *>(input_page.end()) - n;
      auto * output = reinterpret_cast<float *>(output_page.end()) - n;
      for (std::size_t i = 0; i < n; ++i) {
        std::construct_at(input + i, float(int(i) - 3));
        std::construct_at(output + i, 19.0f);
      }
      std::construct_at(output - 1, 19.0f);
      auto value = R::load_partial(input, n, 17);
      std::array<float, N> loaded;
      value.storeu(loaded.data());
      for (std::size_t i = 0; i < N; ++i)
        if (loaded[i] != (i < n ? input[i] : 17)) return false;
      value.store_partial(output, n);
      if (output[-1] != 19) return false;
      for (std::size_t i = 0; i < n; ++i)
        if (output[i] != input[i]) return false;
    }
    auto empty = R::load_partial(static_cast<float const *>(nullptr), 0, 17);
    empty.store_partial(static_cast<float *>(nullptr), 0);
    std::array<float, N> loaded;
    empty.storeu(loaded.data());
    for (float value : loaded) if (value != 17) return false;
    return true;
  }

}


extern "C" std::size_t SIMD_TEST_ENTRY(std::uint32_t * out, std::size_t capacity) {
  if (capacity < profile_test::words || !out || !tails<4>() || !tails<lanes>()) return 0;
  std::array<float, profile_test::count> input;
  for (std::size_t i=0; i<input.size(); ++i) input[i] = float(int(i)-48)*.25f;
  simd::wide<F,profile_test::count/lanes> packed;
  for (std::size_t i=0; i<packed.registers.size(); ++i)
    packed.registers[i] = F::loadu(input.data()+i*lanes);
  auto exponential = simd::exp(packed);
  for (std::size_t i=0; i<packed.registers.size(); ++i) {
    auto offset=i*lanes; auto x=packed.registers[i];
    auto put=[&](std::size_t column,F value) {value.store_bits(out+column*profile_test::count+offset);};
    put(0,x+F(.5f)); put(1,x*F(.5f)); put(2,fma(x,F(.5f),F(-1.f)));
    put(3,select(x>F(0.f),x,-x)); put(4,exponential.registers[i]);
    using U=test_vec<std::uint32_t,lanes>;
    ((x.bits()+U(3u))^U(0x9e3779b9u)).store(out+5*profile_test::count+offset);
    put(6,masked_scaleb_zero(x>F(0.f),x,F(-1.f)));
    put(7,sqrt(abs(x)));
    std::array<std::uint32_t,lanes> direct;
    exp(x).store_bits(direct.data());
    for (std::size_t j=0;j<lanes;++j) {
      auto lane=offset+j;float value=input[lane];
      if (out[4*profile_test::count+lane]!=direct[j]) return 0;
      std::array expected{value+.5f,value*.5f,std::fma(value,.5f,-1.f),
        value>0.f?value:-value,value>0.f?value*.5f:0.f,std::sqrt(std::abs(value))};
      constexpr std::size_t columns[]{0,1,2,3,6,7};
      for (std::size_t k=0;k<expected.size();++k)
        if (out[columns[k]*profile_test::count+lane]!=std::bit_cast<std::uint32_t>(expected[k])) return 0;
      if (out[5*profile_test::count+lane]!=((std::bit_cast<std::uint32_t>(value)+3u)^0x9e3779b9u)) return 0;
    }
  }
  return profile_test::words;
}
