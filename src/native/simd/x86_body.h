
namespace native {
  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct native_empty_bases simd<float, 4,Arch> : detail::register_memory<simd<float,4,Arch>, 4>, detail::swizzle_access<float,4,Arch> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,4,Arch>;
    using vector_mask_type=simd<mask32,4,Arch>;
    using mask_type=std::conditional_t<bool(NATIVE_HAS_AVX512VL),predicate<4,Arch>,simd<mask32,4,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<4,Arch>;
    __m128 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast x to all lanes.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=_mm_set1_ps(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline native_target("sse") constexpr simd(__m128 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    native_inline constexpr void store(native_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator+(simd a, simd b) { return simd(_mm_add_ps(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator-(simd a, simd b) { return simd(_mm_sub_ps(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator*(simd a, simd b) { return simd(_mm_mul_ps(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator/(simd a, simd b) { return simd(_mm_div_ps(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline native_const simd operator-(simd a) { return simd(_mm_xor_ps(a.value, _mm_set1_ps(-0.f))); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator<(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator>(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator==(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm_castps_si128(_mm_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline native_const simd select(M m,simd a,simd b) {
      if constexpr(M::compact) return simd(_mm_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return simd(_mm_blendv_ps(b.value,a.value,_mm_castsi128_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline native_pure simd fma(simd a, simd b, simd c) { return simd(_mm_fmadd_ps(a.value, b.value, c.value)); }
    /// Compute the native square root in every lane.
    native_artificial native_nodiscard friend native_inline native_pure simd sqrt(simd a) { return simd(_mm_sqrt_ps(a.value)); }
    /// Round to an integral value, ties to even, independent of ambient direction.
    native_artificial native_nodiscard friend native_inline native_const simd round_even(simd a) { return simd(_mm_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline native_const simd normal_pow2(simd n) { return simd(_mm_castsi128_ps(_mm_slli_epi32(_mm_add_epi32(_mm_cvttps_epi32(n.value), _mm_set1_epi32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      if consteval {
        std::array<float,sizeof(native_type)/sizeof(float)> values{};
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) values[i]=p[i];
        return from_native(__builtin_bit_cast(native_type,values));
      }
      if constexpr(Alignment>=16) return simd(_mm_load_ps(p));
      else return simd(_mm_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    native_inline constexpr void store_memory(float * p) const noexcept {
      if consteval {
        auto values=__builtin_bit_cast(std::array<float,sizeof(native_type)/sizeof(float)>,value);
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) p[i]=values[i];
        return;
      }
      if constexpr(Alignment>=16) _mm_store_ps(p,value);
      else _mm_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = __m128;
    using bits_type = simd<uint32_t,4,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("sse") operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("sse") native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(_mm_castps_si128(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(_mm_castsi128_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native register storage without changing its bits.
    native_nodiscard static native_inline constexpr native_const native_target("sse") simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    native_nodiscard static native_inline constexpr native_const native_target("sse") simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Synonym for an unaligned full-vector load.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    native_inline constexpr void storeu(native_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    native_inline constexpr simd(std::array<float,4> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==4) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,4>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };

  /// \ingroup vectors
  /// Raw x86 float storage; the Arch argument fixes comparison-mask representation.
  template <::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct simd<float, 8,Arch> : detail::register_memory<simd<float,8,Arch>, 8> {
    static constexpr isa<> architecture=Arch;
    template <class T> using rebind = simd<T,8,Arch>;
    using vector_mask_type=simd<mask32,8,Arch>;
    using mask_type=std::conditional_t<bool(NATIVE_HAS_AVX512VL),predicate<8,Arch>,simd<mask32,8,Arch>>;
    using mask = mask_type;
    using predicate_type = predicate<8,Arch>;
    __m256 value;
    /// Default initialization leaves storage unspecified; value initialization with braces zero-initializes it.
    native_inline simd() = default;
    /// Copy the stored value without arithmetic or normalization.
    native_inline constexpr simd(simd const &) = default;
    /// Copy the stored value and return *this; no numerical conversion is performed.
    native_reinitializes native_inline constexpr simd & operator=(simd const &) = default;
    /// Broadcast x to all lanes.
    native_inline constexpr simd(float x) {
      if consteval { std::array<float,sizeof(native_type)/sizeof(float)> values{}; values.fill(x); value=__builtin_bit_cast(native_type,values); }
      else { value=_mm256_set1_ps(x); }
    }
    /// Adopt native lane storage without numerical conversion.
    native_inline native_target("avx") constexpr simd(__m256 x) : value(x) {}
    /// Load every logical lane; no extra alignment is required.
    native_nodiscard static native_inline constexpr native_pure simd load(native_noescape float const * p) { return load_memory<1>(p); }
    /// Store every logical lane; no extra alignment is required.
    native_inline constexpr void store(native_noescape float * p) const { store_memory<1>(p); }
    /// Add corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator+(simd a, simd b) { return simd(_mm256_add_ps(a.value, b.value)); }
    /// Subtract corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator-(simd a, simd b) { return simd(_mm256_sub_ps(a.value, b.value)); }
    /// Multiply corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator*(simd a, simd b) { return simd(_mm256_mul_ps(a.value, b.value)); }
    /// Divide corresponding floating-point lanes using the caller's rounding and denormal environment.
    native_artificial native_nodiscard friend native_inline native_pure simd operator/(simd a, simd b) { return simd(_mm256_div_ps(a.value, b.value)); }
    /// Negate every logical lane; floating-point lanes change sign.
    native_nodiscard friend native_inline native_const simd operator-(simd a) { return simd(_mm256_xor_ps(a.value, _mm256_set1_ps(-0.f))); }
    /// Return a mask whose lanes are true where `a < b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator<(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_LT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_LT_OQ)));
    }
    /// Return a mask whose lanes are true where `a > b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator>(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_GT_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_GT_OQ)));
    }
    /// Return a mask whose lanes are true where `a == b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator==(simd a, simd b) {
      if constexpr(bool(NATIVE_HAS_AVX512VL))
        return mask_type::from_native(_mm256_cmp_ps_mask(a.value,b.value,_CMP_EQ_OQ));
      else
        return mask_type::unsafe_from_native(_mm256_castps_si256(_mm256_cmp_ps(a.value,b.value,_CMP_EQ_OQ)));
    }
    /// Choose a where the canonical mask is true, otherwise b; both operands are evaluated.
    template<class M> requires (std::same_as<M,mask_type> || std::same_as<M,vector_mask_type>)
    native_nodiscard friend native_inline native_const simd select(M m,simd a,simd b) {
      if constexpr(M::compact) return simd(_mm256_mask_blend_ps(m.to_native(),b.value,a.value));
      else
        return simd(_mm256_blendv_ps(b.value,a.value,_mm256_castsi256_ps(m.to_native()))); }
    /// Compute a*b+c with one fused rounding per lane.
    native_artificial native_nodiscard friend native_inline native_pure simd fma(simd a, simd b, simd c) { return simd(_mm256_fmadd_ps(a.value, b.value, c.value)); }
    /// Compute the native square root in every lane.
    native_artificial native_nodiscard friend native_inline native_pure simd sqrt(simd a) { return simd(_mm256_sqrt_ps(a.value)); }
    /// Round to an integral value, ties to even, independent of ambient direction.
    native_artificial native_nodiscard friend native_inline native_const simd round_even(simd a) { return simd(_mm256_round_ps(a.value, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC)); }
    /// Construct normal powers of two; require integral exponents in [-126,127].
    native_nodiscard friend native_inline native_const simd normal_pow2(simd n) { return simd(_mm256_castsi256_ps(_mm256_slli_epi32(_mm256_add_epi32(_mm256_cvttps_epi32(n.value), _mm256_set1_epi32(127)), 23))); }

    template <std::size_t Alignment = 1>
    /// Load full lanes, assuming Alignment-byte pointer alignment.
    native_nodiscard static native_inline constexpr native_pure simd load_memory(float const * p) noexcept {
      if consteval {
        std::array<float,sizeof(native_type)/sizeof(float)> values{};
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) values[i]=p[i];
        return from_native(__builtin_bit_cast(native_type,values));
      }
      if constexpr(Alignment>=32) return simd(_mm256_load_ps(p));
      else return simd(_mm256_loadu_ps(p));
    }
    template <std::size_t Alignment = 1>
    /// Store full lanes, assuming Alignment-byte pointer alignment.
    native_inline constexpr void store_memory(float * p) const noexcept {
      if consteval {
        auto values=__builtin_bit_cast(std::array<float,sizeof(native_type)/sizeof(float)>,value);
        for (std::size_t i=0;i<sizeof(native_type)/sizeof(float);++i) p[i]=values[i];
        return;
      }
      if constexpr(Alignment>=32) _mm256_store_ps(p,value);
      else _mm256_storeu_ps(p,value);
    }
    using value_type = float;
    using register_type = simd;
    using native_type = __m256;
    using bits_type = simd<uint32_t,8,Arch>;
    /// Return the native storage value without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("avx") operator native_type() const noexcept { return value; }
    /// Project native register storage without a numerical conversion.
    native_nodiscard native_inline constexpr native_pure native_target("avx") native_type to_native() const noexcept { return value; }
    /// Project exact binary32 lane words into the unsigned vector.
    native_artificial native_nodiscard native_inline constexpr native_pure bits_type bits() const noexcept { if consteval { return bits_type::from_native(__builtin_bit_cast(typename bits_type::native_type,value)); } return bits_type::from_native(_mm256_castps_si256(value)); }
    /// Synonym for bits(): preserve all binary32 representation bits.
    native_nodiscard native_inline constexpr native_pure bits_type to_bits() const noexcept { return bits(); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_artificial native_nodiscard static native_inline constexpr native_const simd from_bits(bits_type bits) noexcept { if consteval { return simd(__builtin_bit_cast(native_type,bits.to_native())); } return simd(_mm256_castsi256_ps(bits.value)); }
    /// Reinterpret unsigned lane words as binary32, without normalization.
    native_nodiscard static native_inline constexpr native_const simd from_bits(std::uint32_t bits) noexcept { return from_bits(bits_type(bits)); }
    /// Broadcast one binary32 value to every lane.
    native_nodiscard static native_inline constexpr native_const simd from_float(float x) noexcept { return simd(x); }
    /// Adopt native register storage without changing its bits.
    native_nodiscard static native_inline constexpr native_const native_target("avx") simd from_native(native_type x) noexcept { return simd(x); }
    /// Adopt native raw float storage; this raw type adds no normalization.
    native_nodiscard static native_inline constexpr native_const native_target("avx") simd unsafe_from_float32(native_type x) noexcept { return simd(x); }
    /// Synonym for an unaligned full-vector load.
    native_nodiscard static native_inline constexpr native_pure simd loadu(native_noescape float const * p) { return load_memory<1>(p); }
    /// Synonym for an unaligned full-vector store.
    native_inline constexpr void storeu(native_noescape float * p) const { store_memory<1>(p); }
    /// Load exact binary32 representations from uint32_t words.
    native_nodiscard static native_inline constexpr native_pure simd load_bits(native_noescape std::uint32_t const * p) noexcept { return from_bits(bits_type::load(p)); }
    /// Store exact binary32 representations as uint32_t words.
    native_inline constexpr void store_bits(native_noescape std::uint32_t * p) const noexcept { bits().store(p); }
    /// Load n words and fill the remaining lanes; require n <= lanes.
    native_nodiscard static native_inline constexpr native_pure simd load_bits_partial(native_noescape std::uint32_t const * p,std::size_t n,std::uint32_t fill=0) noexcept { return from_bits(bits_type::load_partial(p,n,fill)); }
    /// Store exactly n representation words; require n <= lanes.
    native_inline constexpr void store_bits_partial(native_noescape std::uint32_t * p,std::size_t n) const noexcept { bits().store_partial(p,n); }
    /// Load one lane from each array element, in array order.
    native_inline constexpr simd(std::array<float,8> const & values) noexcept : simd(loadu(values.data())) {}
#if defined(__clang__)
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : value{static_cast<float>(x)...} {}
#else
    /// Convert one argument per lane; exceptions follow those named-lvalue conversions.
    template <class... X> requires (sizeof...(X)==8) && (std::convertible_to<X,float> && ...)
    native_inline constexpr simd(X... x) noexcept((noexcept(static_cast<float>(x)) && ...)) : simd(loadu(std::array<float,8>{static_cast<float>(x)...}.data())) {}
#endif
    /// Apply the corresponding lane-wise add operation in place and return *this.
    native_inline simd & operator+=(simd b) noexcept { return *this=*this+b; }
    /// Apply the corresponding lane-wise subtract operation in place and return *this.
    native_inline simd & operator-=(simd b) noexcept { return *this=*this-b; }
    /// Apply the corresponding lane-wise multiply operation in place and return *this.
    native_inline simd & operator*=(simd b) noexcept { return *this=*this*b; }
    /// Apply the corresponding lane-wise divide operation in place and return *this.
    native_inline simd & operator/=(simd b) noexcept { return *this=*this/b; }
    /// Return a mask whose lanes are true where `a != b` holds. NaN lanes compare unequal.
    native_nodiscard friend native_inline native_const mask_type operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Return a mask whose lanes are true where `a <= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator<=(simd a,simd b) noexcept { return (a<b)|(a==b); }
    /// Return a mask whose lanes are true where `a >= b` holds. NaN lanes yield false.
    native_nodiscard friend native_inline native_const mask_type operator>=(simd a,simd b) noexcept { return (a>b)|(a==b); }
  };

}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
