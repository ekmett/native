// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated for each disjoint backend under its native register target scope.
// Pattern: [target-scoped includes](../../README.md#target-scoped-includes).
namespace native {
  /// \ingroup vectors
  /// An explicitly permitted shape decomposed into the largest native registers.
  /// Native shapes retain their existing specializations. The final register's
  /// padding is excluded from memory transfers, comparisons and mask reductions.
  /// Scalar-only tags use one scalar per register; custom elements continue to
  /// supply their own semantics through simd_customization over this raw storage.
  template<class T,std::size_t N,isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_shape<T,N,A>
  struct simd<T,N,A> : detail::swizzle_access<T,N,A> {
    using value_type=T;
    static constexpr isa<> architecture=A;
    static constexpr std::size_t lanes=N;
    /// Number of logical lanes in each underlying native register.
    static constexpr std::size_t register_lanes=detail::polyfill_register_lanes<T,A>();
    /// Number of underlying registers, including a possible partial tail.
    static constexpr std::size_t register_count=(N+register_lanes-1)/register_lanes;
    using register_type=simd;
    using chunk_type=typename detail::polyfill_chunk<T,register_lanes,detail::hardware_isa<A>>::type;
    /// Native register representations in logical lane order.
    using native_type=std::array<typename chunk_type::native_type,register_count>;
    using mask=detail::polyfill_predicate<N,A>;
    using mask_type=mask;
    using predicate_type=mask;
    using word_type=std::conditional_t<sizeof(T)==1,std::uint8_t,
      std::conditional_t<sizeof(T)==2,std::uint16_t,
        std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>;
    using bits_type=simd<word_type,N,A>;
    using vector_mask_type=simd<mask_lane<word_type>,N,A>;
    template<class U> using rebind=simd<U,N,A>;
  private:
    alignas(typename chunk_type::native_type) native_type value_{};
    template<class F,class... V>
    static constexpr simd map(F operation,V const &... values) noexcept {
      simd result;
      for(std::size_t i=0;i<register_count;++i)
        result.value_[i]=operation(chunk_type::from_native(values.value_[i])...).to_native();
      return result;
    }
    template<class F>
    static constexpr mask compare(F operation,simd const & a,simd const & b) noexcept {
      std::uint64_t bits=0;
      for(std::size_t i=0;i<register_count;++i)
        bits|=operation(chunk_type::from_native(a.value_[i]),
          chunk_type::from_native(b.value_[i])).to_bitset()<<(i*register_lanes);
      return mask::from_bitset(bits);
    }
  public:
    /// Initialize the stored representations to zero.
    constexpr simd() noexcept=default;
    /// Broadcast an element to all logical lanes.
    constexpr simd(T value) noexcept requires(!simd_integer_element<T>) {
      for(auto & part:value_) part=chunk_type(value).to_native();
    }
    /// Reduce an integral scalar to the lane width and broadcast it.
    template<simd_integer_element U>
    constexpr simd(U value) noexcept requires(simd_integer_element<T>) {
      for(auto & part:value_) part=chunk_type(value).to_native();
    }
    /// Copy logical lanes in array order.
    constexpr simd(std::array<T,N> const & values) noexcept : simd(load(values.data())) {}
    /// Construct logical lanes in argument order.
    template<class... U> requires(sizeof...(U)==N && (std::convertible_to<U,T> && ...) &&
      (!simd_integer_element<T> || (simd_integer_element<U> && ...)))
    constexpr simd(U... values) noexcept : simd(std::array<T,N>{T(values)...}) {}
    /// Adopt register representations, normalizing Boolean and mask lane storage.
    /// Numeric representations and unspecified tail padding are preserved.
    static constexpr simd from_native(native_type value) noexcept {
      simd result;
      if constexpr(std::same_as<T,bool> || simd_mask_element<T>) {
        for(std::size_t i=0;i<register_count;++i)
          result.value_[i]=chunk_type::from_native(value[i]).to_native();
      } else result.value_=value;
      return result;
    }
    /// Adopt canonical representations; callers supply canonical mask/Boolean lanes.
    static constexpr simd unsafe_from_native(native_type value) noexcept { return from_native(value); }
    /// Adopt the underlying register array without numerical conversion.
    constexpr simd(native_type value) noexcept
      requires(!std::same_as<native_type,std::array<T,N>>) : simd(from_native(value)) {}
    /// Broadcast float lanes without imposing an element normalization policy.
    static constexpr simd from_float(float value) noexcept requires(std::same_as<T,float>) { return simd(value); }
    /// Adopt raw binary32 register storage without numerical conversion.
    static constexpr simd unsafe_from_float32(native_type value) noexcept
      requires(std::same_as<T,float>) { return from_native(value); }
    /// Project the underlying register array for native interoperability.
    constexpr operator native_type() const noexcept { return to_native(); }
    /// Return the underlying register representations in logical order.
    constexpr native_type to_native() const noexcept { return value_; }
    /// Read all logical lanes from an unaligned element pointer.
    static constexpr simd load(T const * p) noexcept { return load_partial(p,N); }
    /// Read all logical lanes without an alignment promise.
    static constexpr simd loadu(T const * p) noexcept { return load(p); }
    /// Write all logical lanes to an unaligned element pointer.
    constexpr void store(T * p) const noexcept { store_partial(p,N); }
    /// Write all logical lanes without an alignment promise.
    constexpr void storeu(T * p) const noexcept { store(p); }
    /// Load logical lanes; Alignment is the caller's byte-alignment promise.
    template<std::size_t Alignment=1>
    static constexpr simd load_memory(T const * p) noexcept { return load(p); }
    /// Store logical lanes; Alignment is the caller's byte-alignment promise.
    template<std::size_t Alignment=1>
    constexpr void store_memory(T * p) const noexcept { store(p); }
    /// Read exactly count lanes and fill the remainder. Zero count permits null.
    static constexpr simd load_partial(T const * p,std::size_t count,T fill={}) noexcept
      hint_diagnose_if(count>N,"partial SIMD count exceeds the lane count") {
      simd result;
      for(std::size_t i=0;i<register_count;++i) {
        std::array<T,register_lanes> part{};
        part.fill(fill);
        auto begin=i*register_lanes;
        auto active=begin<count?std::min(register_lanes,count-begin):0;
        if consteval {
          for(std::size_t j=0;j<active;++j) part[j]=p[begin+j];
        } else {
          if(active) std::memcpy(part.data(),reinterpret_cast<unsigned char const *>(p)+begin*sizeof(T),active*sizeof(T));
        }
        result.value_[i]=chunk_type::template load_memory<1>(part.data()).to_native();
      }
      return result;
    }
    /// Write exactly count lanes. Zero count permits null and touches nothing.
    constexpr void store_partial(T * p,std::size_t count) const noexcept
      hint_diagnose_if(count>N,"partial SIMD count exceeds the lane count") {
      for(std::size_t i=0;i<register_count && i*register_lanes<count;++i) {
        std::array<T,register_lanes> part{};
        chunk_type::from_native(value_[i]).template store_memory<1>(part.data());
        auto begin=i*register_lanes;
        auto active=std::min(register_lanes,count-begin);
        if consteval {
          for(std::size_t j=0;j<active;++j) p[begin+j]=part[j];
        } else {
          std::memcpy(reinterpret_cast<unsigned char *>(p)+begin*sizeof(T),part.data(),active*sizeof(T));
        }
      }
    }
    /// Return exact unsigned lane representations without normalization.
    constexpr bits_type bits() const noexcept requires(!std::same_as<T,bool> && !simd_mask_element<T>) {
      std::array<T,N> values{}; store(values.data());
      std::array<word_type,N> words{};
      for(std::size_t i=0;i<N;++i) words[i]=std::bit_cast<word_type>(values[i]);
      return bits_type::template load_memory<1>(words.data());
    }
    /// Return exact unsigned lane representations.
    constexpr bits_type to_bits() const noexcept requires(!std::same_as<T,bool> && !simd_mask_element<T>) { return bits(); }
    /// Adopt unsigned lane representations without conversion.
    static constexpr simd from_bits(bits_type bits) noexcept requires(!std::same_as<T,bool> && !simd_mask_element<T>) {
      std::array<word_type,N> words{}; bits.template store_memory<1>(words.data());
      return load_bits(words.data());
    }
    /// Read exact lane representations from unsigned words.
    static constexpr simd load_bits(word_type const * p) noexcept requires(!std::same_as<T,bool> && !simd_mask_element<T>) {
      std::array<word_type,N> words{};
      if consteval {
        for(std::size_t i=0;i<N;++i) words[i]=p[i];
      } else {
        std::memcpy(words.data(),p,N*sizeof(word_type));
      }
      auto values=std::bit_cast<std::array<T,N>>(words);
      return load(values.data());
    }
    /// Write exact lane representations as unsigned words.
    constexpr void store_bits(word_type * p) const noexcept requires(!std::same_as<T,bool> && !simd_mask_element<T>) {
      bits().template store_memory<1>(p);
    }
    /// Read count representation words, filling remaining lanes; zero permits null.
    static constexpr simd load_bits_partial(word_type const * p,std::size_t count,word_type fill=0) noexcept
      requires(!std::same_as<T,bool> && !simd_mask_element<T>)
      hint_diagnose_if(count>N,"partial SIMD count exceeds the lane count") {
      std::array<word_type,N> words{}; words.fill(fill);
      if consteval {
        for(std::size_t i=0;i<count;++i) words[i]=p[i];
      } else {
        if(count) std::memcpy(words.data(),p,count*sizeof(word_type));
      }
      return load_bits(words.data());
    }
    /// Store count exact lane words; zero permits null and touches nothing.
    constexpr void store_bits_partial(word_type * p,std::size_t count) const noexcept
      requires(!std::same_as<T,bool> && !simd_mask_element<T>)
      hint_diagnose_if(count>N,"partial SIMD count exceeds the lane count") {
      std::array<word_type,N> words{}; store_bits(words.data());
      if consteval {
        for(std::size_t i=0;i<count;++i) p[i]=words[i];
      } else {
        if(count) std::memcpy(p,words.data(),count*sizeof(word_type));
      }
    }
    /// Construct canonical mask lanes from a logical bitset.
    static constexpr simd from_bitset(std::uint64_t bits) noexcept requires(std::same_as<T,bool> || simd_mask_element<T>) {
      if constexpr(std::same_as<T,bool>) {
        std::array<bool,N> values{};
        for(std::size_t i=0;i<N;++i) values[i]=((bits>>i)&1)!=0;
        return load(values.data());
      } else {
        simd result;
        for(std::size_t i=0;i<register_count;++i)
          result.value_[i]=chunk_type::from_bitset(bits>>(i*register_lanes)).to_native();
        return result;
      }
    }
    /// Return logical mask lanes, excluding tail padding.
    constexpr std::uint64_t to_bitset() const noexcept requires(std::same_as<T,bool> || simd_mask_element<T>) {
      std::uint64_t bits=0;
      if constexpr(std::same_as<T,bool>) {
        std::array<bool,N> values{}; store(values.data());
        for(std::size_t i=0;i<N;++i) bits|=std::uint64_t(values[i])<<i;
      } else {
        for(std::size_t i=0;i<register_count;++i)
          bits|=chunk_type::from_native(value_[i]).to_bitset()<<(i*register_lanes);
      }
      if constexpr(N<64) bits&=(std::uint64_t{1}<<N)-1;
      return bits;
    }
#define NATIVE_POLYFILL_BINARY(OP) \
    /** Apply the native register operation to corresponding logical lanes. */ \
    friend constexpr simd operator OP(simd a,simd b) noexcept \
      requires requires(chunk_type x) { { x OP x } -> std::same_as<chunk_type>; } { \
      return map([](auto x,auto y) { return x OP y; },a,b); \
    } \
    /** Apply the corresponding operation in place. */ \
    constexpr simd & operator OP##=(simd b) noexcept \
      requires requires(chunk_type x) { { x OP x } -> std::same_as<chunk_type>; } { return *this=*this OP b; }
    NATIVE_POLYFILL_BINARY(+)
    NATIVE_POLYFILL_BINARY(-)
    NATIVE_POLYFILL_BINARY(*)
    NATIVE_POLYFILL_BINARY(/)
    NATIVE_POLYFILL_BINARY(%)
    NATIVE_POLYFILL_BINARY(&)
    NATIVE_POLYFILL_BINARY(|)
    NATIVE_POLYFILL_BINARY(^)
#undef NATIVE_POLYFILL_BINARY
#define NATIVE_POLYFILL_COMPARE(OP) \
    /** Compare logical lanes, excluding tail padding from the returned mask. */ \
    friend constexpr mask operator OP(simd a,simd b) noexcept \
      requires requires(chunk_type x) { (x OP x).to_bitset(); } { \
      return compare([](auto x,auto y) { return x OP y; },a,b); \
    }
    NATIVE_POLYFILL_COMPARE(==)
    NATIVE_POLYFILL_COMPARE(<)
#undef NATIVE_POLYFILL_COMPARE
    /// Compare corresponding lanes for inequality; unordered floats are unequal.
    friend constexpr mask operator!=(simd a,simd b) noexcept
      requires requires(chunk_type x) { (x==x).to_bitset(); } { return ~(a==b); }
    /// Compare corresponding lanes with the reverse ordered less-than relation.
    friend constexpr mask operator>(simd a,simd b) noexcept
      requires requires(chunk_type x) { (x<x).to_bitset(); } { return b<a; }
    /// Compare corresponding lanes for ordered less-than or equality.
    friend constexpr mask operator<=(simd a,simd b) noexcept
      requires requires(chunk_type x) { (x<x).to_bitset(); (x==x).to_bitset(); } { return (a<b)|(a==b); }
    /// Compare corresponding lanes for ordered greater-than or equality.
    friend constexpr mask operator>=(simd a,simd b) noexcept
      requires requires(chunk_type x) { (x<x).to_bitset(); (x==x).to_bitset(); } { return b<=a; }
    /// Negate every logical lane with the underlying register's semantics.
    friend constexpr simd operator-(simd value) noexcept
      requires requires(chunk_type x) { { -x } -> std::same_as<chunk_type>; } {
      return map([](auto x) { return -x; },value);
    }
    /// Complement every logical lane representation.
    friend constexpr simd operator~(simd value) noexcept
      requires requires(chunk_type x) { { ~x } -> std::same_as<chunk_type>; } {
      return map([](auto x) { return ~x; },value);
    }
    /// Complement every logical mask lane.
    friend constexpr simd operator!(simd value) noexcept
      requires(std::same_as<T,bool> || simd_mask_element<T>) { return from_bitset(~value.to_bitset()); }
    /// Test whether any logical mask lane is true.
    friend constexpr bool any(simd value) noexcept
      requires(std::same_as<T,bool> || simd_mask_element<T>) { return value.to_bitset()!=0; }
    /// Test whether every logical mask lane is false.
    friend constexpr bool none(simd value) noexcept
      requires(std::same_as<T,bool> || simd_mask_element<T>) { return !any(value); }
    /// Test whether every logical mask lane is true.
    friend constexpr bool all(simd value) noexcept
      requires(std::same_as<T,bool> || simd_mask_element<T>) {
      if constexpr(N==64) return value.to_bitset()==~std::uint64_t{};
      else return value.to_bitset()==(std::uint64_t{1}<<N)-1;
    }
    /// Choose each logical lane from a or b without reading padding as a result.
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,predicate<N,A>> || std::same_as<M,vector_mask_type>)
    friend constexpr simd select(M mask,simd a,simd b) noexcept {
      std::array<T,N> first{},second{}; a.store(first.data()); b.store(second.data());
      auto bits=mask.to_bitset();
      for(std::size_t i=0;i<N;++i) if(!((bits>>i)&1)) first[i]=second[i];
      return load(first.data());
    }
    /// Read one logical lane selected at compile time.
    template<std::size_t I> requires(I<N)
    constexpr T get() const noexcept { std::array<T,N> lanes{}; store(lanes.data()); return lanes[I]; }
    /// Replace one logical lane, retaining every other lane's representation.
    template<std::size_t I> requires(I<N)
    constexpr simd set(T value) const noexcept { std::array<T,N> lanes{}; store(lanes.data()); lanes[I]=value; return load(lanes.data()); }
    /// Encode normal powers of two for integral exponents in [-126,127].
    friend constexpr simd normal_pow2(simd n) noexcept requires(std::same_as<T,float>) {
      return map([](auto x) { return normal_pow2(x); },n);
    }
    /// Shift logical lanes left by the valid immediate count.
    template<std::size_t K> requires(simd_integer_element<T> && K<sizeof(T)*8)
    friend constexpr simd operator<<(simd value,imm_t<K>) noexcept { return value.template left<K>(); }
    /// Shift logical lanes right by the valid immediate count, extending signed lanes.
    template<std::size_t K> requires(simd_integer_element<T> && K<sizeof(T)*8)
    friend constexpr simd operator>>(simd value,imm_t<K>) noexcept { return value.template right<K>(); }
    /// Shift each unsigned 32-bit lane by its corresponding count; counts >=32 yield zero.
    friend constexpr simd operator<<(simd value,simd counts) noexcept requires(std::same_as<T,std::uint32_t>) {
      if constexpr(requires(chunk_type x) { { x<<x } -> std::same_as<chunk_type>; })
        return map([](auto x,auto n) { return x<<n; },value,counts);
      else {
        std::array<T,N> lanes{},shifts{}; value.store(lanes.data()); counts.store(shifts.data());
        for(std::size_t i=0;i<N;++i) lanes[i]=shifts[i]<32?lanes[i]<<shifts[i]:0;
        return load(lanes.data());
      }
    }
    /// Shift every integral lane right by the immediate count.
    template<std::size_t K>
    constexpr simd right() const noexcept requires requires(chunk_type x) { x.template right<K>(); } {
      return map([](auto x) { return x.template right<K>(); },*this);
    }
    /// Shift every integral lane left by the immediate count.
    template<std::size_t K>
    constexpr simd left() const noexcept requires requires(chunk_type x) { x.template left<K>(); } {
      return map([](auto x) { return x.template left<K>(); },*this);
    }
    /// Shift every integral lane right by a runtime count.
    friend constexpr simd operator>>(simd value,int count) noexcept
      requires requires(chunk_type x) { { x>>count } -> std::same_as<chunk_type>; } {
      return map([=](auto x) { return x>>count; },value);
    }
    /// Shift every integral lane left by a runtime count.
    friend constexpr simd operator<<(simd value,int count) noexcept
      requires requires(chunk_type x) { { x<<count } -> std::same_as<chunk_type>; } {
      return map([=](auto x) { return x<<count; },value);
    }
    /// Compute a*b+c with each register's fused lane semantics.
    friend constexpr simd fma(simd a,simd b,simd c) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x,auto y,auto z) { return fma(x,y,z); },a,b,c); }
    /// Compute square roots with each register's floating-point semantics.
    friend constexpr simd sqrt(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return sqrt(x); },value); }
    /// Count the set bits of each unsigned lane using the native chunk operation.
    friend constexpr simd popcount(simd value) noexcept
      requires(simd_integer_element<T> && std::is_unsigned_v<T>) {
      return map([](auto x) { return popcount(x); },value);
    }
    /// Clear the sign bit of each floating-point lane.
    friend constexpr simd abs(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return abs(x); },value); }
    /// Round floating-point lanes toward negative infinity.
    friend constexpr simd floor(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return floor(x); },value); }
    /// Round floating-point lanes toward positive infinity.
    friend constexpr simd ceil(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return ceil(x); },value); }
    /// Round floating-point lanes toward zero.
    friend constexpr simd trunc(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return trunc(x); },value); }
    /// Select the smaller lane using the underlying floating-point comparison.
    friend constexpr simd min(simd a,simd b) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return select(a<b,a,b); }
    /// Select the larger lane using the underlying floating-point comparison.
    friend constexpr simd max(simd a,simd b) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return select(a>b,a,b); }
    /// Round to nearest integral values, choosing even at ties.
    friend constexpr simd round_even(simd value) noexcept
      requires(detail::polyfill_element_traits<T>::arithmetic) { return map([](auto x) { return round_even(x); },value); }
  };
}

