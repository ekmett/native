// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#if NATIVE_SCALEB_HEADERS
#include <native/simd.h>
#include <native/wide_math.h>
#else
import native.math;
#endif

template<class V> concept plain = requires(V a) { { native::scaleb(a,a) } -> std::same_as<V>; };
template<class V,class M> concept masked = requires(V a,M m) {
  { native::masked_scaleb(m,a,a,a) } -> std::same_as<V>;
  { native::masked_scaleb_zero(m,a,a) } -> std::same_as<V>;
};
template<class V> concept wide_plain = requires(V a) { wide::scaleb(a,a); };
template<class V,class M> concept wide_masked = requires(V a,M m) {
  wide::masked_scaleb(m,a,a,a); wide::masked_scaleb_zero(m,a,a);
};
template<native::isa<> A,std::size_t N,bool Expected>
consteval bool admitted() {
  using V=native::simd<float,N,A>;
  using M=typename V::mask_type;
  using VM=typename V::vector_mask_type;
  static_assert(plain<V> == Expected && masked<V,M> == Expected && masked<V,VM> == Expected);
  static_assert(!wide_plain<V> && !wide_masked<V,M>);
  static_assert(wide_plain<std::array<V,2>> == Expected);
  static_assert(wide_masked<std::array<V,2>,std::array<M,2>> == Expected);
#if !NATIVE_SCALEB_HEADERS
  // Lifted primitives accept standard arrays; the public wide record is not
  // implicitly converted into a batch, even on a native scaling profile.
  static_assert(!wide_plain<native::wide<V,2>>);
  static_assert(!wide_masked<native::wide<V,2>,native::wide<M,2>>);
#endif
  if constexpr(Expected) {
    // Exercise native-admitted constant semantics, including inactive payloads.
    auto value=V::from_bits(1), exponent=V(1.f), prior=V::from_bits(0x7f812345u);
    auto full=native::scaleb(value,exponent);
    auto merge=native::masked_scaleb(M(false),prior,value,exponent);
    auto zero=native::masked_scaleb_zero(M(false),value,exponent);
    std::array<std::uint32_t,N> f{},m{},z{};
    full.store_bits(f.data());merge.store_bits(m.data());zero.store_bits(z.data());
    for(std::size_t i=0;i<N;++i) if(f[i]!=2 || m[i]!=0x7f812345u || z[i]!=0) return false;
    if constexpr(N==2 || N==3) {
      for(auto result:{full,merge,zero}) {
        auto words=std::bit_cast<std::array<std::uint32_t,4>>(result.to_native());
        for(std::size_t i=N;i<4;++i) if(words[i]!=0) return false;
      }
    }
    auto fractional=native::scaleb(V(1.f),V(-.5f));
    fractional.store_bits(f.data());
    for(auto word:f) if(word!=0x3f000000u) return false;
  }
  return true;
}
static_assert(admitted<native::scalar,1,false>());
static_assert(!wide_plain<float> && !wide_masked<float,bool>);
#if NATIVE_HOST_X86
static_assert(admitted<native::avx2,1,false>() && admitted<native::avx2,2,false>());
static_assert(admitted<native::avx2,3,false>() && admitted<native::avx2,4,false>());
static_assert(admitted<native::avx2,8,false>());
constexpr auto no_vl=native::target_features<native::x86>("avx2,fma,avx512f,avx512dq");
static_assert(admitted<no_vl,1,true>() && admitted<no_vl,16,true>());
static_assert(admitted<no_vl,2,false>() && admitted<no_vl,3,false>());
static_assert(admitted<no_vl,4,false>() && admitted<no_vl,8,false>());
static_assert(admitted<native::avx512,1,true>() && admitted<native::avx512,2,true>());
static_assert(admitted<native::avx512,3,true>() && admitted<native::avx512,4,true>());
static_assert(admitted<native::avx512,8,true>() && admitted<native::avx512,16,true>());
#elif NATIVE_HOST_NEON
static_assert(admitted<native::neon,1,false>() && admitted<native::neon,2,false>());
static_assert(admitted<native::neon,3,false>() && admitted<native::neon,4,false>());
#elif NATIVE_HOST_WASM
constexpr auto wasm=native::target_features<native::wasm>("simd128");
static_assert(admitted<wasm,4,false>());
#endif
int main() {}
