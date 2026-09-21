// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Register shapes needed by instruction interfaces outside the arithmetic kernels.
export namespace native {
  namespace detail {
    template<class T> concept instruction_element = simd_integer_element<T> ||
      std::same_as<T,float> || std::same_as<T,double> ||
      std::same_as<T,fp16> || std::same_as<T,bf16>;

    template<class T, std::size_t N, isa A>
    inline constexpr bool instruction_storage_shape = [] {
      if constexpr(!instruction_element<T> || N<2) return false;
      else {
        constexpr auto bytes=sizeof(T)*N;
#if NATIVE_HOST_NEON
        if constexpr(!(neon<=A)) return false;
        if constexpr(std::same_as<T,fp16>) return N==4 || (N==8 && !(neon_fp16<=A));
        if constexpr(std::same_as<T,bf16>) return N==4 || (N==8 && !(neon_bf16<=A));
        if constexpr(std::same_as<T,double>) return N==2;
        // Existing two-lane 32-bit vectors have intentional four-lane storage.
        return simd_integer_element<T> && sizeof(T)<4 && bytes==8;
#elif NATIVE_HOST_X86
        if constexpr(!A.has(x86_feature::sse2)) return false;
        if constexpr(std::same_as<T,fp16>) return N==4 || N==8;
        if constexpr(std::same_as<T,bf16>) return false;
        if constexpr(bytes==8) return simd_integer_element<T> && sizeof(T)<4;
        if constexpr(bytes==16 || bytes==32) {
          if constexpr(bytes==32 && !A.has(x86_feature::avx)) return false;
          return std::same_as<T,double> || !(avx2<=A);
        }
        if constexpr(bytes==64 && A.has(x86_feature::avx512f)) {
          if constexpr(std::same_as<T,double>) return true;
          return !(kernel_base<=A) || (sizeof(T)<4 && !A.has(x86_feature::avx512bw));
        }
        return false;
#else
        return false;
#endif
      }
    }();

    template<class T,std::size_t N,isa A>
      requires ordinary_simd_element<T> && instruction_storage_shape<T,N,A>
    struct value_traits<simd<T,N,A>> {
      static constexpr isa value=A;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
#if NATIVE_HOST_X86
    template<std::size_t N,isa A> requires(N>0 && N<=64 && A.has(x86_feature::sse2) && !(avx2<=A))
    struct value_traits<predicate<N,A>> {
      static constexpr isa value=A;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
#endif

    template<class T, std::size_t N> struct instruction_register {
#if NATIVE_HOST_NEON
      using type=std::conditional_t<std::same_as<T,double>,float64x2_t,
        std::conditional_t<(sizeof(T)*N<=8),uint8x8_t,uint8x16_t>>;
#elif NATIVE_HOST_X86
      using type=std::conditional_t<std::same_as<T,float>,
        std::conditional_t<(sizeof(T)*N<=16),__m128,std::conditional_t<(sizeof(T)*N==32),__m256,__m512>>,
        std::conditional_t<std::same_as<T,double>,
          std::conditional_t<(sizeof(T)*N<=16),__m128d,std::conditional_t<(sizeof(T)*N==32),__m256d,__m512d>>,
          std::conditional_t<(sizeof(T)*N<=16),__m128i,std::conditional_t<(sizeof(T)*N==32),__m256i,__m512i>>>>;
#endif
    };
  }

#if NATIVE_HOST_X86
  /// Compact logical mask for register-only instruction shapes below the AVX2 kernel profile.
  template<std::size_t N, isa A> requires(N>0 && N<=64 && A.has(x86_feature::sse2) && !(avx2<=A))
  struct predicate<N,A> {
    static constexpr isa architecture=A;
    static constexpr std::size_t lanes=N;
    using native_type=std::conditional_t<(N<=8),std::uint8_t,
      std::conditional_t<(N<=16),std::uint16_t,std::conditional_t<(N<=32),std::uint32_t,std::uint64_t>>>;
    using mask=predicate;
    using mask_type=predicate;
    static constexpr bool compact=true;
  private:
    native_type value_{};
    static constexpr std::uint64_t active=[] { if constexpr(N==64) return ~std::uint64_t{}; else return (std::uint64_t{1}<<N)-1; }();
  public:
    /// Construct an empty mask.
    constexpr predicate() noexcept=default;
    /// Broadcast one Boolean value to every logical lane.
    explicit constexpr predicate(bool value) noexcept : value_(value?native_type(active):0) {}
    /// Read one bit per lane and clear bits above the logical lane count.
    static constexpr predicate from_bits(std::uint64_t bits) noexcept { predicate p; p.value_=native_type(bits&active); return p; }
    /// Construct from the logical lane bitset.
    static constexpr predicate from_bitset(std::uint64_t bits) noexcept { return from_bits(bits); }
    /// Adopt the compact representation, clearing unused bits.
    static constexpr predicate from_native(native_type bits) noexcept { return from_bits(bits); }
    /// Return the compact implementation representation.
    constexpr native_type to_native() const noexcept { return value_; }
    /// Return one bit per logical lane.
    constexpr std::uint64_t bits() const noexcept { return value_; }
    /// Return the logical lane bitset.
    constexpr std::uint64_t to_bitset() const noexcept { return value_; }
    /// Test whether any logical lane is set.
    friend constexpr bool any(predicate p) noexcept { return p.value_!=0; }
    /// Test whether every logical lane is set.
    friend constexpr bool all(predicate p) noexcept { return p.value_==active; }
    /// Test whether every logical lane is clear.
    friend constexpr bool none(predicate p) noexcept { return p.value_==0; }
    /// Complement logical lanes, leaving padding clear.
    friend constexpr predicate operator~(predicate p) noexcept { return from_bits(~p.value_); }
    /// Intersect the logical lane masks.
    friend constexpr predicate operator&(predicate a,predicate b) noexcept { return from_bits(a.value_&b.value_); }
    /// Unite the logical lane masks.
    friend constexpr predicate operator|(predicate a,predicate b) noexcept { return from_bits(a.value_|b.value_); }
    /// Toggle lanes present in exactly one operand.
    friend constexpr predicate operator^(predicate a,predicate b) noexcept { return from_bits(a.value_^b.value_); }
  };
#endif

