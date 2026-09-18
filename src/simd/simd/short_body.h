#if defined(__x86_64__) || defined(_M_X64)
#elif defined(__aarch64__) || defined(_M_ARM64)
#endif

namespace simd {
  namespace detail::SIMD_BACKEND {
    template<class T> concept short_element = std::same_as<T,float> ||
      std::same_as<T,std::int32_t> || std::same_as<T,std::uint32_t> || std::same_as<T,mask32>;
    template<class T> using short_lane = std::conditional_t<simd_mask_element<T>,std::uint32_t,T>;
    template<class T> using short_native = T __attribute__((ext_vector_type(4)));
    template<class T> constexpr short_lane<T> short_word(T value) noexcept {
      if constexpr(simd_mask_element<T>) return value.to_bits();
      else return value;
    }
  }

  // Logical short vectors use one physical four-lane register. Floating padding
  // stays zero; division supplies one in the unused denominator lanes.
  /// \ingroup vectors
  /// Two or three logical lanes in the corresponding four-lane native storage.
  /// Loads and stores touch only logical lanes; mask reductions ignore padding.
  /// Division supplies harmless inactive operands before using the full register.
  /// \snippet api.cc swizzles
  template<SIMD_BACKEND_NAMESPACE::short_element T,std::size_t N,SIMD_ARCH_CONCEPT Arch>
    requires(N==2 || N==3) && requires { typename vec<T,4,Arch>::native_type; }
  struct vec<T,N,Arch> : detail::swizzle_access<T,N,Arch> {
    using value_type=T;
    using architecture=Arch;
    using required_architecture=SIMD_DEFAULT_ARCH;
    using required_architecture_owner = vec;
    using storage_type=vec<T,4,Arch>;
    using native_type=SIMD_BACKEND_NAMESPACE::short_native<SIMD_BACKEND_NAMESPACE::short_lane<T>>;
    using register_type=vec;
    using unsigned_register_tag=void;
    using bits_type=vec<std::uint32_t,N,Arch>;
    using vector_mask_type=vec<mask32,N,Arch>;
    using mask_type=std::conditional_t<simd_mask_element<T>,vec,
      std::conditional_t<bool(SIMD_HAS_AVX512VL),predicate<N,Arch>,vector_mask_type>>;
    using mask=mask_type;
    using predicate_type=predicate<N,Arch>;
    template<class U> using rebind=vec<U,N,Arch>;
    static constexpr std::size_t lanes=N;
    static constexpr std::size_t storage_lanes=4;
    static constexpr bool compact=false;
    static constexpr std::uint64_t lane_mask=(std::uint64_t(1)<<N)-1;
    native_type value;

    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    simd_inline constexpr vec() = default;
    /// Copy the stored value without arithmetic or normalization.
    simd_inline constexpr vec(vec const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    simd_inline constexpr vec & operator=(vec const &) = default;
    /// Broadcast the supplied value to each logical lane. Unused physical lanes are zero.
    simd_inline constexpr vec(T x) noexcept : value{SIMD_BACKEND_NAMESPACE::short_word(x),SIMD_BACKEND_NAMESPACE::short_word(x),
      N==3?SIMD_BACKEND_NAMESPACE::short_word(x):SIMD_BACKEND_NAMESPACE::short_lane<T>(0),0} {}
    /// Broadcast the supplied value to each logical lane. Unused physical lanes are zero.
    explicit simd_inline constexpr vec(bool x) noexcept requires simd_mask_element<T> : vec(T(x)) {}
    /// Adopt native lane storage and clear physical padding. Mask elements must already be canonical.
    simd_inline constexpr vec(native_type x) noexcept
      : value(__builtin_shufflevector(x,native_type{},0,1,N==3?2:4,4)) {}
    /// Construct logical lanes in argument order. Any element conversions determine the exception specification.
    template<class... X> requires(sizeof...(X)==N) && (std::convertible_to<X,T> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<T>(x)) && ...))
      : value{SIMD_BACKEND_NAMESPACE::short_word(static_cast<T>(x))...} {}
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<vec,X...>
    simd_inline constexpr vec(Arch,X &&... x) noexcept(std::is_nothrow_constructible_v<vec,X...>)
      : vec(std::forward<X>(x)...) {}
    /// Copy one value per logical lane in array order.
    simd_inline vec(std::array<T,N> const & values) noexcept : vec(load(values.data())) {}

