// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.vnni;
#include "../x86_vnni/checks.h"

namespace vnni_constant_test {
  using namespace vnni_fixture;
  using namespace native_test;
  template<std::size_t N> constexpr auto storage=native::target_features<native::x86>(
    N==16 ? "sse2" : N==32 ? "avx" : "avx512f");
  template<class T,native::isa<native::x86> A,class U,std::size_t N>
  constexpr auto load_bits(std::array<U,N> const& values) {
    auto typed=std::bit_cast<std::array<T,N*sizeof(U)/sizeof(T)>>(values);
    return native::simd<T,N*sizeof(U)/sizeof(T),A>::load(typed.data());
  }
  template<class V,std::size_t N> constexpr void store_bits(V value,std::array<std::uint32_t,N>& out) {
    std::array<typename V::value_type,N> typed{}; value.store(typed.data());
    out=std::bit_cast<std::array<std::uint32_t,N>>(typed);
  }
  template<native::isa<native::x86> A,unsigned Kind,unsigned Mode,std::size_t N>
  consteval auto calculate(inputs<N> in,unsigned bits) {
    outputs<N,Kind==0 ? 4 : 6> result{};
    auto mask=native::predicate<N/4,A>::from_bitset(bits);
    unsigned index=0;
#define CORE(Name,AT,BT) { \
    auto acc=load_bits<std::int32_t,A>(in.accumulator); \
    auto a=load_bits<AT,A>(in.a); auto b=load_bits<BT,A>(in.b); \
    if constexpr(Mode==0) store_bits(native::Name<A>(acc,a,b),result[index]); \
    if constexpr(Mode==1) store_bits(native::mask_##Name<A>(acc,mask,a,b),result[index]); \
    if constexpr(Mode==2) store_bits(native::maskz_##Name<A>(mask,acc,a,b),result[index]); \
    ++index; }
#define EXT(Name,CT,AT,BT) { \
    auto acc=load_bits<CT,A>(in.accumulator); \
    auto a=load_bits<AT,A>(in.a); auto b=load_bits<BT,A>(in.b); \
    store_bits(native::Name<A>(acc,a,b),result[index++]); }
    if constexpr(Kind==0) {
      CORE(dpbusd,std::uint8_t,std::int8_t)
      CORE(dpbusds,std::uint8_t,std::int8_t)
      CORE(dpwssd,std::int16_t,std::int16_t)
      CORE(dpwssds,std::int16_t,std::int16_t)
    } else if constexpr(Kind==1) {
      EXT(dpbssd,std::int32_t,std::int8_t,std::int8_t)
      EXT(dpbssds,std::int32_t,std::int8_t,std::int8_t)
      EXT(dpbsud,std::int32_t,std::int8_t,std::uint8_t)
      EXT(dpbsuds,std::int32_t,std::int8_t,std::uint8_t)
      EXT(dpbuud,std::uint32_t,std::uint8_t,std::uint8_t)
      EXT(dpbuuds,std::uint32_t,std::uint8_t,std::uint8_t)
    } else {
      EXT(dpwsud,std::int32_t,std::int16_t,std::uint16_t)
      EXT(dpwsuds,std::int32_t,std::int16_t,std::uint16_t)
      EXT(dpwusd,std::int32_t,std::uint16_t,std::int16_t)
      EXT(dpwusds,std::int32_t,std::uint16_t,std::int16_t)
      EXT(dpwuud,std::uint32_t,std::uint16_t,std::uint16_t)
      EXT(dpwuuds,std::uint32_t,std::uint16_t,std::uint16_t)
    }
#undef EXT
#undef CORE
    return result;
  }
  template<unsigned Kind> constexpr auto operations=[] {
    if constexpr(Kind==0) return core_operations;
    else if constexpr(Kind==1) return int8_operations;
    else return int16_operations;
  }();
  template<std::size_t N> constexpr auto make_case(property_rng& random) {
    inputs<N> result{};
    for(auto& x:result.accumulator) x=random.bits<std::uint32_t>();
    for(auto& x:result.a) x=random.bits<std::uint8_t>();
    for(auto& x:result.b) x=random.bits<std::uint8_t>();
    return result;
  }
  template<std::size_t N> constexpr auto cases=property_cases<16>(property_seed,make_case<N>);
  template<native::isa<native::x86> A,unsigned Kind,unsigned Mode,std::size_t N>
  consteval bool check_constants() {
    for(unsigned index=0;index<32;++index) {
      auto in=cases<N>[index%16];
      unsigned bits=1u<<index;
      if(index<16) {
        // All signedness boundaries, including two -32768 products whose sum
        // exceeds int32_t before the accumulator can cancel it.
        constexpr std::array<std::uint16_t,8> boundary{0,1,127,128,32767,32768,65534,65535};
        constexpr std::array<std::uint32_t,4> accumulators{0,0x7fffffff,0x80000000,0xffffffff};
        for(std::size_t lane=0;lane<N/4;++lane) in.accumulator[lane]=accumulators[(index+lane)%4];
        for(std::size_t lane=0;lane<N;++lane) {
          in.a[lane]=std::uint8_t(boundary[index%8] >> (8*(lane%2)));
          in.b[lane]=std::uint8_t(boundary[(index/2+lane/4)%8] >> (8*(lane%2)));
        }
      }
      auto wanted=oracle(in,operations<Kind>);
      if constexpr(Mode!=0) for(auto& op:wanted) for(std::size_t lane=0;lane<N/4;++lane)
        if(!((bits>>lane)&1)) op[lane]=Mode==1 ? in.accumulator[lane] : 0;
      if(calculate<A,Kind,Mode>(in,bits)!=wanted) return false;
    }
    return true;
  }
#define CHECK_CORE(A,N) \
  static_assert(check_constants<A,0,0,N>()); \
  static_assert(check_constants<A,0,1,N>()); \
  static_assert(check_constants<A,0,2,N>())
  CHECK_CORE(storage<16>,16); CHECK_CORE(storage<32>,32); CHECK_CORE(storage<64>,64);
  CHECK_CORE(evexvl,16); CHECK_CORE(evexvl,32); CHECK_CORE(evex512,64);
#undef CHECK_CORE
  static_assert(check_constants<core,0,0,16>());
  static_assert(check_constants<core,0,0,32>());
#define CHECK_EXT(A,Kind,N) static_assert(check_constants<A,Kind,0,N>())
  CHECK_EXT(storage<16>,1,16); CHECK_EXT(storage<32>,1,32);
  CHECK_EXT(storage<16>,2,16); CHECK_EXT(storage<32>,2,32);
  CHECK_EXT(int8,1,16); CHECK_EXT(int8,1,32);
  CHECK_EXT(int16,2,16); CHECK_EXT(int16,2,32);
#undef CHECK_EXT

