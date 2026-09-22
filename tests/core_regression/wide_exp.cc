#include "support/failure.h"
#include "support/profile.h"
#include "support/fp_environment.h"
// Independent staged split-scale exponential reference. Separate names allow
// this graph to coexist with the pack/native-scale implementation.
import native.wide;
#include "support/profile.h"
#include <limits>

namespace exp_before {
  using namespace test_backend::native;
  using native::wide;
  namespace detail {
    template<float_register V> struct exp_state {
      V x, n, r, y;
      native_inline explicit exp_state(V input) : x(input) {}
    };
    template<float_register V> struct exp_stages {
      template<class ... S> native_inline wide<V, sizeof...(S)> operator()(S ... a) const noexcept {
        ((a.r = min(max(a.x, V(-104)), V(88.72283935546875f))), ...);
        ((a.n = round_even(a.r * V(1.4426950408889634f))), ...);
        ((a.r = fma(a.n, V(-0x1.62e400p-1f), a.r)), ...);
        ((a.r = fma(a.n, V(-0x1.7f7d1cp-20f), a.r)), ...);
        // Integer conversion used finite values. Restore NaNs now, then release
        // the original inputs before Horner evaluation. Clamped finite endpoints
        // naturally produce zero/infinity, including either infinite input.
        ((a.r = select(a.x == a.x, a.r, a.x)), ...);
        ((a.y = V(0x1.a1d714d7b1510dp-13f)), ...);
        ((a.y = fma(a.r, a.y, V(0x1.6da756e670ea6p-10f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.11105b3161a6fp-7f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.5554649b7487fp-5f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.555555c673724p-3f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.0000005c8dd89p-1f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.ffffffffb0eefp-1f))), ...);
        ((a.y = fma(a.r, a.y, V(0x1.ffffffffa70c9p-1f))), ...);
        // Split scaling handles n=128 and subnormal results with normal factors.
        ((a.r = min(max(a.n, V(-126)), V(127))), ...);
        ((a.y = (a.y * normal_pow2(a.n - a.r)) * normal_pow2(a.r)), ...);
#if defined(__aarch64__) || defined(_M_ARM64)
        // ARM's single normal factor is zero for n <= -127. Preserve the
        // independent historical graph everywhere outside that exact band.
        ((a.y = select((a.n <= V(-127)) & (a.x == a.x), V(0.f), a.y)), ...);
#endif
        // General exp explicitly permits infinity from the first n=128 input.
        ((a.y = select(a.x >= V(88.3762664794921875f),
          V(std::numeric_limits<float>::infinity()), a.y)), ...);
        return {{a.y...}};
      }
    };
  }
  namespace detail {
    template<float_register V> struct exp_registers {
      template<class ... X> native_inline auto operator()(X ... x) const noexcept {
        return exp_stages<V>{}(exp_state<V>(x)...);
      }
    };
  }
  // Sollya degree 7, tools/sollya_exp.sollya; coefficients round to FP32.
  // Apply the accepted range policy after the historical graph; no FP control changes.
  // Each dependency stage expands across independent register chains.
  template<float_register V, std::size_t N>
  native_nodiscard native_inline native_pure wide<V, N> exp(wide<V, N> const & input) noexcept {
    return input.apply(detail::exp_registers<V>{});
  }
  template<float_register V> native_nodiscard native_inline native_pure V exp(V x) noexcept { return exp_before::exp(wide<V, 1>{{x}}).registers[0]; }
#if defined(__ARM_NEON)
  // Compatibility for existing channel-packed callers.
  native_nodiscard native_inline native_pure float32x4_t exp(float32x4_t x) noexcept { return exp_before::exp(fp32x4(x)).value; }
  template<std::size_t N> native_nodiscard native_inline native_pure wide<float32x4_t, N> exp(wide<float32x4_t, N> x) noexcept {
    wide<fp32x4, N> input;
    for (std::size_t i = 0; i < N; ++i) input.registers[i] = fp32x4(x.registers[i]);
    auto output = exp(input);
    for (std::size_t i = 0; i < N; ++i) x.registers[i] = output.registers[i].value;
    return x;
  }
#endif
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Evaluates the independent split-scale exponential reference.
 */
#include <bit>
#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
using test_backend::native::fp32x1;
static_assert(std::tuple_size_v<decltype(test_backend::native::exp(std::array<fp32x1,0>{}))> == 0);
bool equal(unsigned a,unsigned b) { return a==b || ((a&0x7fffffffu)>0x7f800000u && (b&0x7fffffffu)>0x7f800000u); }
template<class V> void check(std::vector<unsigned> const & input) {
  constexpr std::size_t count=96/V::lanes;
  for(std::size_t i=0;i<input.size();i+=96) {
    native::wide<V,count> x;
    for(std::size_t k=0;k<count;++k) {
      std::array<unsigned,V::lanes> words{};
      for(std::size_t j=0;j<V::lanes;++j) if(i+k*V::lanes+j<input.size()) words[j]=input[i+k*V::lanes+j];
      x.registers[k]=V::load_bits(words.data());
    }
    auto before=exp_before::exp(x);
    auto after=test_backend::native::exp(x.registers);
    for(std::size_t k=0;k<count;++k) {
      std::array<unsigned,V::lanes>a,b; before.registers[k].store_bits(a.data());after[k].store_bits(b.data());
      for(std::size_t j=0;j<V::lanes;++j) {
        if(!equal(a[j],b[j])) {
          std::cerr<<"mismatch lanes="<<V::lanes<<" input="<<std::hex<<input[std::min(i+k*V::lanes+j,input.size()-1)]<<" old="<<a[j]<<" new="<<b[j]<<'\n';native::test::fail(std::runtime_error("exp mismatch"));
        }
      }
    }
  }
}
int main() {
  std::vector<unsigned> words{0,0x80000000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7f800001u,0xff800001u,0xffffffffu,0x43320000u};
  for(unsigned e=0;e<256;++e)for(unsigned f:{0u,1u,0x003fffffu,0x007ffffeu,0x007fffffu})for(unsigned s:{0u,0x80000000u})words.push_back(s|(e<<23)|f);
  for(unsigned c:{0xc2aeac50u,0xc2af5dc3u,0xc2d00000u,0x42b0c0a6u,0x42b17218u})for(int d=-4096;d<=4096;++d)words.push_back(c+unsigned(d));
  for(unsigned b=0xc2aeac40u;b<0xc2d00010u;++b) words.push_back(b);
  unsigned random=0x379bae12u; for(unsigned i=0;i<1000000;++i){random^=random<<13;random^=random>>17;random^=random<<5;words.push_back(random);}
  auto saved=native::test::read_fp_state();
  for(auto mode:{native::test::fp_mode::gradual,native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);check<fp32x1>(words);
#if defined(__AVX2__) || defined(__ARM_NEON)
    check<test_backend::native::fp32x4>(words);
#endif
#if defined(__AVX2__)
    check<test_backend::native::fp32x8>(words);
#endif
#if defined(__AVX512F__)
    check<test_backend::native::fp32x16>(words);
#endif
    if(!scope.controls_match())native::test::fail(std::runtime_error("controls changed"));
  }
  if(native::test::read_fp_state()!=saved)native::test::fail(std::runtime_error("state not restored"));
  std::cout<<"exact raw baseline comparisons passed; input words="<<words.size()<<" per width per mode\n";
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks direct wide exp against its independent historical graph.
 */
