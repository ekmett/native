// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated under the matching backend target; intrinsic wrappers stay in GMF.
namespace native {
#define NATIVE_POLYFILL_QUALIFIED_UNARY(NAME) \
  /** Make the permitted lane operation available through qualified namespace lookup. */ \
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> && \
    detail::polyfill_element_traits<T>::arithmetic \
  constexpr simd<T,N,A> NAME(simd<T,N,A> value) noexcept { return NAME(value); }
  NATIVE_POLYFILL_QUALIFIED_UNARY(abs)
  NATIVE_POLYFILL_QUALIFIED_UNARY(sqrt)
  NATIVE_POLYFILL_QUALIFIED_UNARY(floor)
  NATIVE_POLYFILL_QUALIFIED_UNARY(ceil)
  NATIVE_POLYFILL_QUALIFIED_UNARY(trunc)
  NATIVE_POLYFILL_QUALIFIED_UNARY(round_even)
#undef NATIVE_POLYFILL_QUALIFIED_UNARY
  /// Make explicitly permitted fused arithmetic available through qualified lookup.
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> &&
    detail::polyfill_element_traits<T>::arithmetic
  constexpr simd<T,N,A> fma(simd<T,N,A> a,simd<T,N,A> b,simd<T,N,A> c) noexcept { return fma(a,b,c); }

  /// Expand a logical compact mask into canonical full-vector lanes.
  template<simd_mask_element T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill))
  constexpr simd<T,N,A> to_vector_mask(detail::polyfill_predicate<N,A> mask) noexcept {
    return simd<T,N,A>::from_bitset(mask.to_bitset());
  }
  /// Preserve a logical compact predicate already in canonical form.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill))
  constexpr auto to_predicate(detail::polyfill_predicate<N,A> mask) noexcept { return mask; }
  /// Compress truth into the stable logical predicate for an emulated domain.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && (simd_mask_element<T> || std::same_as<T,bool>) &&
      !::NATIVE_BACKEND_NAMESPACE::predicate_shape<N>)
  constexpr auto to_predicate(simd<T,N,A> value) noexcept {
    return detail::polyfill_predicate<N,A>::from_bitset(value.to_bitset());
  }
  /// Expand compact truth to zero-or-one Boolean data lanes.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill))
  constexpr simd<bool,N,A> to_bool(detail::polyfill_predicate<N,A> mask) noexcept {
    std::array<bool,N> lanes{}; auto bits=mask.to_bitset();
    for(std::size_t i=0;i<N;++i) lanes[i]=((bits>>i)&1)!=0;
    return simd<bool,N,A>::load(lanes.data());
  }
  /// Expand a public compact predicate when the native backend has no converter.
  template<simd_mask_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && !::NATIVE_BACKEND_NAMESPACE::predicate_shape<N>)
  constexpr simd<T,N,A> to_vector_mask(predicate<N,A> mask) noexcept { return simd<T,N,A>::from_bitset(mask.to_bitset()); }
  /// Expand public compact truth to Boolean data lanes.
  template<std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && !::NATIVE_BACKEND_NAMESPACE::predicate_shape<N>)
  constexpr simd<bool,N,A> to_bool(predicate<N,A> mask) noexcept {
    return to_bool(detail::polyfill_predicate<N,A>::from_bitset(mask.to_bitset()));
  }
  /// Expand each logical predicate bit into an unsigned zero/all-one lane.
  template<simd_integer_element T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill))
  constexpr simd<std::make_unsigned_t<T>,N,A> mask_bits(detail::polyfill_predicate<N,A> mask) noexcept {
    using U=std::make_unsigned_t<T>;
    std::array<U,N> lanes{}; auto bits=mask.to_bitset();
    for(std::size_t i=0;i<N;++i) lanes[i]=((bits>>i)&1)?~U{}:U{};
    return simd<U,N,A>::load(lanes.data());
  }
  /// Expand a public predicate when native conversion is absent.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && !::NATIVE_BACKEND_NAMESPACE::predicate_shape<N>)
  constexpr simd<std::make_unsigned_t<T>,N,A> mask_bits(predicate<N,A> mask) noexcept {
    return mask_bits<T>(detail::polyfill_predicate<N,A>::from_bitset(mask.to_bitset()));
  }