  template<unsigned Kind,unsigned Accumulator> consteval bool saturation_boundaries() {
    constexpr std::array<std::uint16_t,8> values{0,1,127,128,32767,32768,65534,65535};
    constexpr std::array<std::uint32_t,4> accumulators{0,0x7fffffff,0x80000000,0xffffffff};
    inputs<16> input{};
    input.accumulator.fill(accumulators[Accumulator]);
    for(auto a:values) for(auto b:values) {
      for(unsigned lane=0;lane<16;++lane) {
        input.a[lane]=std::uint8_t(a>>(8*(lane%2)));
        input.b[lane]=std::uint8_t(b>>(8*(lane%2)));
      }
      if(calculate<storage<16>,Kind,0>(input,~0u)!=oracle(input,operations<Kind>)) return false;
    }
    return true;
  }
#define BOUNDARIES(Kind) \
  static_assert(saturation_boundaries<Kind,0>()); \
  static_assert(saturation_boundaries<Kind,1>()); \
  static_assert(saturation_boundaries<Kind,2>()); \
  static_assert(saturation_boundaries<Kind,3>())
  BOUNDARIES(0); BOUNDARIES(1); BOUNDARIES(2);
#undef BOUNDARIES

  template<unsigned Kind,std::size_t N>
  bool compare(evaluator<N,Kind==0 ? 4 : 6> evaluate,bool masked) {
    constexpr auto expected=[] consteval {
      std::array<outputs<N,Kind==0 ? 4 : 6>,16> result{};
      for(unsigned i=0;i<16;++i) result[i]=calculate<storage<N>,Kind,0>(cases<N>[i],~0u);
      return result;
    }();
    auto one=[&](inputs<N> const& in,unsigned bits,auto const& wanted,std::uint64_t seed,std::size_t index) {
      outputs<N,Kind==0 ? 4 : 6> actual{},merged{},zeroed{};
      evaluate(&in,&actual,bits,&merged,&zeroed);
      if(!property_equal("VNNI plain",seed,index,wanted,actual,in.accumulator,in.a,in.b)) return false;
      if(masked) {
        auto wm=wanted,wz=wanted;
        for(std::size_t op=0;op<wm.size();++op) for(std::size_t lane=0;lane<N/4;++lane) if(!((bits>>lane)&1)) {
          wm[op][lane]=in.accumulator[lane]; wz[op][lane]=0;
        }
        if(!property_equal("VNNI merge",seed,index,wm,merged,bits,in.accumulator,in.a,in.b) ||
           !property_equal("VNNI zero",seed,index,wz,zeroed,bits,in.accumulator,in.a,in.b)) return false;
      }
      return true;
    };
    for(unsigned i=0;i<16;++i) if(!one(cases<N>[i],1u<<i,expected[i],property_seed,i)) return false;
    auto settings=property_config(); property_rng random{settings.seed};
    for(std::size_t i=0;i<settings.cases;++i) {
      auto in=make_case<N>(random);
      if(!one(in,unsigned(random.next()),oracle(in,operations<Kind>),settings.seed,i)) return false;
    }
    return true;
  }
}
int main() {
  using namespace vnni_constant_test;
  auto cpu=native::observe_x86_capabilities(); unsigned executed=0;
  auto run=[&](char const* name,auto arch,auto body) {
    auto admission=native::classify_isa(cpu,arch);
    if(admission.invalid_features) return false;
    if(!admission.admitted()) { std::printf("SKIP %s: %s\n",name,admission.reason()); return true; }
    ++executed; return body();
  };
  if(!run("AVX-VNNI",core,[]{return compare<0>(invoke_core128,false) && compare<0>(invoke_core256,false);}) ||
     !run("AVX512-VNNI",evex512,[]{return compare<0>(invoke_evex512,true);}) ||
     !run("AVX512VL-VNNI",evexvl,[]{return compare<0>(invoke_evex128,true) && compare<0>(invoke_evex256,true);}) ||
     !run("VNNI-INT8",int8,[]{return compare<1>(invoke_int8_128,false) && compare<1>(invoke_int8_256,false);}) ||
     !run("VNNI-INT16",int16,[]{return compare<2>(invoke_int16_128,false) && compare<2>(invoke_int16_256,false);})) return 1;
  return executed ? 0 : 77;
}
