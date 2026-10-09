// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <limits>
#if NATIVE_POLYFILL_HEADERS
#include <native/simd.h>
#include <native/packing.h>
#else
import native;
#endif
namespace {
  using namespace native;
  constexpr auto permitted=feature_closure(wasm_feature::simd128)|polyfill;
  template<class V> constexpr auto lanes(V x) noexcept {
    std::array<typename V::value_type,V::lanes> result{}; x.store(result.data()); return result;
  }
  template<class V> concept multipliable=requires(V x) { x*x; };
  template<class V> concept fusable=requires(V x) { fma(x,x,x); };
  constexpr auto hardware=feature_closure(wasm_feature::simd128);
  static_assert(!multipliable<simd<std::uint8_t,16,hardware>> && multipliable<simd<std::uint8_t,16,permitted>>);
  static_assert(!fusable<simd<float,4,hardware>> && fusable<simd<float,4,permitted>>);
  template<isa<> A> constexpr bool corpus(unsigned seed=0) {
    using B=simd<std::int8_t,16,A>; using U=simd<std::uint8_t,16,A>;
    using H=simd<std::int16_t,8,A>; using I=simd<std::int32_t,4,A>;
    using F=simd<float,4,A>; using D=simd<double,2,A>;
    if(lanes(U(200)*U(3))[0]!=88 || lanes(narrow_concat<std::uint8_t>(simd<std::uint16_t,8,A>(257),simd<std::uint16_t,8,A>(258)))[15]!=2) return false;
    if(lanes(fma(F(2.f),F(3.f),F(1.f)))[0]!=7.f) return false;
    auto compact=predicate<4,A>::from_bitset(5);
    if(lanes(masked_add_zero(compact,F(2.f),F(1.f)))!=std::array{3.f,0.f,3.f,0.f}) return false;
    auto selected=F(1.f)<F(std::array{2.f,0.f,3.f,0.f});
    auto packed=compress(selected,F(std::array{10.f,20.f,30.f,40.f}));
    if(packed.count!=2 || lanes(packed.value)!=std::array{10.f,30.f,0.f,0.f} ||
       lanes(expand(selected,packed.value,F(-1.f)))!=std::array{10.f,-1.f,30.f,-1.f}) return false;
    if(reduce_add(normal_pow2(F(1.f)))!=8.f || lanes(to_bool(to_predicate(selected)))!=std::array{true,false,true,false}) return false;
    if(lanes(add_sat(B(120),B(20)))[0]!=127 || lanes(sub_sat(U(3),U(4)))[0]!=0 ||
       lanes(average_round(U(2),U(3)))[0]!=3 || lanes(abs(B(-128)))[0]!=-128) return false;
    auto a=H(std::array<std::int16_t,8>{-32768,32767,-9,8,7,6,5,4});
    if(lanes(narrow_sat<std::uint8_t>(a,a))[0]!=0 || lanes(narrow_sat<std::int8_t>(a,a))[1]!=127 ||
       lanes(extend_high(a))[0]!=7 || lanes(multiply_widened_low(a,a))[0]!=1073741824) return false;
    if(lanes(q15mulr_sat(H(-32768),H(-32768)))[0]!=32767 || lanes(dot(H(-32768),H(-32768)))[0]!=std::numeric_limits<std::int32_t>::min()) return false;
    auto index=U(std::array<std::uint8_t,16>{15,0,16,255,1,2,3,4,5,6,7,8,9,10,11,12});
    auto input=U(std::array<std::uint8_t,16>{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
    auto shuffled=shuffle<31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16>(input,input+U(16));
    if(lanes(shuffled)[0]!=31 || lanes(shuffled)[15]!=16 || lanes(swizzle(input,index))[0]!=15 || lanes(swizzle(input,index))[2]!=0) return false;
    auto z=F::load_bits(std::array{0u,0x80000000u,0x7fc12345u,0x3f800000u}.data());
    auto minimum=lanes(native::min(z,F::load_bits(std::array{0x80000000u,0u,0x3f800000u,0x7fc98765u}.data())).bits());
    auto maximum=lanes(native::max(z,F::load_bits(std::array{0x80000000u,0u,0x3f800000u,0x7fc98765u}.data())).bits());
    if(minimum[0]!=0x80000000u || minimum[1]!=0x80000000u || maximum[0]!=0 || maximum[1]!=0 ||
       (minimum[2]&0x7fffffff)<=0x7f800000u || (maximum[3]&0x7fffffff)<=0x7f800000u) return false;
    if(lanes(pmin(z,F(1.f)).bits())[0]!=0 || lanes(pmax(z,F(1.f)).bits())[2]!=0x7fc12345u) return false;
    if(lanes(trunc_sat<std::int32_t>(F(std::array{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity(),-1.5f,3.5f})))[2]!=-1) return false;
    auto converted=convert<float>(D(std::array{1.5,2.5}));
    static_assert(decltype(converted)::lanes==4);
    if(lanes(converted)!=std::array{1.5f,2.5f,0.f,0.f} || lanes(convert<double>(I(std::array{1,2,3,4})))!=std::array{1.,2.}) return false;
    if(bitmask(B(-1))!=65535 || !all(I(1)) || any(I(0))) return false;
    using Tail=simd<std::uint8_t,17,A>;
    auto tail=Tail(unsigned(seed)+3)*Tail(7);
    if(lanes(tail)[16]!=std::uint8_t((seed+3)*7) || lanes(add_sat(Tail(250),Tail(20)))[16]!=255) return false;
    using Logical=simd<std::int16_t,18,A>;
    std::array<std::int16_t,18> ordered{}; for(std::size_t i=0;i<18;++i) ordered[i]=std::int16_t(i+1);
    auto logical=Logical::load(ordered.data()); auto extended=lanes(extend_high(logical));
    auto narrowed=lanes(narrow_sat<std::int8_t>(logical,logical+Logical(30)));
    for(std::size_t i=0;i<9;++i) if(extended[i]!=std::int32_t(i+10)) return false;
    for(std::size_t i=0;i<18;++i) if(narrowed[i]!=i+1 || narrowed[18+i]!=i+31) return false;
    if(lanes(pairwise_add_widened(logical))[8]!=35) return false;
    using LogicalFloat=simd<float,17,A>;
    auto zeros=LogicalFloat::load_bits(std::array<std::uint32_t,17>{}.data());
    if(lanes(native::min(zeros,LogicalFloat(-0.f)).bits())[16]!=0x80000000u) return false;
    auto shifted=Tail(1); shifted<<=imm<9>; shifted>>=1;
    if(lanes(shifted)[16]!=1 || lanes((Tail(1)<<imm<9>))[16]!=2 || lanes(Tail(128)>>9)[16]!=64) return false;
    return true;
  }
  constexpr auto scalar_permitted=isa<>(polyfill);
  static_assert(corpus<scalar_permitted>());
  static_assert(corpus<permitted>());
  template<isa<> A> bool transfers() {
    using I=simd<std::int32_t,4,A>; using H=simd<std::int16_t,8,A>;
    alignas(8) std::array<std::byte,18> source{},output{};
    std::int32_t x=-123; std::memcpy(source.data()+1,&x,4);
    auto p=reinterpret_cast<std::int32_t const *>(source.data()+1);
    if(lanes(load_splat<I>(p))[3]!=-123 || lanes(load_zero<I>(p))!=std::array{-123,0,0,0}) return false;
    auto value=load_lane<2>(p,I(7));
    store_lane<2>(reinterpret_cast<std::int32_t *>(output.data()+1),value);
    std::int32_t y; std::memcpy(&y,output.data()+1,4);
    if(y!=x || output[0]!=std::byte{} || output[5]!=std::byte{}) return false;
    std::array<std::int8_t,8> bytes{-128,-1,0,1,2,3,4,127}; std::memcpy(source.data()+1,bytes.data(),8);
    auto widened=load_widened<std::int16_t,std::int8_t,A>(reinterpret_cast<std::int8_t const *>(source.data()+1));
    return lanes(widened)==std::array<std::int16_t,8>{-128,-1,0,1,2,3,4,127};
  }
}
int main(int argc,char **) {
#if NATIVE_POLYFILL_SCALAR_ONLY
  return !(corpus<scalar_permitted>(argc-1) && transfers<scalar_permitted>());
#else
  return !(corpus<scalar_permitted>(argc-1) && corpus<permitted>(argc-1) && transfers<scalar_permitted>() && transfers<permitted>());
#endif
}
