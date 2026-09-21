// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated under each source target scope; no vector crosses a target boundary.
namespace integer_test::INTEGER_CASE_NAME {
  constexpr auto arch = NATIVE_TARGET_ISA(INTEGER_CASE_NAME);
  template<class T, std::size_t N> bool integer_shape() {
    using V = native::simd<T,N,arch>;
    static_assert(std::same_as<decltype(native::popcount(std::declval<V>())),V>);
    static_assert(noexcept(native::popcount(std::declval<V>())));
    std::array<T,N> input{}, actual{};
    for(unsigned round=0;round<259;++round) {
      for(std::size_t lane=0;lane<N;++lane)
        input[lane]=round==0?T{}:round==1?std::numeric_limits<T>::max():
          round<2+8*sizeof(T)?T(T{1}<<(round-2)):
          T(std::uint64_t(round)*0x9e3779b97f4a7c15ull+lane*0xd1b54a32d192ed03ull);
      auto value=V::loadu(input.data());
      native::popcount(value).storeu(actual.data());
      for(std::size_t lane=0;lane<N;++lane)
        if(actual[lane]!=T(std::popcount(input[lane]))) return false;
      auto signed_value=native::reinterpret_bits<std::make_signed_t<T>>(value);
      native::reinterpret_bits<T>(signed_value).storeu(actual.data());
      if(actual!=input) return false;
      if constexpr(sizeof(T)==4 && (N==2 || N==3)) {
        auto poisoned=value;
        for(std::size_t lane=N;lane<4;++lane) poisoned.value[lane]=std::numeric_limits<T>::max();
        auto population=native::popcount(poisoned);
        population.storeu(actual.data());
        for(std::size_t lane=0;lane<N;++lane)
          if(actual[lane]!=T(std::popcount(input[lane]))) return false;
        for(std::size_t lane=N;lane<4;++lane)
          if(population.to_native()[lane]!=0) return false;
        std::uint64_t sum=0;
        for(auto lane:input) sum+=lane;
        if(native::reduce_add_widened(poisoned)!=sum) return false;
        if constexpr(N==2)
          if(native::pairwise_add_widened(poisoned).to_native()!=sum) return false;
      }
      if constexpr(N>1 && N%2==0 && sizeof(T)<=4) {
        auto pairs=native::pairwise_add_widened(value);
        using U=typename decltype(pairs)::value_type;
        std::array<U,N/2> widened{};
        pairs.storeu(widened.data());
        for(std::size_t lane=0;lane<N/2;++lane)
          if(widened[lane]!=std::uint64_t(input[2*lane])+input[2*lane+1]) return false;
      }
      if constexpr(sizeof(T)<=4) {
        // Empty prefixes and every tail exercise the zero/reduction boundary.
        for(std::size_t count=0;count<=N;++count) {
          auto prefix=V::load_partial(input.data(),count);
          std::uint64_t expected=0;
          for(std::size_t lane=0;lane<count;++lane) expected+=input[lane];
          if(native::reduce_add_widened(prefix)!=expected) return false;
        }
      }
    }
    return true;
  }
  template<class To,class From,std::size_t N> bool packing_shape() {
    using V=native::simd<From,N,arch>;
    using R=native::simd<To,2*N,arch>;
    static_assert(std::same_as<decltype(native::narrow_concat<To>(std::declval<V>(),std::declval<V>())),R>);
    static_assert(noexcept(native::narrow_concat<To>(std::declval<V>(),std::declval<V>())));
    std::array<From,N> a{},b{};
    std::array<To,2*N> actual{};
    for(unsigned round=0;round<257;++round) {
      for(std::size_t lane=0;lane<N;++lane) {
        a[lane]=round==0?std::numeric_limits<From>::max():
          From(std::uint64_t(round)*0x9e3779b97f4a7c15ull+lane*0xd1b54a32d192ed03ull);
        b[lane]=From(~a[lane]+lane);
      }
      native::narrow_concat<To>(V::loadu(a.data()),V::loadu(b.data())).storeu(actual.data());
      for(std::size_t lane=0;lane<N;++lane)
        if(actual[lane]!=To(a[lane]) || actual[N+lane]!=To(b[lane])) return false;
    }
    return true;
  }
  template<std::size_t Bytes> bool register_shape() {
    bool good=true;
    if constexpr(integer_test::has_shape<std::uint8_t,Bytes,arch>)
      good=good && integer_shape<std::uint8_t,Bytes>();
    if constexpr(integer_test::has_shape<std::uint16_t,Bytes/2,arch>)
      good=good && integer_shape<std::uint16_t,Bytes/2>();
    if constexpr(integer_test::has_shape<std::uint32_t,Bytes/4,arch>)
      good=good && integer_shape<std::uint32_t,Bytes/4>();
    if constexpr(integer_test::has_shape<std::uint64_t,Bytes/8,arch>)
      good=good && integer_shape<std::uint64_t,Bytes/8>();
    if constexpr(integer_test::can_pack<std::uint8_t,std::uint16_t,Bytes/2,arch>)
      good=good && packing_shape<std::uint8_t,std::uint16_t,Bytes/2>();
    if constexpr(integer_test::can_pack<std::uint16_t,std::uint32_t,Bytes/4,arch>)
      good=good && packing_shape<std::uint16_t,std::uint32_t,Bytes/4>();
    if constexpr(integer_test::can_pack<std::uint32_t,std::uint64_t,Bytes/8,arch>)
      good=good && packing_shape<std::uint32_t,std::uint64_t,Bytes/8>();
    return good;
  }
  __attribute__((noinline)) bool run() {
    if(!integer_shape<std::uint8_t,1>() || !integer_shape<std::uint16_t,1>() ||
       !integer_shape<std::uint32_t,1>() || !integer_shape<std::uint64_t,1>()) return false;
    if constexpr(integer_test::has_shape<std::uint32_t,2,arch>)
      if(!integer_shape<std::uint32_t,2>() || !integer_shape<std::uint32_t,3>()) return false;
    if(!register_shape<16>() || !register_shape<32>() || !register_shape<64>()) return false;
    return true;
  }
}
