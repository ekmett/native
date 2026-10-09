// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated under the matching backend target; intrinsic wrappers stay in GMF.
namespace native {
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

#define NATIVE_POLYFILL_MASKED(NAME,OP) \
  /** Apply lane arithmetic where the matching mask is true, retaining prior elsewhere. */ \
  template<class T,std::size_t N,isa<> A,class M> \
    requires NATIVE_ARCH_REQUIRES(A) && (detail::polyfill_operation_shape<T,N,A> || \
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

  /// Broadcast one compile-time-selected logical lane.
  template<std::size_t I,class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> && (I<N)
  constexpr simd<T,N,A> broadcast(simd<T,N,A> value,imm_t<I>) noexcept { return simd<T,N,A>(value.template get<I>()); }
  /// Sum binary32 logical lanes in increasing order, rounding after each addition.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<float,N,A>
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
      (detail::polyfill_operation_shape<From,N,A> || detail::polyfill_operation_shape<To,N,A> ||
       !detail::ordinary_simd_element<To> || !detail::ordinary_simd_element<From>) &&
      requires { sizeof(simd<To,N,A>); })
  constexpr simd<To,N,A> convert(simd<From,N,A> value) noexcept {
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
#if NATIVE_HOST_NEON
  /// Truncate binary32 lanes with ARM's defined signed saturation and NaN-to-zero result.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<float,N,A>
  constexpr simd<std::int32_t,N,A> fcvtzs(simd<float,N,A> value) noexcept {
    std::array<float,N> input{}; std::array<std::int32_t,N> output{}; value.store(input.data());
    for(std::size_t i=0;i<N;++i) output[i]=detail::float_constant::fcvtzs(std::bit_cast<std::uint32_t>(input[i]));
    return simd<std::int32_t,N,A>::load(output.data());
  }
  /// Truncate binary32 lanes with ARM's unsigned saturation and NaN/negative-to-zero result.
  template<std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<float,N,A>
  constexpr simd<std::uint32_t,N,A> fcvtzu(simd<float,N,A> value) noexcept {
    std::array<float,N> input{}; std::array<std::uint32_t,N> output{}; value.store(input.data());
    for(std::size_t i=0;i<N;++i) output[i]=detail::float_constant::fcvtzu(std::bit_cast<std::uint32_t>(input[i]));
    return simd<std::uint32_t,N,A>::load(output.data());
  }
#endif

  /// Pack selected logical lanes in increasing order, filling the suffix.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr compaction_result<simd<T,N,A>> compress(typename simd<T,N,A>::mask mask,simd<T,N,A> value,T fill={}) noexcept {
    std::array<T,N> input{},output{}; value.store(input.data()); output.fill(fill);
    auto bits=mask.to_bitset(); std::size_t count=0;
    for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[count++]=input[i];
    return {simd<T,N,A>::load(output.data()),count};
  }
  /// Expand a globally packed prefix into selected logical lanes, retaining prior elsewhere.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr simd<T,N,A> expand(typename simd<T,N,A>::mask mask,simd<T,N,A> packed,simd<T,N,A> prior) noexcept {
    std::array<T,N> input{},output{}; packed.store(input.data()); prior.store(output.data());
    auto bits=mask.to_bitset(); std::size_t count=0;
    for(std::size_t i=0;i<N;++i) if((bits>>i)&1) output[i]=input[count++];
    return simd<T,N,A>::load(output.data());
  }
  /// Write only the selected prefix that fits capacity; zero writes permit null.
  template<class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_operation_shape<T,N,A> &&
      (std::same_as<T,float> || std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t>)
  constexpr std::size_t compress_store(T * p,std::size_t capacity,typename simd<T,N,A>::mask mask,simd<T,N,A> value) noexcept {
    auto packed=compress(mask,value);
    auto count=std::min(capacity,packed.count); packed.value.store_partial(p,count); return count;
  }
}
