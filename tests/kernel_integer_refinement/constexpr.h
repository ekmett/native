// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included inside each target-specific fixture namespace.
template<class T,std::size_t N> struct integer_input {
  std::array<T,N> a,b,prior;
  std::uint64_t mask;
};
template<class T,std::size_t N> struct integer_output {
  std::array<T,N> population,selected,bits,add,sub,mul,add_zero,sub_zero,mul_zero,roundtrip;
  std::array<std::uint64_t,N> pairs;
  std::uint64_t sum;
  bool signed_agree=true;
  constexpr bool operator==(integer_output const &) const = default;
};
template<class T,std::size_t N> constexpr auto integer_cases=native_test::property_cases<24>(native_test::property_seed,
  [](native_test::property_rng &rng) {
    integer_input<T,N> in{};
    for(std::size_t i=0;i<N;++i) { in.a[i]=rng.bits<T>(); in.b[i]=rng.bits<T>(); in.prior[i]=rng.bits<T>(); }
    in.mask=rng.next(); return in;
  });
template<class T,std::size_t N> constexpr integer_output<T,N> integer_expected(integer_input<T,N> const &in) {
  integer_output<T,N> out{};
  for(std::size_t i=0;i<N;++i) {
    bool active=((in.mask>>i)&1)!=0;
    for(unsigned bit=0;bit<sizeof(T)*8;++bit) out.population[i]+=T((in.a[i]>>bit)&1);
    out.selected[i]=active?in.a[i]:in.b[i];
    out.bits[i]=T((in.prior[i]&in.a[i])|(~in.prior[i]&in.b[i]));
    out.add[i]=active?T(std::uint64_t(in.a[i])+in.b[i]):in.prior[i];
    out.sub[i]=active?T(std::uint64_t(in.a[i])-in.b[i]):in.prior[i];
    out.mul[i]=active?T(std::uint64_t(in.a[i])*in.b[i]):in.prior[i];
    using V=native::simd<T,N,arch>;
    if constexpr(requires(V v,typename V::mask_type m) { masked_add_zero(m,v,v); }) {
      out.add_zero[i]=active?out.add[i]:T(0);
      out.sub_zero[i]=active?out.sub[i]:T(0);
      out.mul_zero[i]=active?out.mul[i]:T(0);
    }
    out.roundtrip[i]=in.a[i];
    if constexpr(sizeof(T)<=4) out.sum+=in.a[i];
  }
  if constexpr(N>1 && N%2==0 && sizeof(T)<=4)
    for(std::size_t i=0;i<N/2;++i) out.pairs[i]=std::uint64_t(in.a[2*i])+in.a[2*i+1];
  return out;
}
template<class T,std::size_t N> constexpr integer_output<T,N> integer_evaluate(integer_input<T,N> const &in) {
  using V=native::simd<T,N,arch>;
  using M=typename V::mask_type;
  V a(in.a),b(in.b),prior(in.prior);
  auto m=M::from_bitset(in.mask);
  integer_output<T,N> out{};
  native::popcount(a).store(out.population.data());
  // Short-vector arithmetic supplies its own select and masked operations.
  select(m,a,b).store(out.selected.data());
  bit_select(prior,a,b).store(out.bits.data());
  masked_add(m,prior,a,b).store(out.add.data());
  masked_sub(m,prior,a,b).store(out.sub.data());
  masked_mul(m,prior,a,b).store(out.mul.data());
  if constexpr(requires { masked_add_zero(m,a,b); }) {
    masked_add_zero(m,a,b).store(out.add_zero.data());
    masked_sub_zero(m,a,b).store(out.sub_zero.data());
    masked_mul_zero(m,a,b).store(out.mul_zero.data());
  }
  native::reinterpret_bits<T>(native::reinterpret_bits<std::make_signed_t<T>>(a)).store(out.roundtrip.data());
  auto sa=native::reinterpret_bits<std::make_signed_t<T>>(a);
  auto sb=native::reinterpret_bits<std::make_signed_t<T>>(b);
  auto sp=native::reinterpret_bits<std::make_signed_t<T>>(prior);
  std::array<T,N> signed_words{};
  native::reinterpret_bits<T>(masked_add(m,sp,sa,sb)).store(signed_words.data());
  out.signed_agree=out.signed_agree && signed_words==out.add;
  native::reinterpret_bits<T>(masked_sub(m,sp,sa,sb)).store(signed_words.data());
  out.signed_agree=out.signed_agree && signed_words==out.sub;
  native::reinterpret_bits<T>(masked_mul(m,sp,sa,sb)).store(signed_words.data());
  out.signed_agree=out.signed_agree && signed_words==out.mul;
  if constexpr(sizeof(T)<=4) out.sum=native::reduce_add_widened(a);
  if constexpr(N>1 && N%2==0 && sizeof(T)<=4) {
    auto pairs=native::pairwise_add_widened(a);
    std::array<typename decltype(pairs)::value_type,N/2> values{};
    pairs.store(values.data());
    for(std::size_t i=0;i<N/2;++i) out.pairs[i]=values[i];
  }
  return out;
}
template<class T,std::size_t N> bool integer_properties() {
  static_assert([] {
    for(auto const &in:integer_cases<T,N>) if(integer_evaluate(in)!=integer_expected(in)) return false;
    return true;
  }());
  auto config=native_test::property_config(256);
  native_test::property_rng rng{config.seed};
  for(std::size_t index=0;index<config.cases;++index) {
    integer_input<T,N> in{};
    for(std::size_t i=0;i<N;++i) { in.a[i]=rng.bits<T>(); in.b[i]=rng.bits<T>(); in.prior[i]=rng.bits<T>(); }
    in.mask=rng.next();
    auto expected=integer_expected(in),actual=integer_evaluate(in);
    if(!native_test::property_check("integer utilities",config.seed,index,expected==actual,in.a,in.b,in.prior,in.mask)) return false;
  }
  return true;
}
template<class To,class From,std::size_t N> constexpr auto packing_cases=native_test::property_cases<24>(native_test::property_seed,
  [](native_test::property_rng &rng) {
    std::array<std::array<From,N>,2> in{};
    for(auto &v:in) for(auto &x:v) x=rng.bits<From>();
    return in;
  });