#if NATIVE_HOST_WASM
  /// Expand a matching compact predicate before native full-register selection.
  template<class T,std::size_t N,isa<> A,class M> requires NATIVE_ARCH_REQUIRES(A) &&
    (A.has(polyfill) && !detail::polyfill_operation_shape<T,N,A>) &&
    (std::same_as<M,predicate<N,A>> || std::same_as<M,detail::polyfill_predicate<N,A>>) &&
    (!std::same_as<M,typename simd<T,N,A>::mask_type>) && detail::ordinary_simd_element<T> &&
    requires { typename simd<T,N,A>::word_type; }
  constexpr simd<T,N,A> select(M mask,simd<T,N,A> a,simd<T,N,A> b) noexcept {
    using U=typename simd<T,N,A>::word_type;
    return select(simd<mask_lane<U>,N,A>::from_bitset(mask.to_bitset()),a,b);
  }

#endif
#define NATIVE_POLYFILL_MASKED(NAME,OP) \
  /** Apply lane arithmetic where the matching mask is true, retaining prior elsewhere. */ \
  template<class T,std::size_t N,isa<> A,class M> \
    requires NATIVE_ARCH_REQUIRES(A) && (detail::polyfill_helper_shape<T,N,A> || \
      (A.has(polyfill) && !detail::ordinary_simd_element<T> && detail::polyfill_element_traits<T>::arithmetic)) && \
      (std::same_as<M,typename simd<T,N,A>::mask_type> || std::same_as<M,typename simd<T,N,A>::vector_mask_type> || \
       std::same_as<M,predicate<N,A>>) && requires(simd<T,N,A> x) { x OP x; } \
  constexpr simd<T,N,A> NAME(M mask,simd<T,N,A> prior,simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return select(mask,a OP b,prior); \
  } \
  /** Apply lane arithmetic where the mask is true, writing positive zero elsewhere. */ \
  template<class T,std::size_t N,isa<> A,class M> \
    requires NATIVE_ARCH_REQUIRES(A) && requires(M m,simd<T,N,A> x) { NAME(m,x,x,x); } \
  constexpr simd<T,N,A> NAME##_zero(M mask,simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return NAME(mask,simd<T,N,A>(T{}),a,b); \
  }
  NATIVE_POLYFILL_MASKED(masked_add,+)
  NATIVE_POLYFILL_MASKED(masked_sub,-)
  NATIVE_POLYFILL_MASKED(masked_mul,*)
#undef NATIVE_POLYFILL_MASKED

#define NATIVE_POLYFILL_NATIVE_HALF_UNARY(NAME) \
  /** Supply a permitted scalar operation absent from native half's storage API. */ \
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && \
    detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 && !detail::polyfill_operation_shape<T,N,A>) \
  constexpr simd<T,N,A> NAME(simd<T,N,A> value) noexcept { \
    using S=typename detail::polyfill_chunk<T,1,detail::hardware_isa<A>>::type; \
    std::array<T,N> lanes{}; value.store(lanes.data()); \
    for(std::size_t i=0;i<N;++i) NAME(S::load(lanes.data()+i)).store(lanes.data()+i); \
    return simd<T,N,A>::load(lanes.data()); \
  }
  NATIVE_POLYFILL_NATIVE_HALF_UNARY(abs)
  NATIVE_POLYFILL_NATIVE_HALF_UNARY(floor)
  NATIVE_POLYFILL_NATIVE_HALF_UNARY(ceil)
  NATIVE_POLYFILL_NATIVE_HALF_UNARY(trunc)
  NATIVE_POLYFILL_NATIVE_HALF_UNARY(round_even)
#undef NATIVE_POLYFILL_NATIVE_HALF_UNARY
  /// Bridge a matching compact mask to native half's existing vector-mask selection.
  template<class T,std::size_t N,isa<> A,class M> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) &&
    detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 && !detail::polyfill_operation_shape<T,N,A>) &&
    (std::same_as<M,predicate<N,A>> || std::same_as<M,detail::polyfill_predicate<N,A>>)
  constexpr simd<T,N,A> select(M mask,simd<T,N,A> a,simd<T,N,A> b) noexcept {
    return select(simd<T,N,A>::mask_type::from_bitset(mask.to_bitset()),a,b);
  }
#define NATIVE_POLYFILL_NATIVE_HALF_SCALAR(OP) \
  /** Broadcast a matching half scalar before the native lane operation. */ \
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && \
    detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 && !detail::polyfill_operation_shape<T,N,A>) \
  constexpr auto operator OP(simd<T,N,A> a,T b) noexcept { return a OP simd<T,N,A>(b); } \
  /** Broadcast a matching half scalar before the native lane operation. */ \
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && \
    detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 && !detail::polyfill_operation_shape<T,N,A>) \
  constexpr auto operator OP(T a,simd<T,N,A> b) noexcept { return simd<T,N,A>(a) OP b; }
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(+)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(-)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(*)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(/)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(==)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(!=)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(<)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(<=)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(>)
  NATIVE_POLYFILL_NATIVE_HALF_SCALAR(>=)
