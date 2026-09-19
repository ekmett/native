#define SIMD_ARCH_REQUIRES(A) (::simd::neon_fp16 <= A)
#pragma clang attribute push(__attribute__((target("neon,fullfp16"))), apply_to=function)
export namespace simd {
  namespace detail {
    template<::simd::isa A> requires SIMD_ARCH_REQUIRES(A)
    struct value_traits<vec<fp16,8,A>> {
      static constexpr isa value=neon_fp16;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
  }
  /// \ingroup vectors
  /// One 128-bit register of FP16 representations. Loads, stores and bit bridges
  /// preserve every encoding. Native addition, subtraction, multiplication, division,
  /// square root and fused multiply-add round directly to half precision under the caller's FPCR.
  /// FZ16 controls half subnormal inputs/results; rounding, DN and exception
  /// controls retain their architectural meaning. Operations may update FPSR.
  /// No operation changes FPCR. NaN payload/sign propagation is instruction- and
  /// FPCR-dependent, not a portable promise. Scalar fp16 conversions are unchanged.
  /// Only the 8-lane shape is provided.
  /// The application must admit that CPU/OS profile before entering compiled code.
  /// Every storage operation preserves subnormal, signed-zero and NaN encodings;
  /// none performs a floating-point conversion or quiets a signaling NaN.
  template<::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch) struct vec<fp16,8,Arch> {
    /// Scalar storage element; each lane retains all 16 representation bits.
    using value_type = fp16;
    /// The distinct compile-time NEON_FP16 instruction profile.
    static constexpr isa architecture=Arch;
    /// This one-register vector type, for generic register-based algorithms.
    using register_type = vec;
    /// Native 128-bit FP16 register representation; native bridges copy bits.
    using native_type = float16x8_t;
    /// Unsigned 16-bit lanes in the same profile and lane order.
    using bits_type = vec<std::uint16_t,8,architecture>;
    /// Full-register predicate with zero or all-one bits in each 16-bit lane.
    using mask = vec<mask16,8,architecture>;
    /// Generic mask spelling for the full-register lane mask.
    using mask_type = mask;
    /// Explicit predicate spelling for the same full-register mask type.
    using predicate_type = mask;
    /// Full-register mask shape with zero or all-one 16-bit lanes.
    using vector_mask_type = vec<mask16,8,architecture>;
    /// Replace the element type while retaining 8 lanes and this profile.
    /// Unsupported resulting shapes remain incomplete.
    template<class T> using rebind = vec<T,8,architecture>;
    /// Number of logical FP16 lanes, with no padding lanes.
    static constexpr std::size_t lanes = 8;
  private:
    native_type value_;
  public:
    /// Default initialization leaves storage unspecified; braces zero it.
    vec() noexcept = default;
    /// Broadcast the exact representation of value to all 8 lanes.
    simd_inline explicit vec(fp16 value) noexcept
      : value_(std::bit_cast<native_type>(bits_type(value.to_bits()).to_native())) {}
    /// Copy array element i into lane i without conversion or representation changes.
    simd_inline explicit vec(std::array<fp16,lanes> const & values) noexcept : vec(load(values.data())) {}
    /// Construct all 8 lanes from FP16 values in argument order, preserving their bits.
    template<class... T> requires(sizeof...(T) == lanes && (std::same_as<T,fp16> && ...))
    simd_inline vec(T... values) noexcept : vec(std::array<fp16,lanes>{values...}) {}
    /// Adopt a native register without conversion or representation changes.
    simd_inline vec(native_type value) noexcept : value_(value) {}
    /// Project the native register for direct intrinsic interoperability.
    simd_nodiscard simd_inline operator native_type() const noexcept { return value_; }
    /// Return all lane bits as a native register, without conversion or lane reordering.
    simd_nodiscard simd_inline native_type to_native() const noexcept { return value_; }
    /// Copy a native FP16 register into this vector, preserving every representation bit.
    simd_nodiscard static simd_inline vec from_native(native_type value) noexcept {
      vec result; result.value_ = value; return result;
    }
    /// Return the 16-bit representation of each lane in an unsigned vector.
    simd_nodiscard simd_inline bits_type bits() const noexcept {
      return bits_type::from_native(std::bit_cast<typename bits_type::native_type>(value_));
    }
    /// Synonym for bits(); this is a representation bridge, not a numeric conversion.
    simd_nodiscard simd_inline bits_type to_bits() const noexcept { return bits(); }
    /// Interpret each unsigned lane as a FP16 representation without changing its bits.
    simd_nodiscard static simd_inline vec from_bits(bits_type value) noexcept {
      return from_native(std::bit_cast<native_type>(value.to_native()));
    }
    /// Read exactly 8 accessible uint16_t objects into corresponding FP16 lane bits.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    simd_nodiscard static simd_inline vec load_bits(std::uint16_t const * p) noexcept {
      return from_bits(bits_type::load(p));
    }
    /// Write every lane representation to 8 accessible uint16_t objects in lane order.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    simd_inline void store_bits(std::uint16_t * p) const noexcept { bits().store(p); }
    /// Read exactly 8 accessible FP16 objects, preserving every encoding.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for FP16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    simd_nodiscard static simd_inline vec load_memory(fp16 const * p) noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      native_type value; std::memcpy(&value, p, sizeof(value)); return from_native(value);
    }
    /// Write all lane representations to exactly 8 accessible FP16 objects.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for FP16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    simd_inline void store_memory(fp16 * p) const noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      std::memcpy(p, &value_, sizeof(value_));
    }
    /// Load 8 FP16 objects with the default alignment contract of load_memory().
    simd_nodiscard static simd_inline vec load(fp16 const * p) noexcept { return load_memory(p); }
    /// Store 8 FP16 objects with the default alignment contract of store_memory().
    simd_inline void store(fp16 * p) const noexcept { store_memory(p); }
    /// Synonym for load(); no register-width alignment is required.
    simd_nodiscard static simd_inline vec loadu(fp16 const * p) noexcept { return load(p); }
    /// Synonym for store(); no register-width alignment is required.
    simd_inline void storeu(fp16 * p) const noexcept { store(p); }
    /// Read exactly the first n accessible FP16 objects, where n <= 8.
    /// Copy their representations to lanes [0,n); remaining lanes receive fill's
    /// exact representation. The default fill is positive zero. No access occurs
    /// for n == 0, when p may be null; otherwise p must address n FP16 objects.
    simd_nodiscard static simd_inline vec load_partial(fp16 const * p, std::size_t n,
        fp16 fill = fp16::from_bits(0)) noexcept {
      assert(n <= lanes);
      std::array<fp16,lanes> values; values.fill(fill);
      if (n) std::memcpy(values.data(), p, n * sizeof(fp16));
      return load(values.data());
    }
    /// Write the representations of lanes [0,n) to exactly n accessible FP16
    /// objects, where n <= 8. Memory outside that prefix is untouched. No access
    /// occurs for n == 0, when p may be null; otherwise p must address n objects.
    simd_inline void store_partial(fp16 * p, std::size_t n) const noexcept {
      assert(n <= lanes);
      if (n) std::memcpy(p, &value_, n * sizeof(fp16));
    }
    /// Add corresponding half lanes, rounding directly under the caller's FPCR.
    simd_nodiscard friend simd_inline vec operator+(vec a,vec b) noexcept {
      return from_native(detail::neon_fp16_backend::add_half(a.value_,b.value_));
    }
    /// Subtract corresponding half lanes, rounding directly under the caller's FPCR.
    simd_nodiscard friend simd_inline vec operator-(vec a,vec b) noexcept {
      return from_native(detail::neon_fp16_backend::sub_half(a.value_,b.value_));
    }
    /// Multiply corresponding half lanes, rounding directly under the caller's FPCR.
    simd_nodiscard friend simd_inline vec operator*(vec a,vec b) noexcept {
      return from_native(detail::neon_fp16_backend::mul_half(a.value_,b.value_));
    }
    /// Divide corresponding half lanes with native half-precision rounding.
    /// The caller's FPCR and native exception behavior apply.
    simd_nodiscard friend simd_inline vec operator/(vec a,vec b) noexcept {
      return from_native(detail::neon_fp16_backend::div_half(a.value_,b.value_));
    }
    /// Compute each half lane's square root with native half-precision rounding.
    /// Signed zero is preserved; negative nonzero operands after native input
    /// flushing produce a quiet NaN.
    /// The caller's FPCR and native exception behavior apply.
    simd_nodiscard friend simd_inline vec sqrt(vec a) noexcept {
      return from_native(detail::neon_fp16_backend::sqrt_half(a.value_));
    }
    /// Apply native FNEG to each half lane; no scalar half-to-float conversion occurs.
    simd_nodiscard friend simd_inline vec operator-(vec a) noexcept {
      return from_native(detail::neon_fp16_backend::neg_half(a.value_));
    }
    /// Ordered lane equality. NaNs compare false; signed zeros compare equal.
    /// FPCR half-denormal controls and native comparison exception behavior apply.
    simd_nodiscard friend simd_inline mask operator==(vec a,vec b) noexcept {
      return mask::unsafe_from_native(detail::neon_fp16_backend::eq_half(a.value_,b.value_));
    }
    /// Lane inequality, true for unordered NaN operands; complements native equality.
    simd_nodiscard friend simd_inline mask operator!=(vec a,vec b) noexcept { return ~(a==b); }
    /// Ordered lane less-than. NaNs compare false; native FPCR/FPSR semantics apply.
    simd_nodiscard friend simd_inline mask operator<(vec a,vec b) noexcept {
      return mask::unsafe_from_native(detail::neon_fp16_backend::lt_half(a.value_,b.value_));
    }
    /// Ordered lane less-or-equal. NaNs compare false; native FPCR/FPSR semantics apply.
    simd_nodiscard friend simd_inline mask operator<=(vec a,vec b) noexcept {
      return mask::unsafe_from_native(detail::neon_fp16_backend::le_half(a.value_,b.value_));
    }
    /// Ordered lane greater-than, with the native less-than operands reversed.
    simd_nodiscard friend simd_inline mask operator>(vec a,vec b) noexcept { return b<a; }
    /// Ordered lane greater-or-equal, with native less-or-equal operands reversed.
    simd_nodiscard friend simd_inline mask operator>=(vec a,vec b) noexcept { return b<=a; }
    /// Choose a lane from a when its canonical mask lane is true, otherwise b.
    /// Selection copies every representation bit without arithmetic or NaN quieting.
    simd_nodiscard friend simd_inline vec select(mask m,vec a,vec b) noexcept {
      return from_native(detail::neon_fp16_backend::select_half(m.to_native(),a.value_,b.value_));
    }
  };


  /// Compute a*b+c in each lane with one final half-precision rounding (FMLA).
  /// The caller's FPCR rounding/FZ16/DN/exception controls apply, FPSR may change,
  /// and FPCR is preserved. NaN payload/sign follow the native instruction.
  template<::simd::isa Arch> requires SIMD_ARCH_REQUIRES(Arch)
  simd_nodiscard simd_inline vec<fp16,8,Arch> fma(
      vec<fp16,8,Arch> a,vec<fp16,8,Arch> b,vec<fp16,8,Arch> c) noexcept {
    return vec<fp16,8,Arch>::from_native(
      detail::neon_fp16_backend::fma_half(a.to_native(),b.to_native(),c.to_native()));
  }
}

#pragma clang attribute pop
#undef SIMD_ARCH_REQUIRES
