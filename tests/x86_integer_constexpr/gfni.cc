// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.gfni;
#include "../x86_gfni/checks.h"

namespace gfni_constant_test {
  using namespace gfni_fixture;
  using namespace native_test;
  template<std::size_t N> constexpr auto storage=native::target_features<native::x86>(
    N==16 ? "sse2" : N==32 ? "avx" : "avx512f");
  template<std::size_t N> constexpr auto make_case(property_rng& random) {
    inputs<N> result{};
    for(auto& x:result.a) x=random.bits<byte>();
    for(auto& x:result.b) x=random.bits<byte>();
    for(auto& x:result.source) x=random.bits<byte>();
    return result;
  }
  template<std::size_t N> constexpr auto cases=property_cases<4>(property_seed,make_case<N>);

  template<native::isa<native::x86> A, unsigned Mode, std::size_t N>
  consteval auto calculate(inputs<N> input,std::uint64_t bits) {
    using V=native::simd<byte,N,A>;
    using M=native::simd<std::uint64_t,N/8,A>;
    auto a=V::load(input.a.data()), b=V::load(input.b.data()), src=V::load(input.source.data());
    auto matrices=std::bit_cast<std::array<std::uint64_t,N/8>>(input.b);
    auto matrix=M::load(matrices.data());
    auto k=native::predicate<N,A>::from_bitset(bits);
    outputs<N> result{};
    if constexpr(Mode==0) native::gf2p8mulb<A>(a,b).store(result[0].data());
    if constexpr(Mode==1) native::gf2p8mulb_mask<A>(src,k,a,b).store(result[0].data());
    if constexpr(Mode==2) native::gf2p8mulb_maskz<A>(k,a,b).store(result[0].data());
    [&]<unsigned... I>(std::integer_sequence<unsigned,I...>) {
      unsigned index=1;
      ([&] {
        if constexpr(Mode==0) {
          native::gf2p8affineqb<A,I>(a,matrix).store(result[index].data());
          native::gf2p8affineinvqb<A,I>(a,matrix).store(result[index+3].data());
        }
        if constexpr(Mode==1) {
          native::gf2p8affineqb_mask<A,I>(src,k,a,matrix).store(result[index].data());
          native::gf2p8affineinvqb_mask<A,I>(src,k,a,matrix).store(result[index+3].data());
        }
        if constexpr(Mode==2) {
          native::gf2p8affineqb_maskz<A,I>(k,a,matrix).store(result[index].data());
          native::gf2p8affineinvqb_maskz<A,I>(k,a,matrix).store(result[index+3].data());
        }
        ++index;
      }(),...);
    }(std::integer_sequence<unsigned,0,0xa5,0xff>{});
    return result;
  }
  template<native::isa<native::x86> A,unsigned Mode,std::size_t N>
  consteval bool check_constants() {
    for(unsigned i=0;i<cases<N>.size();++i) {
      auto input=cases<N>[i];
      auto mask=std::array<std::uint64_t,4>{0,~std::uint64_t{0},0xaaaaaaaaaaaaaaaa,0x8000000000000001}[i];
      auto expected=oracle(input);
      if constexpr(Mode!=0) for(auto& operation:expected) for(std::size_t lane=0;lane<N;++lane)
        if(!((mask>>lane)&1)) operation[lane]=Mode==1 ? input.source[lane] : 0;
      if(calculate<A,Mode>(input,mask)!=expected) return false;
    }
    return true;
  }
#define CHECK(A,N) \
  static_assert(check_constants<A,0,N>()); \
  static_assert(check_constants<A,1,N>()); \
  static_assert(check_constants<A,2,N>())
  CHECK(storage<16>,16); CHECK(storage<32>,32); CHECK(storage<64>,64);
  CHECK(mask_narrow,16); CHECK(mask_narrow,32); CHECK(mask512,64);
#undef CHECK
  static_assert(check_constants<arch128,0,16>());
  static_assert(check_constants<arch256,0,32>());
  static_assert(check_constants<arch512,0,64>());

  // Every field product: the oracle uses unreduced polynomial multiplication
  // and long division, independently of the implementation's shift/reduce loop.
  template<unsigned High> consteval bool products() {
    using V=native::simd<byte,16,storage<16>>;
    for(unsigned low=0;low<256;low+=16) {
      std::array<byte,16> aa{},bb{},actual{};
      for(unsigned lane=0;lane<16;++lane) { aa[lane]=byte(High); bb[lane]=byte(low+lane); }
      native::gf2p8mulb<storage<16>>(V::load(aa.data()),V::load(bb.data())).store(actual.data());
      for(unsigned lane=0;lane<16;++lane) if(actual[lane]!=multiply(aa[lane],bb[lane])) return false;
    }
    return true;
  }
  template<unsigned I> constexpr bool product_checked=products<I>();
  static_assert([]<unsigned... I>(std::integer_sequence<unsigned,I...>) {
    return (product_checked<I> && ...);
  }(std::make_integer_sequence<unsigned,256>{}));