  /// One register of representation-preserving instruction operands.
  /// This shape provides storage and transfer operations; arithmetic is supplied
  /// by the instruction modules supported by its architecture tag.
  template<class T, std::size_t N, isa A> requires detail::instruction_storage_shape<T,N,A>
  struct simd<T,N,A> {
    using value_type=T;
    using register_type=simd;
    using native_type=typename detail::instruction_register<T,N>::type;
    static constexpr isa architecture=A;
    static constexpr std::size_t lanes=N;
    using mask=predicate<N,A>;
    using mask_type=mask;
    using predicate_type=mask;
    using bits_type=simd<std::conditional_t<sizeof(T)==1,std::uint8_t,
      std::conditional_t<sizeof(T)==2,std::uint16_t,
        std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>,N,A>;
    template<class U> using rebind=simd<U,N,A>;
  private:
    native_type value_;
  public:
    /// Default initialization leaves storage unspecified; braces zero it.
    simd() noexcept=default;
    /// Copy each lane's representation; unused physical bytes are zero.
    native_inline explicit simd(T value) noexcept {
      std::array<T,N> values;
      values.fill(value);
      *this=load(values.data());
    }
    /// Copy the array elements in lane order.
    native_inline explicit simd(std::array<T,N> const & values) noexcept : simd(load(values.data())) {}
    /// Construct exactly N lanes from values of the element type.
    template<class... U> requires(sizeof...(U)==N && (std::same_as<U,T> && ...))
    native_inline simd(U... values) noexcept : simd(std::array<T,N>{values...}) {}
    /// Bridge to the implementation register without numerical conversion.
    native_nodiscard native_inline native_type to_native() const noexcept { return value_; }
    /// Adopt implementation bits and clear unused physical bytes.
    native_nodiscard static native_inline simd from_native(native_type value) noexcept {
      simd result;
      if constexpr(sizeof(native_type)==sizeof(T)*N) result.value_=value;
      else { result.value_={}; std::memcpy(&result.value_,&value,sizeof(T)*N); }
      return result;
    }
    /// Read exactly N objects, without requiring register-width alignment.
    template<std::size_t Alignment=1>
    native_nodiscard static native_inline simd load_memory(T const * p) noexcept {
      static_assert(Alignment>0 && (Alignment&(Alignment-1))==0);
      simd result{};
      std::memcpy(&result.value_,p,sizeof(T)*N);
      return result;
    }
    /// Write exactly N objects; Alignment is a caller promise.
    template<std::size_t Alignment=1>
    native_inline void store_memory(T * p) const noexcept {
      static_assert(Alignment>0 && (Alignment&(Alignment-1))==0);
      std::memcpy(p,&value_,sizeof(T)*N);
    }
    /// Read exactly N elements with their natural alignment.
    native_nodiscard static native_inline simd load(T const * p) noexcept { return load_memory(p); }
    /// Synonym for load; register alignment is unnecessary.
    native_nodiscard static native_inline simd loadu(T const * p) noexcept { return load(p); }
    /// Write exactly N elements in lane order.
    native_inline void store(T * p) const noexcept { store_memory(p); }
    /// Synonym for store; register alignment is unnecessary.
    native_inline void storeu(T * p) const noexcept { store(p); }
    /// Read n <= N elements and fill the remainder; a null pointer is valid for n == 0.
    native_nodiscard static native_inline simd load_partial(T const * p,std::size_t n,T fill=T{}) noexcept {
      assert(n<=N);
      std::array<T,N> values; values.fill(fill);
      if(n) std::memcpy(values.data(),p,n*sizeof(T));
      return load(values.data());
    }
    /// Write the first n <= N elements; a null pointer is valid for n == 0.
    native_inline void store_partial(T * p,std::size_t n) const noexcept {
      assert(n<=N);
      if(n) std::memcpy(p,&value_,n*sizeof(T));
    }
    /// Return each half lane as its unchanged unsigned representation.
    native_nodiscard native_inline bits_type bits() const noexcept requires(std::same_as<T,fp16> || std::same_as<T,bf16>) {
      std::array<std::uint16_t,N> words;
      std::memcpy(words.data(),&value_,sizeof(T)*N);
      return bits_type::load(words.data());
    }
    /// Synonym for bits; no floating-point conversion occurs.
    native_nodiscard native_inline bits_type to_bits() const noexcept requires(std::same_as<T,fp16> || std::same_as<T,bf16>) { return bits(); }
    /// Interpret unsigned words as half representations without conversion.
    native_nodiscard static native_inline simd from_bits(bits_type words) noexcept requires(std::same_as<T,fp16> || std::same_as<T,bf16>) {
      std::array<std::uint16_t,N> values; words.store(values.data());
      return load_bits(values.data());
    }
    /// Read N unsigned half representations without numerical conversion.
    native_nodiscard static native_inline simd load_bits(std::uint16_t const * p) noexcept requires(std::same_as<T,fp16> || std::same_as<T,bf16>) {
      simd result{}; std::memcpy(&result.value_,p,2*N); return result;
    }
    /// Write N unsigned half representations without numerical conversion.
    native_inline void store_bits(std::uint16_t * p) const noexcept requires(std::same_as<T,fp16> || std::same_as<T,bf16>) { std::memcpy(p,&value_,2*N); }
  };
}