#undef NATIVE_POLYFILL_NATIVE_HALF_SCALAR
#define NATIVE_POLYFILL_NATIVE_HALF_ASSIGN(OP) \
  /** Apply permitted native half arithmetic in place. */ \
  template<class T,std::size_t N,isa<> A,class U> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && \
    detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 && !detail::polyfill_operation_shape<T,N,A>) && \
    (std::same_as<U,T> || std::same_as<U,simd<T,N,A>>) \
  constexpr simd<T,N,A> & operator OP##=(simd<T,N,A> & a,U b) noexcept { return a=a OP b; }
  NATIVE_POLYFILL_NATIVE_HALF_ASSIGN(+)
  NATIVE_POLYFILL_NATIVE_HALF_ASSIGN(-)
  NATIVE_POLYFILL_NATIVE_HALF_ASSIGN(*)
  NATIVE_POLYFILL_NATIVE_HALF_ASSIGN(/)
#undef NATIVE_POLYFILL_NATIVE_HALF_ASSIGN


  /// Broadcast one compile-time-selected logical lane.
  template<std::size_t I,class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> && (I<N)
  constexpr simd<T,N,A> broadcast(simd<T,N,A> value,imm_t<I>) noexcept { return simd<T,N,A>(value.template get<I>()); }
#define NATIVE_POLYFILL_ARRAY_ROUND(NAME) \
  /** Round each explicitly permitted register; empty arrays do no work. */ \
  template<class T,std::size_t N,std::size_t M,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill)) && \
    (!(std::same_as<T,float> && ::NATIVE_BACKEND_NAMESPACE::float_shape<N> && !detail::polyfill_operation_shape<T,N,A>)) && \
    requires(simd<T,N,A> value) { NAME(value); } \
  constexpr std::array<simd<T,N,A>,M> NAME(std::array<simd<T,N,A>,M> const & input) noexcept { \
    std::array<simd<T,N,A>,M> output{}; \
    for(std::size_t i=0;i<M;++i) output[i]=NAME(input[i]); \
    return output; \
  }
  NATIVE_POLYFILL_ARRAY_ROUND(floor)
  NATIVE_POLYFILL_ARRAY_ROUND(ceil)
  NATIVE_POLYFILL_ARRAY_ROUND(trunc)
