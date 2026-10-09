// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace native::detail {
  // Hardware-only lookup must never recursively select the emulated shape.
  template<isa<> A> inline constexpr isa<> hardware_isa=[] {
    auto result=A;
    result.allow_polyfill=false;
    return result;
  }();
  template<class T,std::size_t N,isa<> A>
  concept polyfill_shape=ordinary_simd_element<T> && A.has(polyfill) && N>0 && N<=64 &&
    !requires { sizeof(simd<T,N,hardware_isa<A>>); };

  template<class T,isa<> A,std::size_t M=64/sizeof(T)>
  consteval std::size_t polyfill_register_lanes() noexcept {
    if constexpr(requires { typename simd<T,M,hardware_isa<A>>::native_type; }) return M;
    else if constexpr(M>1) return polyfill_register_lanes<T,A,M/2>();
    else return 1;
  }
}

namespace native {
  /// \ingroup masks
  /// Compact logical mask for an emulated shape, with unused bits cleared.
  template<std::size_t N,isa<> A>
    requires(A.has(polyfill) && N>0 && N<=64 &&
      !requires { sizeof(predicate<N,detail::hardware_isa<A>>); })
  struct predicate<N,A> {
    static constexpr isa<> architecture=A;
    static constexpr std::size_t lanes=N;
    using native_type=std::uint64_t;
    using mask_type=predicate;
    using mask=predicate;
    static constexpr bool compact=true;
  private:
    native_type value_{};
    static constexpr native_type active=[] {
      if constexpr(N==64) return ~native_type{};
      else return (native_type{1}<<N)-1;
    }();
  public:
    /// Initialize every logical mask lane to false.
    constexpr predicate() noexcept=default;
    /// Broadcast one truth value to the logical lanes.
    explicit constexpr predicate(bool value) noexcept : value_(value?active:0) {}
    /// Copy one bit per logical lane, clearing unused high bits.
    static constexpr predicate from_bitset(native_type bits) noexcept {
      predicate result; result.value_=bits&active; return result;
    }
    /// Copy a logical mask bitset.
    static constexpr predicate from_bits(native_type bits) noexcept { return from_bitset(bits); }
    /// Adopt the compact storage, clearing padding bits.
    static constexpr predicate from_native(native_type bits) noexcept { return from_bitset(bits); }
    /// Return the compact representation.
    constexpr native_type to_native() const noexcept { return value_; }
    /// Return one bit per logical mask lane.
    constexpr native_type to_bitset() const noexcept { return value_; }
    /// Return one bit per logical mask lane.
    constexpr native_type bits() const noexcept { return value_; }
    /// Test whether any logical lane is true.
    friend constexpr bool any(predicate value) noexcept { return value.value_!=0; }
    /// Test whether every logical lane is true.
    friend constexpr bool all(predicate value) noexcept { return value.value_==active; }
    /// Test whether every logical lane is false.
    friend constexpr bool none(predicate value) noexcept { return value.value_==0; }
    /// Complement every logical lane, leaving padding clear.
    friend constexpr predicate operator~(predicate value) noexcept { return from_bitset(~value.value_); }
    /// Complement every logical lane.
    friend constexpr predicate operator!(predicate value) noexcept { return ~value; }
    /// Intersect corresponding mask lanes.
    friend constexpr predicate operator&(predicate a,predicate b) noexcept { return from_bitset(a.value_&b.value_); }
    /// Unite corresponding mask lanes.
    friend constexpr predicate operator|(predicate a,predicate b) noexcept { return from_bitset(a.value_|b.value_); }
    /// Toggle corresponding mask lanes.
    friend constexpr predicate operator^(predicate a,predicate b) noexcept { return from_bitset(a.value_^b.value_); }
    /// Compare corresponding mask truth values.
    friend constexpr predicate operator==(predicate a,predicate b) noexcept { return ~(a^b); }
    /// Compare corresponding mask truth values for inequality.
    friend constexpr predicate operator!=(predicate a,predicate b) noexcept { return a^b; }
    /// Intersect with another mask in place.
    constexpr predicate & operator&=(predicate b) noexcept { return *this=*this&b; }
    /// Unite with another mask in place.
    constexpr predicate & operator|=(predicate b) noexcept { return *this=*this|b; }
    /// Toggle another mask in place.
    constexpr predicate & operator^=(predicate b) noexcept { return *this=*this^b; }
    /// Choose each mask lane from a or b.
    friend constexpr predicate select(predicate m,predicate a,predicate b) noexcept { return (m&a)|(~m&b); }
  };