  template<unsigned Imm8> consteval bool affine_constant() {
    using V=native::simd<byte,16,storage<16>>;
    using M=native::simd<std::uint64_t,2,storage<16>>;
    std::array<byte,16> actual{};
    native::gf2p8affineqb<storage<16>,Imm8>(V(byte{0xff}),M(std::uint64_t{0})).store(actual.data());
    for(auto value:actual) if(value!=Imm8) return false;
    return true;
  }
  static_assert([]<unsigned... I>(std::integer_sequence<unsigned,I...>) {
    return (affine_constant<I>() && ...);
  }(std::make_integer_sequence<unsigned,256>{}));

  consteval bool matrices_and_inverse() {
    using V=native::simd<byte,16,storage<16>>;
    using M=native::simd<std::uint64_t,2,storage<16>>;
    std::array<byte,16> actual{},values{};
    for(unsigned value=0;value<256;value+=16) {
      for(unsigned lane=0;lane<16;++lane) values[lane]=byte(value+lane);
      native::gf2p8affineinvqb<storage<16>,0>(V::load(values.data()),M(std::uint64_t{0x0102040810204080})).store(actual.data());
      for(unsigned lane=0;lane<16;++lane)
        if(actual[lane]!=inverses[values[lane]]) return false;
    }
    for(unsigned row=0;row<8;++row) for(unsigned column=0;column<8;++column) {
      auto matrix=std::uint64_t{1} << ((7-row)*8+column);
      for(unsigned lane=0;lane<16;++lane) values[lane]=byte(1u<<(lane%8));
      native::gf2p8affineqb<storage<16>,0>(V::load(values.data()),M(matrix)).store(actual.data());
      for(unsigned lane=0;lane<16;++lane)
        if(actual[lane]!=(lane%8==column ? byte(1u<<row) : byte{0})) return false;
    }
    return true;
  }
  static_assert(matrices_and_inverse());

  template<std::size_t N> bool compare(evaluator<N> plain,mask_evaluator<N> masked) {
    constexpr auto expected=[] consteval {
      std::array<outputs<N>,cases<N>.size()> result{};
      for(unsigned i=0;i<result.size();++i) result[i]=calculate<storage<N>,0>(cases<N>[i],~std::uint64_t{0});
      return result;
    }();
    auto one=[&](inputs<N> const& input,std::uint64_t bits,outputs<N> const& wanted,std::uint64_t seed,std::size_t index) {
      outputs<N> actual{},merge{},zero{};
      if(plain) {
        plain(input,actual);
        if(!property_equal("GFNI plain",seed,index,wanted,actual,input.a,input.b)) return false;
      }
      if(masked) {
        masked(input,bits,merge,zero);
        auto wm=wanted,wz=wanted;
        for(std::size_t op=0;op<7;++op) for(std::size_t lane=0;lane<N;++lane) if(!((bits>>lane)&1)) {
          wm[op][lane]=input.source[lane]; wz[op][lane]=0;
        }
        if(!property_equal("GFNI merge",seed,index,wm,merge,bits,input.a,input.b,input.source) ||
           !property_equal("GFNI zero",seed,index,wz,zero,bits,input.a,input.b)) return false;
      }
      return true;
    };
    for(unsigned i=0;i<cases<N>.size();++i)
      if(!one(cases<N>[i],0x8000000000000001|i,expected[i],property_seed,i)) return false;
    auto settings=property_config(); property_rng random{settings.seed};
    for(std::size_t i=0;i<settings.cases;++i) {
      auto input=make_case<N>(random);
      if(!one(input,random.next(),oracle(input),settings.seed,i)) return false;
    }
    return true;
  }
}
int main() {
  using namespace gfni_constant_test;
  auto cpu=native::observe_x86_capabilities(); unsigned executed=0;
  auto run=[&](char const* name,auto arch,auto body) {
    auto admission=native::classify_isa(cpu,arch);
    if(admission.invalid_features) return false;
    if(!admission.admitted()) { std::printf("SKIP %s: %s\n",name,admission.reason()); return true; }
    ++executed; return body();
  };
  if(!run("GFNI128",arch128,[]{return compare<16>(evaluate128,nullptr);}) ||
     !run("GFNI256",arch256,[]{return compare<32>(evaluate256,nullptr);}) ||
     !run("GFNI512",arch512,[]{return compare<64>(evaluate512,nullptr);}) ||
     !run("GFNI mask128",mask_narrow,[]{return compare<16>(nullptr,evaluate_mask128);}) ||
     !run("GFNI mask256",mask_narrow,[]{return compare<32>(nullptr,evaluate_mask256);}) ||
     !run("GFNI mask512",mask512,[]{return compare<64>(nullptr,evaluate_mask512);})) return 1;
  return executed ? 0 : 77;
}