template<class To,class From,std::size_t N> constexpr auto packing_evaluate(std::array<std::array<From,N>,2> const &in) {
  using V=native::simd<From,N,arch>;
  std::array<To,2*N> out{};
  native::narrow_concat<To>(V(in[0]),V(in[1])).store(out.data());
  return out;
}
template<class To,class From,std::size_t N> bool packing_properties() {
  static_assert([] {
    for(auto const &in:packing_cases<To,From,N>) {
      auto out=packing_evaluate<To>(in);
      for(std::size_t i=0;i<N;++i) if(out[i]!=To(in[0][i]) || out[N+i]!=To(in[1][i])) return false;
    }
    return true;
  }());
  auto config=native_test::property_config(256);
  native_test::property_rng rng{config.seed};
  for(std::size_t index=0;index<config.cases;++index) {
    std::array<std::array<From,N>,2> in{};
    std::array<To,2*N> expected{};
    for(std::size_t i=0;i<N;++i) { in[0][i]=rng.bits<From>(); in[1][i]=rng.bits<From>(); expected[i]=To(in[0][i]); expected[N+i]=To(in[1][i]); }
    if(!native_test::property_equal("narrow_concat",config.seed,index,expected,packing_evaluate<To>(in),in)) return false;
  }
  return true;
}