#undef NATIVE_POLYFILL_ARRAY_ROUND

  /// Sum binary32 logical lanes in increasing order, rounding after each addition.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_helper_shape<float,N,A>
  constexpr float reduce_add(simd<float,N,A> value) noexcept {
    std::array<float,N> lanes{}; value.store(lanes.data()); float result=0;
    for(auto lane:lanes) {
      if consteval { result=std::bit_cast<float>(detail::constexpr_float::add_bits<detail::constexpr_float::binary32>(
        std::bit_cast<std::uint32_t>(result),std::bit_cast<std::uint32_t>(lane))); }
      else { result+=lane; }
    }
    return result;
  }

  /// Scale active binary32 lanes by 2^floor(exponent), retaining inactive representations.
  /// Floating exception flags are unspecified, matching the native scaling API.
  template<std::size_t N,isa<> A,class M>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) &&
      (!::NATIVE_BACKEND_NAMESPACE::native_scaleb_shape<N> || detail::polyfill_operation_shape<float,N,A>)) &&
      (std::same_as<M,typename simd<float,N,A>::mask_type> || std::same_as<M,typename simd<float,N,A>::vector_mask_type> ||
       std::same_as<M,predicate<N,A>>)
  constexpr simd<float,N,A> masked_scaleb(M mask,simd<float,N,A> prior,simd<float,N,A> value,simd<float,N,A> exponent) noexcept {
    using V=simd<float,N,A>;
    if constexpr(requires { typename V::chunk_type; } && requires(typename V::chunk_type x) { scaleb(x,x); }) {
      auto result=value.to_native(),powers=exponent.to_native();
      for(std::size_t i=0;i<V::register_count;++i)
        result[i]=scaleb(V::chunk_type::from_native(result[i]),V::chunk_type::from_native(powers[i])).to_native();
      return select(mask,V::from_native(result),prior);
    } else {
      std::array<std::uint32_t,N> values{},powers{},output{};
      value.store_bits(values.data()); exponent.store_bits(powers.data()); prior.store_bits(output.data());
      auto bits=mask.to_bitset(); detail::polyfill_half_control control{};
      if !consteval { control=detail::polyfill_float_environment<detail::constexpr_float::binary32>(); }
      for(std::size_t i=0;i<N;++i) if((bits>>i)&1)
        output[i]=detail::float_constant::scale(values[i],powers[i],control.mode,control.policy);
      return V::load_bits(output.data());
    }
  }
  /// Scale active lanes and write positive zero to every inactive lane.
  template<std::size_t N,isa<> A,class M>
    requires NATIVE_ARCH_REQUIRES(A) && requires(M m,simd<float,N,A> x) { masked_scaleb(m,x,x,x); } &&
      (A.has(polyfill) && (!::NATIVE_BACKEND_NAMESPACE::native_scaleb_shape<N> || detail::polyfill_operation_shape<float,N,A>))
  constexpr simd<float,N,A> masked_scaleb_zero(M mask,simd<float,N,A> value,simd<float,N,A> exponent) noexcept {
    return masked_scaleb(mask,simd<float,N,A>(0.f),value,exponent);
  }
  /// Scale every logical lane, preferring each available native scaling register.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) &&
      (!::NATIVE_BACKEND_NAMESPACE::native_scaleb_shape<N> || detail::polyfill_operation_shape<float,N,A>))
  constexpr simd<float,N,A> scaleb(simd<float,N,A> value,simd<float,N,A> exponent) noexcept {
    return masked_scaleb(typename simd<float,N,A>::mask_type(true),simd<float,N,A>(0.f),value,exponent);
  }

  /// Select individual representation bits without canonicalizing the bit mask.
  template<class T,simd_integer_element U,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (sizeof(T)==sizeof(U) &&
      (detail::polyfill_operation_shape<T,N,A> ||
       (A.has(polyfill) && !detail::ordinary_simd_element<T> && detail::polyfill_element_traits<T>::supported)))
  constexpr simd<T,N,A> bit_select(simd<U,N,A> mask,simd<T,N,A> a,simd<T,N,A> b) noexcept {
    using W=std::make_unsigned_t<U>;
    if constexpr(detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::binary16 ||
      detail::polyfill_element_traits<T>::kind==detail::polyfill_element_kind::bfloat16) {
      std::array<W,N> first{},second{}; std::array<U,N> masks{};
      a.store_bits(first.data()); b.store_bits(second.data()); mask.store(masks.data());
      for(std::size_t i=0;i<N;++i) {
        auto m=std::bit_cast<W>(masks[i]); first[i]=W((m&first[i])|(~m&second[i]));
      }
      return simd<T,N,A>::load_bits(first.data());
    }
    std::array<T,N> first{},second{}; std::array<U,N> masks{};
    a.store(first.data()); b.store(second.data()); mask.store(masks.data());
    for(std::size_t i=0;i<N;++i) {
      auto m=std::bit_cast<W>(masks[i]);
      first[i]=std::bit_cast<T>(W((m&std::bit_cast<W>(first[i]))|(~m&std::bit_cast<W>(second[i]))));
    }
    return simd<T,N,A>::load(first.data());
  }

  /// Numerically convert logical lanes; integer destinations require representable truncated results.
  /// Half conversions use the portable scalar format's nearest-even conversion.
  template<class To,class From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && detail::polyfill_element_traits<To>::supported &&
      detail::polyfill_element_traits<From>::supported && !std::same_as<To,bool> && !std::same_as<From,bool> &&
      !simd_mask_element<To> && !simd_mask_element<From> &&
      (detail::polyfill_helper_shape<From,N,A> || detail::polyfill_helper_shape<To,N,A> ||
       !detail::ordinary_simd_element<To> || !detail::ordinary_simd_element<From>) &&
      requires { sizeof(simd<To,N,A>); }
#if NATIVE_HOST_WASM
      && !(sizeof(From)*N==16 && std::floating_point<To> && !std::same_as<To,From> &&
        (sizeof(From)==4 || std::floating_point<From>))