  /// \ingroup vectors
  /// An explicitly permitted shape decomposed into the largest native registers.
  /// Native shapes retain their existing specializations. The final register's
  /// padding is excluded from memory transfers, comparisons and mask reductions.
  /// Scalar-only tags use one scalar per register; custom elements continue to
  /// supply their own semantics through simd_customization over this raw storage.
  template<class T,std::size_t N,isa<> A> requires detail::polyfill_shape<T,N,A>
  struct simd<T,N,A> : detail::swizzle_access<T,N,A> {
    using value_type=T;
    static constexpr isa<> architecture=A;
    static constexpr std::size_t lanes=N;
    /// Number of logical lanes in each underlying native register.
    static constexpr std::size_t register_lanes=detail::polyfill_register_lanes<T,A>();
    /// Number of underlying registers, including a possible partial tail.
    static constexpr std::size_t register_count=(N+register_lanes-1)/register_lanes;
    using register_type=simd;
    using chunk_type=simd<T,register_lanes,detail::hardware_isa<A>>;
    /// Native register representations in logical lane order.
    using native_type=std::array<typename chunk_type::native_type,register_count>;
    using mask=predicate<N,A>;
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
    constexpr simd(T value) noexcept {
      for(auto & part:value_) part=chunk_type(value).to_native();
    }
    /// Copy logical lanes in array order.
    constexpr simd(std::array<T,N> const & values) noexcept : simd(load(values.data())) {}
    /// Construct logical lanes in argument order.
    template<class... U> requires(sizeof...(U)==N && (std::convertible_to<U,T> && ...))
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
        for(std::size_t j=0;j<register_lanes && begin+j<count;++j) part[j]=p[begin+j];
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
        for(std::size_t j=0;j<register_lanes && begin+j<count;++j) p[begin+j]=part[j];
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
      std::array<T,N> values{};
      for(std::size_t i=0;i<N;++i) values[i]=std::bit_cast<T>(p[i]);
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
      for(std::size_t i=0;i<count;++i) words[i]=p[i];
      return load_bits(words.data());
    }
    /// Store count exact lane words; zero permits null and touches nothing.
    constexpr void store_bits_partial(word_type * p,std::size_t count) const noexcept
      requires(!std::same_as<T,bool> && !simd_mask_element<T>)
      hint_diagnose_if(count>N,"partial SIMD count exceeds the lane count") {
      std::array<word_type,N> words{}; store_bits(words.data());
      for(std::size_t i=0;i<count;++i) p[i]=words[i];
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
    NATIVE_POLYFILL_COMPARE(!=)
    NATIVE_POLYFILL_COMPARE(<)
    NATIVE_POLYFILL_COMPARE(<=)
    NATIVE_POLYFILL_COMPARE(>)
    NATIVE_POLYFILL_COMPARE(>=)
#undef NATIVE_POLYFILL_COMPARE
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
    template<class M> requires requires(M m) { m.to_bitset(); }
    friend constexpr simd select(M mask,simd a,simd b) noexcept {
      std::array<T,N> first{},second{}; a.store(first.data()); b.store(second.data());
      auto bits=mask.to_bitset();
      for(std::size_t i=0;i<N;++i) if(!((bits>>i)&1)) first[i]=second[i];
      return load(first.data());
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
      requires(std::same_as<T,float>) { return map([](auto x,auto y,auto z) { return fma(x,y,z); },a,b,c); }
    /// Compute square roots with each register's floating-point semantics.
    friend constexpr simd sqrt(simd value) noexcept
      requires(std::same_as<T,float>) { return map([](auto x) { return sqrt(x); },value); }
    /// Round to nearest integral values, choosing even at ties.
    friend constexpr simd round_even(simd value) noexcept
      requires(std::same_as<T,float>) { return map([](auto x) { return round_even(x); },value); }
  };
}