    /// Return the native storage representation, including physical padding when present.
    simd_nodiscard simd_inline constexpr native_type to_native() const noexcept { return value; }
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline constexpr operator native_type() const noexcept requires(!simd_mask_element<T>) { return value; }
    /// Import native lanes, normalizing mask elements; other elements retain their bits. Clear physical padding.
    simd_nodiscard static simd_inline vec from_native(native_type x) noexcept {
      if constexpr(simd_mask_element<T>) return from_storage(storage_type::from_native(std::bit_cast<typename storage_type::native_type>(x)));
      else return vec(x);
    }
    /// Adopt native bits and clear physical padding. If T is a mask element, every logical lane must already be canonical.
    simd_nodiscard static simd_inline constexpr vec unsafe_from_native(native_type x) noexcept { return vec(x); }
    /// Return the corresponding four-lane storage vector without changing logical lane bits.
    simd_nodiscard simd_inline storage_type to_storage() const noexcept {
      if constexpr(simd_mask_element<T>) return storage_type::unsafe_from_native(std::bit_cast<typename storage_type::native_type>(value));
      else return storage_type::from_native(std::bit_cast<typename storage_type::native_type>(value));
    }
    /// Copy the first N lanes from a four-lane storage value and clear physical padding.
    simd_nodiscard static simd_inline vec from_storage(storage_type x) noexcept {
      return vec(std::bit_cast<native_type>(x.to_native()));
    }
    // The caller supplies exactly N logical lanes; alignment never grants a
    // readable fourth lane. Three-lane x86 transfers use native masked memory.
    /// Load exactly the logical lanes; the template alignment is a caller promise, never permission to read padding.
    template<std::size_t Alignment=1>
    simd_nodiscard static simd_inline vec load_memory(T const * p) noexcept {
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(bool(SIMD_HAS_AVX2)) {
        if constexpr(N==2) return vec(unchecked{},std::bit_cast<native_type>(_mm_loadl_epi64(reinterpret_cast<__m128i const *>(p))));
        else if constexpr(bool(SIMD_HAS_AVX512VL)) {
          if constexpr(std::same_as<T,float>) return vec(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_ps(7,p)));
          else return vec(unchecked{},std::bit_cast<native_type>(_mm_maskz_loadu_epi32(7,p)));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) return vec(unchecked{},std::bit_cast<native_type>(_mm_maskload_ps(p,active)));
          else return vec(unchecked{},std::bit_cast<native_type>(_mm_maskload_epi32(reinterpret_cast<int const *>(p),active)));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr(::simd::detail::neon_architecture<Arch>) {
        if constexpr(std::same_as<T,float>) {
          auto x=vcombine_f32(vld1_f32(p),vdup_n_f32(0.f));
          if constexpr(N==3) x=vld1q_lane_f32(p+2,x,2);
          return vec(unchecked{},std::bit_cast<native_type>(x));
        } else {
          auto q=reinterpret_cast<std::uint32_t const *>(p);
          auto x=vcombine_u32(vld1_u32(q),vdup_n_u32(0));
          if constexpr(N==3) x=vld1q_lane_u32(q+2,x,2);
          return vec(unchecked{},std::bit_cast<native_type>(x));
        }
      }
#endif
    }
    /// Store exactly the logical lanes; the template alignment is a caller promise, never permission to write padding.
    template<std::size_t Alignment=1>
    simd_inline void store_memory(T * p) const noexcept {
#if defined(__x86_64__) || defined(_M_X64)
      if constexpr(bool(SIMD_HAS_AVX2)) {
        if constexpr(N==2) _mm_storel_epi64(reinterpret_cast<__m128i *>(p),std::bit_cast<__m128i>(value));
        else if constexpr(bool(SIMD_HAS_AVX512VL)) {
          if constexpr(std::same_as<T,float>) _mm_mask_storeu_ps(p,7,std::bit_cast<__m128>(value));
          else _mm_mask_storeu_epi32(p,7,std::bit_cast<__m128i>(value));
        } else {
          auto active=_mm_set_epi32(0,-1,-1,-1);
          if constexpr(std::same_as<T,float>) _mm_maskstore_ps(p,active,std::bit_cast<__m128>(value));
          else _mm_maskstore_epi32(reinterpret_cast<int *>(p),active,std::bit_cast<__m128i>(value));
        }
      }
#elif defined(__aarch64__) || defined(_M_ARM64)
      if constexpr(::simd::detail::neon_architecture<Arch>) {
        if constexpr(std::same_as<T,float>) {
          auto x=std::bit_cast<float32x4_t>(value);vst1_f32(p,vget_low_f32(x));
          if constexpr(N==3) vst1q_lane_f32(p+2,x,2);
        } else {
          auto q=reinterpret_cast<std::uint32_t *>(p);auto x=std::bit_cast<uint32x4_t>(value);
          vst1_u32(q,vget_low_u32(x));
          if constexpr(N==3) vst1q_lane_u32(q+2,x,2);
        }
      }
#endif
    }
    /// Load exactly the logical lanes; no extra pointer alignment is required.
    simd_nodiscard static simd_inline vec load(T const * p) noexcept { return load_memory(p); }
    /// Load exactly the logical lanes without an additional alignment requirement.
    simd_nodiscard static simd_inline vec loadu(T const * p) noexcept { return load_memory(p); }
    /// Store exactly the logical lanes; no extra pointer alignment is required.
    simd_inline void store(T * p) const noexcept { store_memory(p); }
    /// Store exactly the logical lanes without an additional alignment requirement.
    simd_inline void storeu(T * p) const noexcept { store_memory(p); }
    /// Read exactly n logical lanes and fill the remainder; require n <= lanes. For n == 0, p may be null.
    simd_nodiscard static simd_inline vec load_partial(T const * p,std::size_t n,T fill={}) noexcept {
      std::array<T,N> values;values.fill(fill);
      if(n) std::memcpy(values.data(),p,n*sizeof(T));
      return load(values.data());
    }
    /// Write exactly n logical lanes; require n <= lanes. For n == 0, p may be null.
    simd_inline void store_partial(T * p,std::size_t n) const noexcept {
      std::array<T,N> values;store(values.data());
      if(n) std::memcpy(p,values.data(),n*sizeof(T));
    }
    /// Return the exact binary32 lane representations in the unsigned vector.
    simd_nodiscard simd_inline bits_type bits() const noexcept requires std::same_as<T,float> {
      return bits_type::from_storage(to_storage().bits());
    }
    /// Return the exact binary32 lane representations in the unsigned vector.
    simd_nodiscard simd_inline bits_type to_bits() const noexcept requires std::same_as<T,float> { return bits(); }
    /// Reinterpret binary32 words as lane values without normalization; a scalar word is broadcast.
    simd_nodiscard static simd_inline vec from_bits(bits_type x) noexcept requires std::same_as<T,float> {
      return from_storage(storage_type::from_bits(x.to_storage()));
    }
    /// Reinterpret binary32 words as lane values without normalization; a scalar word is broadcast.
    simd_nodiscard static simd_inline vec from_bits(std::uint32_t x) noexcept requires std::same_as<T,float> { return from_bits(bits_type(x)); }
    /// Broadcast the float value without adding an FTZ or other normalization policy.
    simd_nodiscard static simd_inline vec from_float(float x) noexcept requires std::same_as<T,float> { return vec(x); }
    /// Adopt raw float storage without numerical conversion or normalization.
    simd_nodiscard static simd_inline vec unsafe_from_float32(native_type x) noexcept requires std::same_as<T,float> { return vec(x); }
    /// Load exactly the logical count of binary32 words without normalizing their representations.
    simd_nodiscard static simd_inline vec load_bits(std::uint32_t const * p) noexcept requires std::same_as<T,float> { return from_bits(bits_type::load(p)); }
    /// Store the exact binary32 words for every logical lane.
    simd_inline void store_bits(std::uint32_t * p) const noexcept requires std::same_as<T,float> { bits().store(p); }
    /// Read n representation words and fill the remaining logical lanes; require n <= lanes. A zero count permits null.
    simd_nodiscard static simd_inline vec load_bits_partial(std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept requires std::same_as<T,float> { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Write n exact representation words; require n <= lanes. A zero count permits null.
    simd_inline void store_bits_partial(std::uint32_t * p,std::size_t n) const noexcept requires std::same_as<T,float> { bits().store_partial(p,n); }
    /// Import lane i from bit i, clearing bits above the logical lane count.
    simd_nodiscard static simd_inline vec from_bitset(std::uint64_t bits) noexcept requires simd_mask_element<T> { return from_storage(storage_type::from_bitset(bits&lane_mask)); }
    /// Pack each logical lane truth value into bit i; higher bits are zero.
    simd_nodiscard simd_inline std::uint64_t to_bitset() const noexcept requires simd_mask_element<T> { return to_storage().to_bitset()&lane_mask; }
    /// Return true when at least one logical lane is true.
    simd_nodiscard friend simd_inline bool any(vec x) noexcept requires simd_mask_element<T> { return x.to_bitset()!=0; }
    /// Return true exactly when every logical lane is true.
    simd_nodiscard friend simd_inline bool all(vec x) noexcept requires simd_mask_element<T> { return x.to_bitset()==lane_mask; }
    /// Return true exactly when no logical lane is true.
    simd_nodiscard friend simd_inline bool none(vec x) noexcept requires simd_mask_element<T> { return !any(x); }

    /// Add corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    simd_nodiscard friend simd_inline vec operator+(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()+b.to_storage()); }
    /// Subtract corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    simd_nodiscard friend simd_inline vec operator-(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()-b.to_storage()); }
    /// Multiply corresponding logical lanes using the full-register operation.
    /// Integer results wrap at the lane width; floating results follow the caller's environment.
    simd_nodiscard friend simd_inline vec operator*(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return clean(a.to_storage()*b.to_storage()); }
    /// Divide corresponding logical lanes using the full-register operation.
    /// Inactive denominator lanes are set to one so padding does not introduce division by zero.
    simd_nodiscard friend simd_inline vec operator/(vec a,vec b) noexcept requires std::same_as<T,float> {
      using M4=typename storage_type::vector_mask_type;
      auto divisor=select(M4::from_bitset(lane_mask),b.to_storage(),storage_type(1.f));
      return clean(a.to_storage()/divisor);
    }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_nodiscard friend simd_inline vec operator-(vec a) noexcept requires(!simd_mask_element<T>) { return from_storage(-a.to_storage()); }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline vec operator&(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()&b.to_storage()); }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline vec operator|(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()|b.to_storage()); }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline vec operator^(vec a,vec b) noexcept requires(!std::same_as<T,float>) { return from_storage(a.to_storage()^b.to_storage()); }
    /// Complement every bit in every lane.
    simd_nodiscard friend simd_inline vec operator~(vec a) noexcept requires(!std::same_as<T,float>) { return from_storage(~a.to_storage()); }
    /// Return the lane-wise logical complement, retaining this mask type.
    simd_nodiscard friend simd_inline vec operator!(vec a) noexcept requires simd_mask_element<T> { return ~a; }
    /// Return a mask whose lanes are true where `a == b` holds.
    simd_nodiscard friend simd_inline mask_type operator==(vec a,vec b) noexcept { return comparison(a.to_storage()==b.to_storage()); }
    /// Return a mask whose lanes are true where `a != b` holds.
    simd_nodiscard friend simd_inline mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a < b` holds.
    simd_nodiscard friend simd_inline mask_type operator<(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()<b.to_storage()); }
    /// Return a mask whose lanes are true where `a > b` holds.
    simd_nodiscard friend simd_inline mask_type operator>(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return comparison(a.to_storage()>b.to_storage()); }
    /// Return a mask whose lanes are true where `a <= b` holds.
    simd_nodiscard friend simd_inline mask_type operator<=(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds.
    simd_nodiscard friend simd_inline mask_type operator>=(vec a,vec b) noexcept requires(!simd_mask_element<T>) { return (a>b)|(a==b); }
    /// Choose a in true mask lanes and b in false lanes; both values are already evaluated.
    template<class M> requires(std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline vec select(M mask,vec a,vec b) noexcept {
      using FM=typename storage_type::mask_type;
      return clean(select(FM::from_bitset(mask.to_bitset()),a.to_storage(),b.to_storage()));
    }
    /// Compute a*b+c with one fused rounding per logical lane in the caller's floating-point environment.
    simd_nodiscard friend simd_inline vec fma(vec a,vec b,vec c) noexcept requires std::same_as<T,float> { return clean(fma(a.to_storage(),b.to_storage(),c.to_storage())); }
    /// Compute the native square root of each logical lane in the caller's floating-point environment.
    simd_nodiscard friend simd_inline vec sqrt(vec a) noexcept requires std::same_as<T,float> { return clean(sqrt(a.to_storage())); }
    /// Round each logical lane to an integral value, ties to even, independently of the ambient rounding direction.
    simd_nodiscard friend simd_inline vec round_even(vec a) noexcept requires std::same_as<T,float> { return clean(round_even(a.to_storage())); }
    /// Construct normal powers of two; require each input to be an integral exponent in [-126,127].
    simd_nodiscard friend simd_inline vec normal_pow2(vec a) noexcept requires std::same_as<T,float> { return from_storage(normal_pow2(a.to_storage())); }
    /// Shift every lane left by K bits, discarding high bits. Require K smaller than the lane bit width.
    template<std::size_t K> requires(K<32)
    simd_nodiscard friend simd_inline vec operator<<(vec a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()<<imm<K>); }
    /// Shift every lane right by K; signed lanes extend the sign, unsigned lanes shift in zero. Require K smaller than the lane bit width.
    template<std::size_t K> requires(K<32)
    simd_nodiscard friend simd_inline vec operator>>(vec a,imm_t<K>) noexcept requires simd_integer_element<T> { return from_storage(a.to_storage()>>imm<K>); }
    /// Shift each lane left by compile-time K, discarding high bits; require K below the lane bit width.
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    simd_nodiscard simd_inline vec left() const noexcept { return *this<<imm<K>; }
    /// Shift each lane right by compile-time K; signed lanes extend their sign. Require K below the lane bit width.
    template<unsigned K> requires(K<32) && simd_integer_element<T>
    simd_nodiscard simd_inline vec right() const noexcept { return *this>>imm<K>; }
    // Match native integer registers: division and run-time shifts are absent.
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    friend vec operator<<(vec,vec) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    friend vec operator>>(vec,vec) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator/(vec,vec) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    friend vec operator%(vec,vec) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template<simd_integer_element U> friend vec operator<<(vec,U) requires simd_integer_element<T> = delete;
    /// Reject runtime shift counts; use a compile-time imm<K> within the lane width.
    template<simd_integer_element U> friend vec operator>>(vec,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend vec operator/(vec,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend vec operator/(U,vec) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend vec operator%(vec,U) requires simd_integer_element<T> = delete;
    /// Integer division and remainder are not provided; do not fall back to native-register conversions.
    template<simd_integer_element U> friend vec operator%(U,vec) requires simd_integer_element<T> = delete;
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept requires(!simd_mask_element<T>) { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept requires std::same_as<T,float> { return *this=*this/b; }
    /// Apply the corresponding lane-wise AND operation in place and return *this.
    simd_inline vec & operator&=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this&b; }
    /// Apply the corresponding lane-wise OR operation in place and return *this.
    simd_inline vec & operator|=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this|b; }
    /// Apply the corresponding lane-wise XOR operation in place and return *this.
    simd_inline vec & operator^=(vec b) noexcept requires(!std::same_as<T,float>) { return *this=*this^b; }
  private:
    struct unchecked {};
    simd_inline constexpr vec(unchecked,native_type x) noexcept : value(x) {}
    simd_nodiscard static simd_inline vec clean(storage_type x) noexcept { return vec(unchecked{},std::bit_cast<native_type>(x.to_native())); }
    template<class M> simd_nodiscard static simd_inline mask_type comparison(M x) noexcept {
      if constexpr(mask_type::compact) return mask_type::from_native(x.to_native());
      else return mask_type::from_storage(x);
    }
  };

  /// Choose each bit from a where the corresponding mask bit is one, otherwise from b; arbitrary bit masks are permitted.
  template<simd_integer_element T,std::size_t N,SIMD_ARCH_CONCEPT Arch>
    requires(N==2 || N==3) && (sizeof(T)==4)
  simd_nodiscard simd_inline vec<T,N,Arch> bit_select(vec<T,N,Arch> bits,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return (bits&a)|(~bits&b); }
  /// Add matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,SIMD_ARCH_CONCEPT Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_add(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a+b,prior); }
  /// Subtract matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,SIMD_ARCH_CONCEPT Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_sub(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a-b,prior); }
  /// Multiply matching integer lanes, retaining prior in inactive mask lanes; arithmetic wraps at the lane width.
  template<simd_integer_element T,std::size_t N,SIMD_ARCH_CONCEPT Arch,class M>
    requires(N==2 || N==3) && (sizeof(T)==4) &&
      (std::same_as<M,typename vec<T,N,Arch>::mask> || std::same_as<M,vec<mask32,N,Arch>>)
  simd_nodiscard simd_inline vec<T,N,Arch> masked_mul(M m,vec<T,N,Arch> prior,vec<T,N,Arch> a,vec<T,N,Arch> b) noexcept { return select(m,a*b,prior); }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