#endif
      )
  constexpr simd<To,N,A> convert(simd<From,N,A> value) noexcept {
    using R=simd<To,N,A>; using V=simd<From,N,A>;
    if constexpr(detail::polyfill_chunk_compatible<R,V> &&
      requires(typename V::chunk_type x) { { convert<To>(x) } -> std::same_as<typename R::chunk_type>; }) {
      return detail::polyfill_transform_chunks<R>(value,[](auto chunk) { return convert<To>(chunk); });
    } else {
    std::array<From,N> input{}; std::array<To,N> output{}; value.store(input.data());
    for(std::size_t i=0;i<N;++i) {
      if constexpr(!simd_integer_element<From> && !simd_integer_element<To>) {
        using F=typename detail::polyfill_element_traits<From>::format;
        using G=typename detail::polyfill_element_traits<To>::format;
        if constexpr(sizeof(To)==2 || sizeof(From)==2) {
          output[i]=std::bit_cast<To>(detail::constexpr_float::convert_bits<G,F>(std::bit_cast<typename F::bits_type>(input[i])));
        } else {
          if consteval {
            output[i]=std::bit_cast<To>(detail::constexpr_float::convert_bits<G,F>(std::bit_cast<typename F::bits_type>(input[i])));
          } else { output[i]=static_cast<To>(input[i]); }
        }
      } else if constexpr(simd_integer_element<From> && sizeof(To)==2 && !simd_integer_element<To>) {
        using G=typename detail::polyfill_element_traits<To>::format;
        using U=std::make_unsigned_t<From>;
        bool sign=false; U magnitude=static_cast<U>(input[i]);
        if constexpr(std::is_signed_v<From>) if(input[i]<0) { sign=true; magnitude=U{}-magnitude; }
        output[i]=std::bit_cast<To>(detail::constexpr_float::round_pack<G>(sign,
          detail::constexpr_float::magnitude<1>{{static_cast<std::uint64_t>(magnitude)}},0,
          detail::constexpr_float::rounding::nearest_even,{}));
      } else if constexpr(!simd_integer_element<From> && sizeof(From)==2 && simd_integer_element<To>) {
        output[i]=static_cast<To>(float(input[i]));
      } else { output[i]=static_cast<To>(input[i]); }
    }
    return simd<To,N,A>::load(output.data());
    }
  }
#if NATIVE_HOST_NEON
  /// Truncate binary32 lanes with ARM's defined signed saturation and NaN-to-zero result.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<float,N,A>
  constexpr simd<std::int32_t,N,A> fcvtzs(simd<float,N,A> value) noexcept {
    using R=simd<std::int32_t,N,A>; using V=simd<float,N,A>;
    if constexpr(detail::polyfill_chunk_compatible<R,V>) {
      return detail::polyfill_transform_chunks<R>(value,[](auto chunk) { return fcvtzs(chunk); });
    } else {
    std::array<float,N> input{}; std::array<std::int32_t,N> output{}; value.store(input.data());
    for(std::size_t i=0;i<N;++i) output[i]=detail::float_constant::fcvtzs(std::bit_cast<std::uint32_t>(input[i]));
    return simd<std::int32_t,N,A>::load(output.data());
    }
  }
  /// Truncate binary32 lanes with ARM's unsigned saturation and NaN/negative-to-zero result.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<float,N,A>
  constexpr simd<std::uint32_t,N,A> fcvtzu(simd<float,N,A> value) noexcept {
    using R=simd<std::uint32_t,N,A>; using V=simd<float,N,A>;
    if constexpr(detail::polyfill_chunk_compatible<R,V>) {
      return detail::polyfill_transform_chunks<R>(value,[](auto chunk) { return fcvtzu(chunk); });
    } else {
    std::array<float,N> input{}; std::array<std::uint32_t,N> output{}; value.store(input.data());
    for(std::size_t i=0;i<N;++i) output[i]=detail::float_constant::fcvtzu(std::bit_cast<std::uint32_t>(input[i]));
    return simd<std::uint32_t,N,A>::load(output.data());
    }
  }
#endif

  /// Pack selected logical lanes in increasing order, filling the suffix.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_helper_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr compaction_result<simd<T,N,A>> compress(typename simd<T,N,A>::mask mask,simd<T,N,A> value,T fill={}) noexcept {
    std::array<T,N> input{},output{}; value.store(input.data()); output.fill(fill);
    auto bits=mask.to_bitset(); std::size_t count=0;
    for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[count++]=input[i];
    return {simd<T,N,A>::load(output.data()),count};
  }
  /// Expand a globally packed prefix into selected logical lanes, retaining prior elsewhere.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_helper_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr simd<T,N,A> expand(typename simd<T,N,A>::mask mask,simd<T,N,A> packed,simd<T,N,A> prior) noexcept {
    std::array<T,N> input{},output{}; packed.store(input.data()); prior.store(output.data());
    auto bits=mask.to_bitset(); std::size_t count=0;
    for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[i]=input[count++];
    return simd<T,N,A>::load(output.data());
  }
  /// Write only the selected prefix that fits capacity; zero writes permit null.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_helper_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr std::size_t compress_store(T * p,std::size_t capacity,typename simd<T,N,A>::mask mask,simd<T,N,A> value) noexcept {
    auto packed=compress(mask,value);
    auto count=std::min(capacity,packed.count); packed.value.store_partial(p,count); return count;
  }
}
