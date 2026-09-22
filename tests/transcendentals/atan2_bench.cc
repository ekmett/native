// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-FileCopyrightText: 2010-2025 Naoki Shibata and contributors
// SPDX-License-Identifier: (BSD-2-Clause OR Apache-2.0) AND BSL-1.0
// Optional direct-division versus normalized-reciprocal throughput fixture.
// The comparison polynomial derives from SLEEF 3.9.0 atan2kf through FTZ;
// retain the full Boost notice below. This is not a library API.
#include <native/wide.h>
#include <native/wide_math.h>
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstdio>
#include <vector>
#include <cstdint>
#if defined(__x86_64__)
using v=native::simd<float,8,native::avx2>;
#else
using v=native::simd<float,4,native::neon>;
#endif
namespace candidate {
  // Comparison graph: normalized mantissas, three reciprocal Newton steps,
  // integer exponent reconstruction, then the same polynomial and quadrants.
  // Raw subnormal inputs are signed zero; tiny reconstructed ratios are flushed.
  // The timing bank contains ordinary normal operands without tiny ratios.
  template<class V, std::size_t N>
    requires (::wide::detail::binary32_register<V>)
  native_nodiscard native_inline constexpr auto atan2_newton_kernel(
      std::array<V, N> const & y, std::array<V, N> const & x) noexcept {
    namespace w = ::wide;
    auto const c = [&](float value) { return w::constant_like(y, value); };
    auto const yw = w::bits(y), xw = w::bits(x);
    auto const u = [&](std::uint32_t value) { return w::constant_like(yw, value); };
    auto const f = [&](std::uint32_t value) { return c(std::bit_cast<float>(value)); };
    auto ay = w::bit_and(yw, u(0x7fffffffu));
    auto ax = w::bit_and(xw, u(0x7fffffffu));
    ay = w::select(w::cmp_lt(ay, u(0x00800000u)), u(0), ay);
    ax = w::select(w::cmp_lt(ax, u(0x00800000u)), u(0), ax);
    auto const swap = w::cmp_gt(ay, ax);
    auto const negative_x = w::cmp_ne(w::bit_and(xw, u(0x80000000u)), u(0));
    auto const a = w::select(swap, ax, ay);
    auto const b = w::select(swap, ay, ax);
    auto const ma = w::from_bits(w::bit_or(w::bit_and(a, u(0x007fffffu)), u(0x3f800000u)));
    auto const mb = w::bit_or(w::bit_and(b, u(0x007fffffu)), u(0x3f800000u));
    auto const m = w::from_bits(mb);
    auto r = w::from_bits(w::sub(u(0x7ef311c3u), mb));
    auto e = w::detail::madd(w::negate(m), r, c(1.f));
    r = w::detail::madd(r, e, r);
    e = w::detail::madd(w::negate(m), r, c(1.f));
    r = w::detail::madd(r, e, r);
    e = w::detail::madd(w::negate(m), r, c(1.f));
    r = w::detail::madd(r, e, r);
    r = w::mul(ma, r);
    auto word = w::bits(r);
    auto fraction = w::bit_and(word, u(0x007fffffu));
    auto exponent = w::sub(w::add(w::right<23>(word), w::right<23>(a)), w::right<23>(b));
    auto ratio_word = w::select(w::bit_and(w::cmp_gt(exponent, u(0)), w::cmp_lt(exponent, u(255))),
      w::bit_or(w::left<23>(exponent), fraction), w::select(
        w::bit_and(w::cmp_eq(exponent, u(0)), w::cmp_eq(fraction, u(0x007fffffu))), u(0x00800000u), u(0)));
    ratio_word = w::select(w::cmp_gt(ratio_word, u(0x3f800000u)), u(0x3f800000u), ratio_word);
    auto const ratio = w::from_bits(ratio_word);
    auto const tiny = w::cmp_le(w::bits(ratio), u(0x39800000u));
    auto const z = w::mul(ratio, ratio);
    auto h = w::detail::madd(f(0x3b390ccdu), z, f(0xbc82b80du));
    h = w::detail::madd(h, z, f(0x3d2e19b6u));
    h = w::detail::madd(h, z, f(0xbd995ffau));
    h = w::detail::madd(h, z, f(0x3dd9ccf2u));
    h = w::detail::madd(h, z, f(0xbe116f9fu));
    h = w::detail::madd(h, z, f(0x3e4cb9a7u));
    h = w::detail::madd(h, z, f(0xbeaaaa5du));
    auto angle = w::detail::madd(w::mul(z, h), ratio, ratio);
    angle = w::select(tiny, ratio, angle);
    angle = w::select(swap, w::sub(f(0x3fc90fdbu), angle), angle);
    angle = w::select(negative_x, w::sub(f(0x40490fdbu), angle), angle);
    auto result = w::bits(angle);
    auto const axis = w::select(negative_x, u(0x40490fdbu), u(0));
    result = w::select(w::cmp_eq(ax, u(0)), u(0x3fc90fdbu), result);
    result = w::select(w::cmp_eq(ax, u(0x7f800000u)), axis, result);
    result = w::select(w::cmp_eq(ay, u(0x7f800000u)),
      w::select(w::cmp_eq(ax, u(0x7f800000u)),
        w::select(negative_x, u(0x4016cbe4u), u(0x3f490fdbu)), u(0x3fc90fdbu)), result);
    result = w::select(w::cmp_eq(ay, u(0)), axis, result);
    result = w::bit_or(result, w::bit_and(yw, u(0x80000000u)));
    result = w::select(w::bit_or(w::cmp_gt(ay, u(0x7f800000u)),
      w::cmp_gt(ax, u(0x7f800000u))), u(0x7fc00000u), result);
    return w::from_bits(result);
  }
}
namespace fixture {
  template<bool Newton,std::size_t... I>
  native_inline void batch(float* dst,float const* y,float const* x,std::index_sequence<I...>) {
    auto a=std::array{v::load(y+I*v::lanes)...};
    auto b=std::array{v::load(x+I*v::lanes)...};
    if constexpr(Newton) {
      auto r=candidate::atan2_newton_kernel(a,b);
      (r[I].store(dst+I*v::lanes),...);
    } else {
      auto r=math::atan2(a,b);
      (r[I].store(dst+I*v::lanes),...);
    }
  }
  template<std::size_t N,bool Newton>
  native_noinline double measure(float* dst,float const* y,float const* x,std::size_t size,unsigned rounds) {
    auto const start=std::chrono::steady_clock::now();
    for(unsigned r=0;r<rounds;++r) {
      for(std::size_t i=0;i<size;i+=N*v::lanes)batch<Newton>(dst+i,y+i,x+i,std::make_index_sequence<N>{});
      __asm__ volatile("" : : "r"(dst) : "memory");
    }
    auto const finish=std::chrono::steady_clock::now();
    return std::chrono::duration<double,std::nano>(finish-start).count()/(double(size)*rounds);
  }
  template<std::size_t N>
  void compare(float* dst,float const* y,float const* x,std::size_t size) {
    std::array<double,9> direct{},newton{};
    measure<N,false>(dst,y,x,size,16);measure<N,true>(dst,y,x,size,16);
    for(unsigned i=0;i<direct.size();++i) {
      if(i&1) {newton[i]=measure<N,true>(dst,y,x,size,2048);direct[i]=measure<N,false>(dst,y,x,size,2048);}
      else {direct[i]=measure<N,false>(dst,y,x,size,2048);newton[i]=measure<N,true>(dst,y,x,size,2048);}
    }
    std::sort(direct.begin(),direct.end());std::sort(newton.begin(),newton.end());
    std::printf("{\"registers\":%zu,\"lanes\":%zu,\"direct_ns_per_value\":[",N,std::size_t(v::lanes));
    for(unsigned i=0;i<direct.size();++i)std::printf("%s%.6f",i?",":"",direct[i]);
    std::printf("],\"newton_ns_per_value\":[");
    for(unsigned i=0;i<newton.size();++i)std::printf("%s%.6f",i?",":"",newton[i]);
    std::printf("],\"direct_median\":%.6f,\"newton_median\":%.6f,\"ratio_newton_over_direct\":%.6f}\n",direct[4],newton[4],newton[4]/direct[4]);
  }
}
#define FIXTURE_WRAPPERS(N) \
extern "C" void atan2_direct_##N(float* d,float const* y,float const* x) {fixture::batch<false>(d,y,x,std::make_index_sequence<N>{});} \
extern "C" void atan2_newton_##N(float* d,float const* y,float const* x) {fixture::batch<true>(d,y,x,std::make_index_sequence<N>{});}
FIXTURE_WRAPPERS(1)
FIXTURE_WRAPPERS(2)
FIXTURE_WRAPPERS(3)
FIXTURE_WRAPPERS(4)
FIXTURE_WRAPPERS(6)
FIXTURE_WRAPPERS(8)
#undef FIXTURE_WRAPPERS
int main() {
  constexpr std::size_t size=12288;
  std::vector<float> y(size),x(size),dst(size);
  std::uint32_t state=0xbad17413u;
  auto next=[&] {state^=state<<13;state^=state>>17;state^=state<<5;return state;};
  for(std::size_t i=0;i<size;++i) {
    auto yw=next(),xw=next();
    y[i]=std::bit_cast<float>((yw&0x807fffffu)|0x3f000000u);
    x[i]=std::bit_cast<float>((xw&0x807fffffu)|0x3f000000u);
  }
  fixture::compare<1>(dst.data(),y.data(),x.data(),size);
  fixture::compare<2>(dst.data(),y.data(),x.data(),size);
  fixture::compare<3>(dst.data(),y.data(),x.data(),size);
  fixture::compare<4>(dst.data(),y.data(),x.data(),size);
  fixture::compare<6>(dst.data(),y.data(),x.data(),size);
  fixture::compare<8>(dst.data(),y.data(),x.data(),size);
  std::fprintf(stderr,"checksum %.9g\n",dst[42]);
}

/*
Copyright Naoki Shibata and contributors 2010 - 2025.
Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/
