#include "support/profile.h"
#include <array>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
using namespace test_simd;
#if defined(__AVX512F__)
static volatile std::uint64_t mask_runtime_seed=0xaaaaaaaaaaaaaaaaull;
#endif
static void require(bool b,char const * why) { if(!b){std::fprintf(stderr,"%s\n",why);std::exit(1);} }
template<class T,std::size_t N> constexpr std::size_t deduce(test_vec<T,N> const &) { return N; }
template<class X> concept implicit_truth=std::is_convertible_v<X,bool>;
template<class X> concept plusable=requires(X a){a+a;};
template<class To,class From> concept can_mask_cast=requires(From x){mask_cast<To>(x);};
template<class T> constexpr bool scalar_constants() {
  using M=test_vec<T,1>;using U=typename T::storage_type;
  auto m=M::from_native(U(1));auto f=M(false);auto t=M(T(true));
  m &= t;m |= f;m ^= f;
  return all(m) && none(!m) && m.to_native()==U(~U(0)) &&
    M::from_bitset(3).to_bitset()==1 && select(m,t,f).to_bitset()==1;
}
static_assert(scalar_constants<mask8>() && scalar_constants<mask16>() &&
  scalar_constants<mask32>() && scalar_constants<mask64>());
static_assert([] {
  using B=test_vec<bool,1>;
  auto t=B::from_native(0xff);auto f=B{};t&=B(true);t|=f;t^=f;
  return t.to_native()==1 && none(f) && all(t) && all(t==!f) &&
    select(t==B(true),t,f).to_native()==1;
}());

