// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeat under the test case's literal target scope; no production policy header.
namespace CASE_NAMESPACE {
  template<class X,class Y> concept addable = requires(X const & x,Y const & y) {x+y;};
  template<class V> bool same(V a,V b) {
    std::array<float,V::lanes> x{},y{};
    a.storeu(x.data()); b.storeu(y.data());
    for(std::size_t i=0;i<V::lanes;++i)
      if(std::bit_cast<std::uint32_t>(x[i])!=std::bit_cast<std::uint32_t>(y[i])) return false;
    return true;
  }
  // A separate numerical domain over a native register. No implicit float/raw
  // conversion: generic forwarding must retain this type and its array hook.
  // Derived custom domains retain their full architecture requirements.
  template<class V> struct opaque : V {};
  template<class V> struct value {
    static constexpr simd::isa architecture=V::architecture;
    V raw;
    value() noexcept :raw(0.f) {}
    explicit value(V v) noexcept :raw(v) {}
    friend value operator+(value a,value b) noexcept { return value(a.raw+b.raw); }
    friend value abs(value a) noexcept { return value(abs(a.raw)); }
    friend typename V::mask isfinite(value a) noexcept {
      return (a.raw==a.raw) & (abs(a.raw)!=V(std::bit_cast<float>(0x7f800000u)));
    }
  };
  inline unsigned batch_calls=0;
  template<class V,std::size_t N> auto add(std::array<value<V>,N> a,std::array<value<V>,N> const & b) noexcept {
    ++batch_calls;
    for(std::size_t i=0;i<N;++i) a[i]=a[i]+b[i];
    return a;
  }
  template<class V,std::size_t N> auto abs(std::array<value<V>,N> a) noexcept {
    ++batch_calls;
    for(auto & x:a) x=abs(x);
    return a;
  }
  template<std::size_t L,std::size_t N> bool run() {
    constexpr auto A = SIMD_TARGET_ISA(CASE_TARGET);
    using V=simd::vec<float,L,A>;
    using W=simd::wide<V,N>;
    static_assert(!addable<W,simd::wide<V,N+1>>);
    using OtherWidth=simd::vec<float,L==1?2:1,A>;
    static_assert(!addable<W,simd::wide<OtherWidth,N>>);
#if defined(__x86_64__) || defined(_M_X64)
    constexpr auto OtherArch=A==simd::avx2?simd::avx512:simd::avx2;
    if constexpr (L<=8) static_assert(!addable<W,simd::wide<simd::vec<float,L,OtherArch>,N>>);
#endif
#if !SIMD_TEST_IMPORT
    static_assert(simd::detail::wide_features<std::pair<V &,int>>::value==SIMD_TARGET_ISA(CASE_SCOPE));
    static_assert(simd::detail::wide_features<opaque<V>>::value==A);
    static_assert(simd::detail::wide_features<value<V>>::value==A);
    static_assert(simd::detail::wide_equivalent_default<V> == (L==1));
    static_assert(!simd::detail::wide_equivalent_default<opaque<V>>);
    static_assert(!simd::detail::wide_equivalent_default<value<V>>);
#endif
    // Use wide's attributed empty construction: MSVC array<T,0> itself can own
    // a dummy T and an unattributed implicit constructor.
    auto initialize=[]<std::size_t... I>(std::index_sequence<I...>) {
      if constexpr (N==0) return W{};
      else return W{std::array<V,N>{((void)I,V(0.f))...}};
    };
    auto a=initialize(std::make_index_sequence<N>{});
    auto b=initialize(std::make_index_sequence<N>{});
    auto c=initialize(std::make_index_sequence<N>{});
    auto & aa=a.registers;auto & bb=b.registers;auto & cc=c.registers;
    for(std::size_t k=0;k<N;++k) {
      std::array<float,L> a{},b{},c{};
      for(std::size_t i=0;i<L;++i) {
        a[i]=(i%2?-1.f:1.f)*(float(i+k)+.25f);
        b[i]=float(i%3)+1.f; c[i]=-.5f;
      }
      aa[k]=V::loadu(a.data());bb[k]=V::loadu(b.data());cc[k]=V::loadu(c.data());
    }

    auto check=[&](W const & result,auto operation) {
      for(std::size_t k=0;k<N;++k) if(!same(result.registers[k],operation(aa[k],bb[k],cc[k]))) return false;
      return true;
    };
    if(!check(a+b,[](V x,V y,V){return x+y;}) ||
       !check(a-b,[](V x,V y,V){return x-y;}) ||
       !check(a*b,[](V x,V y,V){return x*y;}) ||
       !check(a/b,[](V x,V y,V){return x/y;}) ||
       !check(abs(a),[](V x,V,V){return abs(x);}) ||
       !check(sqrt(abs(a)),[](V x,V,V){return sqrt(abs(x));}) ||
       !check(fma(a,b,c),[](V x,V y,V z){return fma(x,y,z);}) ||
       !check(floor(a),[](V x,V,V){return floor(x);}) ||
       !check(ceil(a),[](V x,V,V){return ceil(x);}) ||
       !check(trunc(a),[](V x,V,V){return trunc(x);})) return false;
    auto masks=(a<b);
    auto classified=(a==a);
    auto combined=(masks & classified) | (masks ^ masks);
    auto chosen=select(combined,a,b);
    static_assert(std::same_as<decltype(chosen),W>);
    static_assert(decltype(masks.registers)::value_type::architecture==A);
    for(std::size_t k=0;k<N;++k) {
      auto m=aa[k]<bb[k];
      if(!same(chosen.registers[k],select(m,aa[k],bb[k]))) return false;
      if(!same(select((~masks).registers[k],aa[k],bb[k]),select(~m,aa[k],bb[k]))) return false;
    }
    auto copied=simd::broadcast<V,N>(V(2.f));
    if(copied.apply([](auto const &... x){return sizeof...(x);})!=N) return false;
    for(auto x:copied.registers) if(!same(x,V(2.f))) return false;
    simd::wide<W,2> nested{a,b};
    auto doubled=nested+nested;
    for(std::size_t k=0;k<N;++k) {
      if(!same(doubled.registers[0].registers[k],aa[k]+aa[k])) return false;
      if(!same(doubled.registers[1].registers[k],bb[k]+bb[k])) return false;
    }
    // A custom domain supplies its own behavior and conservatively keeps A.
    // Exercise it in every raw scope; extra half-tag cases above prove built-in
    // narrowing without claiming requirements of an arbitrary custom wrapper.
    if constexpr (A==SIMD_TARGET_ISA(CASE_SCOPE)) {
      using C=value<V>; using CW=simd::wide<C,N>;
      CW custom{a}; // explicit element conversion, retaining full architecture
      static_assert(std::same_as<typename decltype(custom.registers)::value_type,C>);
      batch_calls=0;
      auto plus=custom+custom;
      auto positive=abs(custom);
      if(batch_calls!=2) return false;
      auto finite=isfinite(custom);
      static_assert(std::same_as<decltype(finite),simd::wide<typename V::mask,N>>);
      for(std::size_t k=0;k<N;++k)
        if(!same(plus.registers[k].raw,aa[k]+aa[k]) ||
           !same(positive.registers[k].raw,abs(aa[k])) ||
           !same(select(finite.registers[k],V(1.f),V(0.f)),V(1.f))) return false;
    }
    return true;
  }
  bool check() {
    return run<1,0>() && run<1,1>() && run<2,3>() &&
      run<3,1>() && run<4,3>() && run<CASE_WIDTH,0>() &&
      run<CASE_WIDTH,1>() && run<CASE_WIDTH,3>();
  }
}