template<class T,std::size_t N> constexpr bool memory_check(std::uint64_t bits) {
  using V=native::simd<T,N,arch>;
  alignas(64) std::array<T,N> source{};
  for(std::size_t i=0;i<N;++i) source[i]=T(((bits>>i)&1)!=0);
  auto value=native::load_simd<V>(source);
  std::array<T,N+2> out{};
  out.fill(T(true));
  native::store_simd(out.data()+1,value);
  if(out.front()!=T(true) || out.back()!=T(true)) return false;
  for(std::size_t i=0;i<N;++i) if(out[i+1]!=source[i]) return false;
  for(std::size_t count=0;count<=N;++count) {
    auto prefix=native::load_simd_partial<V>(count?source.data():nullptr,count,T(true));
    std::array<T,N> actual{};
    native::store_simd(actual.data(),prefix);
    for(std::size_t i=0;i<N;++i) if(actual[i]!=(i<count?source[i]:T(true))) return false;
    out.fill(T(true));
    native::store_simd_partial(count?out.data()+1:nullptr,value,count);
    for(std::size_t i=0;i<N;++i) if(out[i+1]!=(i<count?source[i]:T(true))) return false;
    if(out.front()!=T(true) || out.back()!=T(true)) return false;
    if constexpr(requires(T const *p,T *q) { V::load_partial(p,count,T{}).store_partial(q,count); }) {
      auto member=V::load_partial(count?source.data():nullptr,count,T(true));
      native::store_simd(actual.data(),member);
      for(std::size_t i=0;i<N;++i) if(actual[i]!=(i<count?source[i]:T(true))) return false;
      out.fill(T(true)); member.store_partial(count?out.data()+1:nullptr,count);
      for(std::size_t i=0;i<count;++i) if(out[i+1]!=source[i]) return false;
      for(std::size_t i=count;i<N;++i) if(out[i+1]!=T(true)) return false;
    }
  }
  if constexpr(std::is_same_v<T,bool>) {
    std::array<bool,N> selected{};
    auto mask=native::to_vector_mask(value);
    auto roundtrip=native::to_bool(mask);
    select(mask,value,!value).store(selected.data());
    for(auto lane:selected) if(!lane) return false;
    if(any(roundtrip!=value) || !all(roundtrip==value)) return false;
    if constexpr(requires { native::to_predicate(value); })
      if(any(native::to_bool(native::to_predicate(value))!=value)) return false;
  } else if constexpr(native::simd_mask_element<T>) {
    constexpr auto low=[] { if constexpr(N==64) return ~std::uint64_t{}; else return (std::uint64_t{1}<<N)-1; }();
    if(value.to_bitset()!=(bits & low)) return false;
    if constexpr(requires { native::mask_cast<native::mask8>(value); }) {
      auto narrow=native::mask_cast<native::mask8>(value);
      if(native::mask_cast<T>(narrow).to_bitset()!=value.to_bitset()) return false;
    }
    if constexpr(requires { native::to_predicate(value); })
      if(native::to_vector_mask<T>(native::to_predicate(value)).to_bitset()!=value.to_bitset()) return false;
    auto expanded=native::mask_bits(value);
    std::array<typename decltype(expanded)::value_type,N> lanes{};
    expanded.store(lanes.data());
    for(std::size_t i=0;i<N;++i) if(lanes[i]!=source[i].to_bits()) return false;
  }
  return true;
}
template<class T,std::size_t N,unsigned Index> constexpr bool memory_constant=[] {
  constexpr auto samples=native_test::property_cases<8>(native_test::property_seed,
    [](native_test::property_rng &rng) { return rng.next(); });
  if constexpr(Index==8) return memory_check<T,N>(0);
  else if constexpr(Index==9) return memory_check<T,N>(~std::uint64_t{});
  else return memory_check<T,N>(samples[Index]);
}();
template<class T,std::size_t N> bool memory_properties() {
  static_assert([]<unsigned... Index>(std::integer_sequence<unsigned,Index...>) {
    return (memory_constant<T,N,Index> && ...);
  }(std::make_integer_sequence<unsigned,10>{}));
  auto config=native_test::property_config(32);
  native_test::property_rng rng{config.seed};
  for(std::size_t i=0;i<config.cases;++i) {
    auto bits=rng.next();
    if(!native_test::property_check("typed partial memory",config.seed,i,memory_check<T,N>(bits),bits,N,sizeof(T))) return false;
  }
  return true;
}
template<std::size_t N> bool mask_memory_shapes() {
  bool ok=true;
  if constexpr(requires { sizeof(native::simd<bool,N,arch>); }) ok=memory_properties<bool,N>() && ok;
  if constexpr(requires { sizeof(native::simd<native::mask8,N,arch>); }) ok=memory_properties<native::mask8,N>() && ok;
  if constexpr(requires { sizeof(native::simd<native::mask16,N,arch>); }) ok=memory_properties<native::mask16,N>() && ok;
  if constexpr(requires { sizeof(native::simd<native::mask32,N,arch>); }) ok=memory_properties<native::mask32,N>() && ok;
  if constexpr(requires { sizeof(native::simd<native::mask64,N,arch>); }) ok=memory_properties<native::mask64,N>() && ok;
  return ok;
}