namespace native {
  /// Repartition logical integer lane representations across emulated shapes.
  /// Padding is excluded; the logical byte count must divide the result lane size.
  template<simd_integer_element To,simd_integer_element From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && sizeof(From)*N%sizeof(To)==0 &&
      requires { sizeof(simd<From,N,A>); sizeof(simd<To,sizeof(From)*N/sizeof(To),A>); } &&
      (detail::polyfill_shape<From,N,A> || detail::polyfill_shape<To,sizeof(From)*N/sizeof(To),A> ||
       sizeof(typename simd<From,N,A>::native_type)!=sizeof(typename simd<To,sizeof(From)*N/sizeof(To),A>::native_type)))
  constexpr auto reinterpret_bits(simd<From,N,A> value) noexcept {
    std::array<From,N> input{}; value.store(input.data());
    auto output=std::bit_cast<std::array<To,sizeof(From)*N/sizeof(To)>>(input);
    return simd<To,sizeof(From)*N/sizeof(To),A>::template load_memory<1>(output.data());
  }

  /// Sum each adjacent pair into an unsigned lane twice as wide, without overflow.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (detail::polyfill_operation_shape<T,N,A> && std::is_unsigned_v<T> && sizeof(T)<=4 && N%2==0)
  constexpr auto pairwise_add_widened(simd<T,N,A> value) noexcept {
    using U=std::conditional_t<sizeof(T)==1,std::uint16_t,
      std::conditional_t<sizeof(T)==2,std::uint32_t,std::uint64_t>>;
    std::array<T,N> input{}; value.store(input.data());
    std::array<U,N/2> output{};
    for(std::size_t i=0;i<N/2;++i) output[i]=U(input[2*i])+input[2*i+1];
    return simd<U,N/2,A>::template load_memory<1>(output.data());
  }

  /// Truncate unsigned lanes and concatenate a then b, retaining low result bits.
  template<simd_integer_element To,simd_integer_element From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && std::is_unsigned_v<To> && std::is_unsigned_v<From> &&
      sizeof(From)==2*sizeof(To) &&
      (detail::polyfill_operation_shape<From,N,A> || detail::polyfill_operation_shape<To,2*N,A>) &&
      requires { sizeof(simd<To,2*N,A>); })
  constexpr simd<To,2*N,A> narrow_concat(simd<From,N,A> a,simd<From,N,A> b) noexcept {
    std::array<From,N> first{},second{}; a.store(first.data()); b.store(second.data());
    std::array<To,2*N> output{};
    for(std::size_t i=0;i<N;++i) {
      output[i]=static_cast<To>(first[i]);
      output[N+i]=static_cast<To>(second[i]);
    }
    return simd<To,2*N,A>::template load_memory<1>(output.data());
  }
}
