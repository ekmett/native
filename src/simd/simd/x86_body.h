
namespace simd {
  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch) struct simd_empty_bases vec<float, 4,Arch> : detail::register_memory<vec<float,4,Arch>, 4>, detail::swizzle_access<float,4,Arch> {
    static constexpr isa architecture=Arch;
    template <class T> using rebind = vec<T,4,Arch>;
    using vector_mask_type=vec<mask32,4,Arch>;
    using mask_type=std::conditional_t<bool(SIMD_HAS_AVX512VL),predicate<4,Arch>,vec<mask32,4,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<4,Arch>;
    __m128 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    simd_inline vec() = default;
    /// Copy the stored value without arithmetic or normalization.
    simd_inline constexpr vec(vec const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    simd_reinitializes simd_inline constexpr vec & operator=(vec const &) = default;
    /// Broadcast x to all lanes.
    simd_inline vec(float x) : value(_mm_set1_ps(x)) {}
    /// Adopt native lane storage without numerical conversion.
    simd_inline constexpr vec(__m128 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    simd_nodiscard static simd_inline simd_pure vec load(simd_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    simd_inline void store(simd_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator+(vec a, vec b) { return vec(_mm_add_ps(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator-(vec a, vec b) { return vec(_mm_sub_ps(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator*(vec a, vec b) { return vec(_mm_mul_ps(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator/(vec a, vec b) { return vec(_mm_div_ps(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a) { return vec(_mm_xor_ps(a.value, _mm_set1_ps(-0.f))); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const vec select(M m,vec a,vec b) {
      if constexpr(M::compact) return vec(_mm_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return vec(_mm_blendv_ps(b.value,a.value,_mm_castsi128_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec fma(vec a, vec b, vec c) { return vec(_mm_fmadd_ps(a.value, b.value, c.value)); }
    /// Compute the native square root in every lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec sqrt(vec a) { return vec(_mm_sqrt_ps(a.value)); }
    /// Round to an integral value, ties to even, independent of ambient direction.
    simd_artificial simd_nodiscard friend simd_inline simd_const vec round_even(vec a) { return vec(_mm_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    simd_nodiscard friend simd_inline simd_const vec normal_pow2(vec n) { return vec(_mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(n.value), _mm_set1_epi32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    simd_nodiscard static simd_inline simd_pure vec load_memory(float const * p) noexcept {
      if constexpr(Alignment>=16) return vec(_mm_load_ps(p));
      else return vec(_mm_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    simd_inline void store_memory(float * p) const noexcept {
      if constexpr(Alignment>=16) _mm_store_ps(p,value);
      else _mm_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = vec;
    using native_type = __m128;
    using bits_type = vec<uint32_t,4,Arch>;
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_pure operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    simd_nodiscard simd_inline simd_pure native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    simd_artificial simd_nodiscard simd_inline simd_pure bits_type bits() const noexcept { return bits_type::from_native(_mm_castps_si128(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    simd_nodiscard simd_inline simd_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    simd_artificial simd_nodiscard static simd_inline simd_const vec from_bits(bits_type bits) noexcept { return vec(_mm_castsi128_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    simd_nodiscard static simd_inline simd_const vec from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    simd_nodiscard static simd_inline simd_const vec from_float(float x) noexcept { return vec(x); }
    /// Adopt native register storage without changing its bits.
    simd_nodiscard static simd_inline simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    simd_nodiscard static simd_inline simd_const vec unsafe_from_float32(native_type x) noexcept { return vec(x); }
    /// Synonym for an unaligned full-vector load.
    simd_nodiscard static simd_inline simd_pure vec loadu(simd_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    simd_inline void storeu(simd_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    simd_nodiscard static simd_inline simd_pure vec load_bits(simd_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    simd_inline void store_bits(simd_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    simd_nodiscard static simd_inline simd_pure vec load_bits_partial(simd_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    simd_inline void store_bits_partial(simd_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    simd_inline vec(std::array<float,4> const & values) noexcept : vec(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    simd_inline vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : vec(loadu(std::array<float,4>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a,vec b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a,vec b) noexcept { return (a>b)|(a==b); }
  };

  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch) struct vec<float, 8,Arch> : detail::register_memory<vec<float,8,Arch>, 8> {
    static constexpr isa architecture=Arch;
    template <class T> using rebind = vec<T,8,Arch>;
    using vector_mask_type=vec<mask32,8,Arch>;
    using mask_type=std::conditional_t<bool(SIMD_HAS_AVX512VL),predicate<8,Arch>,vec<mask32,8,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<8,Arch>;
    __m256 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    simd_inline vec() = default;
    /// Copy the stored value without arithmetic or normalization.
    simd_inline constexpr vec(vec const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    simd_reinitializes simd_inline constexpr vec & operator=(vec const &) = default;
    /// Broadcast x to all lanes.
    simd_inline vec(float x) : value(_mm256_set1_ps(x)) {}
    /// Adopt native lane storage without numerical conversion.
    simd_inline constexpr vec(__m256 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    simd_nodiscard static simd_inline simd_pure vec load(simd_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    simd_inline void store(simd_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator+(vec a, vec b) { return vec(_mm256_add_ps(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator-(vec a, vec b) { return vec(_mm256_sub_ps(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator*(vec a, vec b) { return vec(_mm256_mul_ps(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec operator/(vec a, vec b) { return vec(_mm256_div_ps(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    simd_nodiscard friend simd_inline simd_const vec operator-(vec a) { return vec(_mm256_xor_ps(a.value, _mm256_set1_ps(-0.f))); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator==(vec a, vec b) {
      if constexpr(bool(SIMD_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    simd_nodiscard friend simd_inline simd_const vec select(M m,vec a,vec b) {
      if constexpr(M::compact) return vec(_mm256_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return vec(_mm256_blendv_ps(b.value,a.value,_mm256_castsi256_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec fma(vec a, vec b, vec c) { return vec(_mm256_fmadd_ps(a.value, b.value, c.value)); }
    /// Compute the native square root in every lane.
    simd_artificial simd_nodiscard friend simd_inline simd_pure vec sqrt(vec a) { return vec(_mm256_sqrt_ps(a.value)); }
    /// Round to an integral value, ties to even, independent of ambient direction.
    simd_artificial simd_nodiscard friend simd_inline simd_const vec round_even(vec a) { return vec(_mm256_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    simd_nodiscard friend simd_inline simd_const vec normal_pow2(vec n) { return vec(_mm256_castsi256_ps(_mm256_slli_epi32(_mm256_add_epi32(_mm256_cvttps_epi32(n.value), _mm256_set1_epi32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    simd_nodiscard static simd_inline simd_pure vec load_memory(float const * p) noexcept {
      if constexpr(Alignment>=32) return vec(_mm256_load_ps(p));
      else return vec(_mm256_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    simd_inline void store_memory(float * p) const noexcept {
      if constexpr(Alignment>=32) _mm256_store_ps(p,value);
      else _mm256_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = vec;
    using native_type = __m256;
    using bits_type = vec<uint32_t,8,Arch>;
    /// Return the native storage value without a numerical conversion.
    simd_nodiscard simd_inline simd_pure operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    simd_nodiscard simd_inline simd_pure native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    simd_artificial simd_nodiscard simd_inline simd_pure bits_type bits() const noexcept { return bits_type::from_native(_mm256_castps_si256(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    simd_nodiscard simd_inline simd_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    simd_artificial simd_nodiscard static simd_inline simd_const vec from_bits(bits_type bits) noexcept { return vec(_mm256_castsi256_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    simd_nodiscard static simd_inline simd_const vec from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    simd_nodiscard static simd_inline simd_const vec from_float(float x) noexcept { return vec(x); }
    /// Adopt native register storage without changing its bits.
    simd_nodiscard static simd_inline simd_const vec from_native(native_type x) noexcept { return vec(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    simd_nodiscard static simd_inline simd_const vec unsafe_from_float32(native_type x) noexcept { return vec(x); }
    /// Synonym for an unaligned full-vector load.
    simd_nodiscard static simd_inline simd_pure vec loadu(simd_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    simd_inline void storeu(simd_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    simd_nodiscard static simd_inline simd_pure vec load_bits(simd_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    simd_inline void store_bits(simd_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    simd_nodiscard static simd_inline simd_pure vec load_bits_partial(simd_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    simd_inline void store_bits_partial(simd_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    simd_inline vec(std::array<float,8> const & values) noexcept : vec(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    simd_inline constexpr vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    simd_inline vec(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : vec(loadu(std::array<float,8>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    simd_inline vec & operator+=(vec b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    simd_inline vec & operator-=(vec b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    simd_inline vec & operator*=(vec b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    simd_inline vec & operator/=(vec b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    simd_nodiscard friend simd_inline simd_const mask_type operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator<=(vec a,vec b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    simd_nodiscard friend simd_inline simd_const mask_type operator>=(vec a,vec b) noexcept { return (a>b)|(a==b); }
  };

}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
