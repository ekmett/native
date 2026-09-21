// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.pclmul;
import native.x86.vpclmul;
#include "../x86_pclmul/checks.h"

namespace pclmul_constant_test {
  using namespace pclmul_fixture;
  using namespace native_test;
  template<std::size_t N> constexpr auto storage = native::target_features<native::x86>(
    N == 2 ? "sse2" : N == 4 ? "avx" : "avx512f");

  template<native::isa<native::x86> A, bool Legacy, std::size_t N, unsigned... I>
  consteval auto calculate(inputs<N> input, std::integer_sequence<unsigned,I...>) {
    using V = native::simd<word,N,A>;
    outputs<N,sizeof...(I)> result{};
    auto a = V::load(input.a.data()), b = V::load(input.b.data());
    unsigned index = 0;
    ([&] {
      if constexpr (Legacy) native::pclmulqdq<A,I>(a,b).store(result[index++].data());
      else native::vpclmulqdq<A,I>(a,b).store(result[index++].data());
    }(), ...);
    return result;
  }

  template<std::size_t N> constexpr auto make_case(property_rng& random) {
    inputs<N> input{};
    for (auto& x : input.a) x = random.next();
    for (auto& x : input.b) x = random.next();
    return input;
  }
  template<std::size_t N> constexpr auto cases = property_cases<16>(property_seed,make_case<N>);

  template<native::isa<native::x86> A, bool Legacy, std::size_t N>
  consteval bool check_constants() {
    for (auto input : cases<N>) {
      auto actual = calculate<A,Legacy>(input,selectors{});
      for (unsigned selector = 0; selector < 4; ++selector)
        for (std::size_t lane = 0; lane < N; lane += 2) {
          auto expected = polynomial(input.a[lane+(selector&1)],input.b[lane+(selector>>1)]);
          if (actual[selector][lane] != expected[0] || actual[selector][lane+1] != expected[1]) return false;
        }
    }
    // Every ignored immediate bit must leave the selected product unchanged.
    auto every = calculate<A,Legacy>(cases<N>[0],all_immediates{});
    auto four = calculate<A,Legacy>(cases<N>[0],selectors{});
    for (unsigned i = 0; i < 256; ++i)
      if (every[i] != four[(i&1)|((i>>3)&2)]) return false;
    return true;
  }
  static_assert(check_constants<legacy,true,2>());
  static_assert(check_constants<storage<2>,true,2>());
  static_assert(check_constants<vex128,false,2>());
  static_assert(check_constants<storage<2>,false,2>());
  static_assert(check_constants<vex256,false,4>());
  static_assert(check_constants<storage<4>,false,4>());
  static_assert(check_constants<evex512,false,8>());
  static_assert(check_constants<storage<8>,false,8>());

  template<unsigned Bit> consteval bool basis() {
    for (unsigned other = 0; other < 64; ++other) {
      inputs<2> input{{word{1}<<Bit,0},{word{1}<<other,0}};
      auto actual = calculate<storage<2>,true>(input,std::integer_sequence<unsigned,0>{})[0];
      words<2> expected{};
      expected[(Bit+other)/64] = word{1} << ((Bit+other)%64);
      if (actual != expected) return false;
    }
    return true;
  }
  template<unsigned I> constexpr bool basis_checked=basis<I>();
  template<unsigned... I> consteval bool every_basis(std::integer_sequence<unsigned,I...>) {
    return (basis_checked<I> && ...);
  }
  static_assert(every_basis(std::make_integer_sequence<unsigned,64>{}));

  template<std::size_t N, class Evaluate>
  bool compare(Evaluate evaluate) {
    constexpr auto expected = [] consteval {
      std::array<outputs<N,4>,cases<N>.size()> result{};
      for (std::size_t i=0;i<result.size();++i)
        result[i]=calculate<storage<N>,false>(cases<N>[i],selectors{});
      return result;
    }();
    for (std::size_t i=0;i<cases<N>.size();++i) {
      outputs<N,4> actual{};
      evaluate(cases<N>[i],actual,selectors{});
      if (!property_equal("PCLMUL constexpr/native",property_seed,i,expected[i],actual,cases<N>[i].a,cases<N>[i].b)) return false;
    }
    auto settings=property_config(); property_rng random{settings.seed};
    for (std::size_t i=0;i<settings.cases;++i) {
      auto input=make_case<N>(random); outputs<N,4> actual{};
      evaluate(input,actual,selectors{});
      outputs<N,4> wanted{};
      for(unsigned selector=0;selector<4;++selector) for(std::size_t lane=0;lane<N;lane+=2) {
        auto product=polynomial(input.a[lane+(selector&1)],input.b[lane+(selector>>1)]);
        wanted[selector][lane]=product[0]; wanted[selector][lane+1]=product[1];
      }
      if (!property_equal("PCLMUL polynomial",settings.seed,i,wanted,actual,input.a,input.b)) return false;
    }
    return true;
  }
}
int main() {
  using namespace pclmul_constant_test;
  auto cpu=native::observe_x86_capabilities(); unsigned executed=0;
  auto run=[&](char const* name,auto arch,auto body) {
    auto admission=native::classify_isa(cpu,arch);
    if(admission.invalid_features) return false;
    if(!admission.admitted()) { std::printf("SKIP %s: %s\n",name,admission.reason()); return true; }
    ++executed; return body();
  };
  if(!run("PCLMUL",legacy,[]{return compare<2>(evaluate_legacy<0,1,16,17>);}) ||
     !run("VPCLMUL128",vex128,[]{return compare<2>(evaluate_vex128<0,1,16,17>);}) ||
     !run("VPCLMUL256",vex256,[]{return compare<4>(evaluate_vex256<0,1,16,17>);}) ||
     !run("VPCLMUL512",evex512,[]{return compare<8>(evaluate_evex512<0,1,16,17>);})) return 1;
  return executed ? 0 : 77;
}