template<class T,std::size_t N> void full() {
  using M=test_vec<T,N>;using U=typename T::storage_type;
  static_assert(sizeof(M)==sizeof(typename M::native_type));
  static_assert(alignof(M)==alignof(typename M::native_type));
  static_assert(alignof(std::array<M,1>)==alignof(M));
  static_assert(alignof(std::array<M,3>)==alignof(M));
  struct container { unsigned char prefix; std::array<M,3> values; };
  static_assert(offsetof(container,values)%alignof(M)==0);
  container stored{7,{M::from_bitset(1),M::from_bitset(2),M::from_bitset(3)}};
  auto copied=stored.values;
  std::array<std::array<M,3>,2> nested{stored.values,copied};
  static_assert(alignof(decltype(nested))==alignof(M));
  for(auto const & row:nested) for(std::size_t i=0;i<row.size();++i) {
    require(reinterpret_cast<std::uintptr_t>(&row[i])%alignof(M)==0,"mask container alignment");
    require(row[i].to_bitset()==((i+1)&test_backend::mask_low_bits<N>),"mask array copy value");
  }
  static_assert(sizeof(M)==sizeof(T)*N);
  static_assert(std::is_trivially_copyable_v<M>);
  static_assert(!implicit_truth<M> && !plusable<M>);
  constexpr auto low=test_backend::mask_low_bits<N>;
  for(auto bits: {std::uint64_t(0),std::uint64_t(1),std::uint64_t(0x5555555555555555),low,~std::uint64_t(0)}) {
    auto m=M::from_bitset(bits);auto b=M::from_bitset(0xaaaaaaaaaaaaaaaaull);
    require(m.to_bitset()==(bits&low),"full bits");
    require((~m).to_bitset()==((~bits)&low),"full complement");
    require((m&b).to_bitset()==((bits&0xaaaaaaaaaaaaaaaaull)&low),"full and");
    require((m|b).to_bitset()==((bits|0xaaaaaaaaaaaaaaaaull)&low),"full or");
    require((m^b).to_bitset()==((bits^0xaaaaaaaaaaaaaaaaull)&low),"full xor");
    require(all(m)==((bits&low)==low) && any(m)==((bits&low)!=0) && none(m)==((bits&low)==0),"full reductions");
    require(all(m==m) && none(m!=m),"full equality");
    require(select(m,M(true),M(false)).to_bitset()==m.to_bitset(),"full select");
    std::array<T,N> values{};m.store(values.data());auto again=simd::load_simd<test_vec<T,N>>(values.data());
    require(again.to_bitset()==m.to_bitset() && deduce(again)==N,"mask typed memory/deduction");
    for(std::size_t i=0;i<N;++i) require(values[i].to_bits()==(((bits>>i)&1)?U(~U(0)):U(0)),"mask stored canonical");
#if defined(__AVX512F__)
    if constexpr(test_backend::predicate_shape<N>) require(to_vector_mask<T>(to_predicate(m)).to_bitset()==m.to_bitset(),"predicate/full roundtrip");
#endif
  }
  std::array<U,N> raw{};
  for(std::size_t i=0;i<N;++i) raw[i]=(i%3==0)?U(0):(i%3==1)?U(1):U(U(1)<<(sizeof(U)*8-1));
  typename M::native_type native;std::memcpy(&native,raw.data(),sizeof(native));
  auto normalized=M::from_native(native);auto actual=normalized.to_native();std::memcpy(raw.data(),&actual,sizeof(actual));
  for(std::size_t i=0;i<N;++i) require(raw[i]==(i%3==0?U(0):U(~U(0))),"safe full native import");
}
// MSVC19.44 ICEs while combining the reduction-heavy checks into one optimized
// function. Keep every operation and /O2; each bounded witness compiles alone.
#if defined(_MSC_VER)
#define simd_mask_test_noinline __declspec(noinline)
#else
#define simd_mask_test_noinline __attribute__((noinline))
#endif
template<std::size_t N,int Case> simd_mask_test_noinline bool compact_case(std::uint64_t input) {
#if defined(__AVX512F__)
  using P=simd::predicate<N,test_arch>;using U=typename P::native_type;constexpr auto low=test_backend::mask_low_bits<N>;
  auto a=P::from_bitset(input),b=P::from_bitset(input>>1);
  if constexpr(Case==0) return P::from_native(U(~U(0))).to_bitset()==low;
  else if constexpr(Case==1) return P::unsafe_from_native(U(~U(0))).to_bitset()==low;
  else if constexpr(Case==2) return (~a).to_bitset()==((~input)&low);
  else if constexpr(Case==3) return (a&b).to_bitset()==((input&(input>>1))&low);
  else if constexpr(Case==4) return (a|b).to_bitset()==((input|(input>>1))&low);
  else if constexpr(Case==5) return (a^b).to_bitset()==((input^(input>>1))&low);
  else if constexpr(Case==6) return all(a)==((input&low)==low);
  else if constexpr(Case==7) return any(a)==((input&low)!=0);
  else if constexpr(Case==8) return none(a)==((input&low)==0);
  else if constexpr(Case==9) return (a==b).to_bitset()==((~(input^(input>>1)))&low);
  else return select(b,a,P(false)).to_bitset()==((input&(input>>1))&low);
#else
  (void)input;return true;
#endif
}
#undef simd_mask_test_noinline
template<std::size_t N> void compact() {
#if defined(__AVX512F__)
  using P=simd::predicate<N,test_arch>;
  constexpr auto constant=P::from_bitset(~std::uint64_t(0));
  static_assert(all(constant)&&none(~constant));
  static_assert(std::is_trivially_copyable_v<P> && !implicit_truth<P> && !plusable<P>);
  for(auto input:{std::uint64_t(0),std::uint64_t(1),std::uint64_t(mask_runtime_seed),~std::uint64_t(0)}) {
    require(compact_case<N,0>(input),"compact clip upper");require(compact_case<N,1>(input),"compact unsafe clip upper");
    require(compact_case<N,2>(input),"compact not");require(compact_case<N,3>(input),"compact and");
    require(compact_case<N,4>(input),"compact or");require(compact_case<N,5>(input),"compact xor");
    require(compact_case<N,6>(input),"compact all");require(compact_case<N,7>(input),"compact any");
    require(compact_case<N,8>(input),"compact none");require(compact_case<N,9>(input),"compact equality");
    require(compact_case<N,10>(input),"compact select");
  }
#endif
}
template<std::size_t N> void booleans() {
  using B=test_vec<bool,N>;
  static_assert(!implicit_truth<B> && !plusable<B> && std::is_trivially_copyable_v<B>);
  static_assert(sizeof(B)==N);
  std::array<bool,N> values{};for(std::size_t i=0;i<N;++i) values[i]=(i&1)!=0;
  auto a=simd::load_simd<test_vec<bool,N>>(values.data());auto n=a.to_native();std::array<std::uint8_t,N> bytes{};std::memcpy(bytes.data(),&n,sizeof(n));
  for(std::size_t i=0;i<N;++i) require(bytes[i]==(values[i]?1:0),"Boolean bytes 0/1");
  require(all(a==a)&&none(a!=a)&&all(a|!a)&&none(a&!a),"Boolean logic");
  auto selected=select(a==B(true),B(true),B(false));std::array<bool,N> result{};selected.store(result.data());require(result==values,"Boolean select");
  to_bool(to_vector_mask(a)).store(result.data());require(result==values,"Boolean/full explicit roundtrip");
#if defined(__AVX512F__)
  if constexpr(test_backend::predicate_shape<N>) {to_bool(to_predicate(a)).store(result.data());require(result==values,"Boolean/predicate roundtrip");}
#endif
  require(none(B::load_partial(nullptr,0)),"zero Boolean load");a.store_partial(nullptr,0);
  for(std::size_t count=0;count<=N;++count) {
    auto partial=simd::load_simd_partial<test_vec<bool,N>>(values.data(),count,true);std::array<bool,N+2> out{};
    simd::store_simd_partial(out.data()+1,partial,count);require(!out[0]&&!out[count+1],"Boolean tail guards");
    for(std::size_t i=0;i<count;++i) require(out[i+1]==values[i],"Boolean partial prefix");
  }
  bytes.fill(255);std::memcpy(&n,bytes.data(),sizeof(n));auto safe=B::from_native(n).to_native();std::memcpy(bytes.data(),&safe,sizeof(safe));
  for(auto byte:bytes) require(byte==1,"safe Boolean normalization");
}
template<std::size_t N> void floating() {
  using F=test_vec<float,N>;
  auto a=F(1),b=F(2);auto mask=(a<b)&~(a==b);
  auto selected=select(mask,a,b);std::array<float,N> out{};selected.store(out.data());
  for(float x:out) require(x==1,"floating native mask select");
  auto chosen_nan=F::from_bits(0xffc12345u);
  (-chosen_nan).store(out.data());
  for(float x:out) require(std::bit_cast<std::uint32_t>(x)==0x7fc12345u,"Raw negation flips chosen NaN sign only");
  auto full=test_vec<mask32,N>::from_bitset(0xaaaaaaaaaaaaaaaaull);
  select(full,F::from_bits(0x80000000u),F::from_bits(0xffc12345u)).store(out.data());
  for(std::size_t i=0;i<N;++i) require(std::bit_cast<std::uint32_t>(out[i])==((i&1)?0x80000000u:0xffc12345u),"full mask selects raw whole words");
}
int main() {
  static_assert(sizeof(mask8)==1&&sizeof(mask16)==2&&sizeof(mask32)==4&&sizeof(mask64)==8);
  static_assert(!std::is_convertible_v<mask32,std::uint32_t> && !std::is_convertible_v<mask32,bool>);
  static_assert(!plusable<mask32>);
  static_assert(std::same_as<decltype(simd::vec(test_arch{},true)),test_vec<bool,1>>);
  static_assert(!can_mask_cast<mask8,test_vec<mask64,2>>);
  static_assert(std::same_as<decltype(simd::vec(test_arch{},std::array<bool,1>{})),test_vec<bool,1>>);
  full<mask8,1>();full<mask16,1>();full<mask32,1>();full<mask64,1>();booleans<1>();floating<1>();
#if defined(__AVX2__) || defined(__ARM_NEON)
  full<mask8,16>();full<mask16,8>();full<mask32,4>();full<mask64,2>();booleans<16>();floating<4>();
  auto m=simd::vec(test_arch{},mask32(true),mask32(false),mask32(true),mask32(false));static_assert(std::same_as<decltype(m),test_vec<mask32,4>>);
#endif
#if defined(__AVX2__)
  require(mask_cast<mask64>(test_vec<mask32,4>::from_bitset(5)).to_bitset()==5,"explicit cross-width mask expansion");
  require(mask_cast<mask32>(test_vec<mask64,4>::from_bitset(10)).to_bitset()==10,"explicit cross-width mask contraction");
  full<mask8,32>();full<mask16,16>();full<mask32,8>();full<mask64,4>();booleans<32>();floating<8>();
#endif
#if defined(__AVX512F__)
  full<mask32,16>();full<mask64,8>();floating<16>();compact<1>();compact<2>();compact<4>();compact<8>();compact<16>();
#if defined(__AVX512BW__)
  full<mask8,64>();full<mask16,32>();booleans<64>();compact<32>();compact<64>();
#endif
#endif
  std::puts("native masks, full mask domains, Boolean bytes, raw float selection and tails passed");
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks predicate representations, canonical mask lanes and Boolean storage.
 */
