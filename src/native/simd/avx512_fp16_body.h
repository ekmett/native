#define NATIVE_ARCH_REQUIRES(A) (::native::avx512_fp16 <= A)
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"))), apply_to=function)
export namespace native {
  namespace detail {
    template<::native::isa A> requires NATIVE_ARCH_REQUIRES(A)
    struct value_traits<simd<fp16,32,A>> {
      static constexpr isa value=avx512_fp16;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
  }
  /// \ingroup vectors
  /// One 512-bit register of FP16 representations. Loads, stores and bit bridges
  /// preserve every encoding. Native addition, subtraction, multiplication, division,
  /// square root and fused multiply-add round directly to half precision under the caller's MXCSR rounding control.
  /// Half operands/results use gradual underflow regardless of MXCSR.DAZ/FTZ.
  /// MXCSR rounding and exception controls apply; status flags may change.
  /// No operation changes MXCSR control bits. NaN payload/sign propagation follows
  /// the instruction, not a portable promise. Scalar fp16 conversions are unchanged.
  /// Only the 32-lane shape is provided.
  /// The application must admit that CPU/OS profile before entering compiled code.
  /// Every storage operation preserves subnormal, signed-zero and NaN encodings;
  /// none performs a floating-point conversion or quiets a signaling NaN.
  template<::native::isa Arch> requires NATIVE_ARCH_REQUIRES(Arch) struct simd<fp16,32,Arch> {
    /// Scalar storage element; each lane retains all 16 representation bits.
    using value_type = fp16;
    /// The distinct compile-time AVX512_FP16 instruction profile.
    static constexpr isa architecture=Arch;
    /// This one-register vector type, for generic register-based algorithms.
    using register_type = simd;
    /// Native 512-bit FP16 register representation; native bridges copy bits.
    using native_type = __m512h;
    /// Unsigned 16-bit lanes in the same profile and lane order.
    using bits_type = simd<std::uint16_t,32,architecture>;
    /// Compact predicate with bit i selecting FP16 lane i.
    using mask = predicate<32,architecture>;
    /// Generic mask spelling for the compact lane predicate.
    using mask_type = mask;
    /// Explicit predicate spelling for the same compact predicate type.
    using predicate_type = mask;
    /// Full-register mask shape with zero or all-one 16-bit lanes.
    using vector_mask_type = simd<mask16,32,architecture>;
    /// Replace the element type while retaining 32 lanes and this profile.
    /// Unsupported resulting shapes remain incomplete.
    template<class T> using rebind = simd<T,32,architecture>;
    /// Number of logical FP16 lanes, with no padding lanes.
    static constexpr std::size_t lanes = 32;
  private:
    native_type value_;
  public:
    /// Default initialization leaves storage unspecified; braces zero it.
    simd() noexcept = default;
    /// Broadcast the exact representation of value to all 32 lanes.
    native_inline explicit simd(fp16 value) noexcept
      : value_(std::bit_cast<native_type>(bits_type(value.to_bits()).to_native())) {}
    /// Copy array element i into lane i without conversion or representation changes.
    native_inline explicit simd(std::array<fp16,lanes> const & values) noexcept : simd(load(values.data())) {}
    /// Construct all 32 lanes from FP16 values in argument order, preserving their bits.
    template<class... T> requires(sizeof...(T) == lanes && (std::same_as<T,fp16> && ...))
    native_inline simd(T... values) noexcept : simd(std::array<fp16,lanes>{values...}) {}
    /// Adopt a native register without conversion or representation changes.
    native_inline simd(native_type value) noexcept : value_(value) {}
    /// Project the native register for direct intrinsic interoperability.
    native_nodiscard native_inline operator native_type() const noexcept { return value_; }
    /// Return all lane bits as a native register, without conversion or lane reordering.
    native_nodiscard native_inline native_type to_native() const noexcept { return value_; }
    /// Copy a native FP16 register into this vector, preserving every representation bit.
    native_nodiscard static native_inline simd from_native(native_type value) noexcept {
      simd result; result.value_ = value; return result;
    }
    /// Return the 16-bit representation of each lane in an unsigned vector.
    native_nodiscard native_inline bits_type bits() const noexcept {
      return bits_type::from_native(std::bit_cast<typename bits_type::native_type>(value_));
    }
    /// Synonym for bits(); this is a representation bridge, not a numeric conversion.
    native_nodiscard native_inline bits_type to_bits() const noexcept { return bits(); }
    /// Interpret each unsigned lane as a FP16 representation without changing its bits.
    native_nodiscard static native_inline simd from_bits(bits_type value) noexcept {
      return from_native(std::bit_cast<native_type>(value.to_native()));
    }
    /// Read exactly 32 accessible uint16_t objects into corresponding FP16 lane bits.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    native_nodiscard static native_inline simd load_bits(std::uint16_t const * p) noexcept {
      return from_bits(bits_type::load(p));
    }
    /// Write every lane representation to 32 accessible uint16_t objects in lane order.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    native_inline void store_bits(std::uint16_t * p) const noexcept { bits().store(p); }
    /// Read exactly 32 accessible FP16 objects, preserving every encoding.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for FP16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    native_nodiscard static native_inline simd load_memory(fp16 const * p) noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      native_type value; std::memcpy(&value, p, sizeof(value)); return from_native(value);
    }
    /// Write all lane representations to exactly 32 accessible FP16 objects.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for FP16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    native_inline void store_memory(fp16 * p) const noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      std::memcpy(p, &value_, sizeof(value_));
    }
    /// Load 32 FP16 objects with the default alignment contract of load_memory().
    native_nodiscard static native_inline simd load(fp16 const * p) noexcept { return load_memory(p); }
    /// Store 32 FP16 objects with the default alignment contract of store_memory().
    native_inline void store(fp16 * p) const noexcept { store_memory(p); }
    /// Synonym for load(); no register-width alignment is required.
    native_nodiscard static native_inline simd loadu(fp16 const * p) noexcept { return load(p); }
    /// Synonym for store(); no register-width alignment is required.
    native_inline void storeu(fp16 * p) const noexcept { store(p); }
    /// Read exactly the first n accessible FP16 objects, where n <= 32.
    /// Copy their representations to lanes [0,n); remaining lanes receive fill's
    /// exact representation. The default fill is positive zero. No access occurs
    /// for n == 0, when p may be null; otherwise p must address n FP16 objects.
    native_nodiscard static native_inline simd load_partial(fp16 const * p, std::size_t n,
        fp16 fill = fp16::from_bits(0)) noexcept {
      assert(n <= lanes);
      std::array<fp16,lanes> values; values.fill(fill);
      if (n) std::memcpy(values.data(), p, n * sizeof(fp16));
      return load(values.data());
    }
    /// Write the representations of lanes [0,n) to exactly n accessible FP16
    /// objects, where n <= 32. Memory outside that prefix is untouched. No access
    /// occurs for n == 0, when p may be null; otherwise p must address n objects.
    native_inline void store_partial(fp16 * p, std::size_t n) const noexcept {
      assert(n <= lanes);
      if (n) std::memcpy(p, &value_, n * sizeof(fp16));
    }
    /// Add corresponding half lanes, rounding directly under the caller's MXCSR rounding control.
    native_nodiscard friend native_inline simd operator+(simd a,simd b) noexcept {
      return from_native(detail::avx512_fp16_backend::add_half(a.value_,b.value_));
    }
    /// Subtract corresponding half lanes, rounding directly under the caller's MXCSR rounding control.
    native_nodiscard friend native_inline simd operator-(simd a,simd b) noexcept {
      return from_native(detail::avx512_fp16_backend::sub_half(a.value_,b.value_));
    }
    /// Multiply corresponding half lanes, rounding directly under the caller's MXCSR rounding control.
    native_nodiscard friend native_inline simd operator*(simd a,simd b) noexcept {
      return from_native(detail::avx512_fp16_backend::mul_half(a.value_,b.value_));
    }
    /// Divide corresponding half lanes with native half-precision rounding.
    /// The caller's MXCSR rounding control and native exception behavior apply.
    native_nodiscard friend native_inline simd operator/(simd a,simd b) noexcept {
      return from_native(detail::avx512_fp16_backend::div_half(a.value_,b.value_));
    }
    /// Compute each half lane's square root with native half-precision rounding.
    /// Signed zero is preserved; negative nonzero operands produce a quiet NaN.
    /// The caller's MXCSR rounding control and native exception behavior apply.
    native_nodiscard friend native_inline simd sqrt(simd a) noexcept {
      return from_native(detail::avx512_fp16_backend::sqrt_half(a.value_));
    }
    /// Toggle every sign bit, preserving all payload bits without arithmetic exceptions.
    native_nodiscard friend native_inline simd operator-(simd a) noexcept {
      return from_native(detail::avx512_fp16_backend::neg_half(a.value_));
    }
    /// Ordered lane equality. NaNs compare false; signed zeros compare equal.
    /// DAZ does not flush half operands. Signaling NaNs raise native invalid status.
    native_nodiscard friend native_inline mask operator==(simd a,simd b) noexcept {
      return mask::from_native(detail::avx512_fp16_backend::eq_half(a.value_,b.value_));
    }
    /// Lane inequality, true for unordered NaN operands; complements native equality.
    native_nodiscard friend native_inline mask operator!=(simd a,simd b) noexcept { return ~(a==b); }
    /// Ordered lane less-than. NaNs compare false; native MXCSR exception semantics apply.
    native_nodiscard friend native_inline mask operator<(simd a,simd b) noexcept {
      return mask::from_native(detail::avx512_fp16_backend::lt_half(a.value_,b.value_));
    }
    /// Ordered lane less-or-equal. NaNs compare false; native MXCSR exception semantics apply.
    native_nodiscard friend native_inline mask operator<=(simd a,simd b) noexcept {
      return mask::from_native(detail::avx512_fp16_backend::le_half(a.value_,b.value_));
    }
    /// Ordered lane greater-than, with the native less-than operands reversed.
    native_nodiscard friend native_inline mask operator>(simd a,simd b) noexcept { return b<a; }
    /// Ordered lane greater-or-equal, with native less-or-equal operands reversed.
    native_nodiscard friend native_inline mask operator>=(simd a,simd b) noexcept { return b<=a; }
    /// Choose a lane from a when its canonical mask lane is true, otherwise b.
    /// Selection copies every representation bit without arithmetic or NaN quieting.
    native_nodiscard friend native_inline simd select(mask m,simd a,simd b) noexcept {
      return from_native(detail::avx512_fp16_backend::select_half(m.to_native(),a.value_,b.value_));
    }
  };


  /// Compute a*b+c in each lane with one final half-precision rounding (VFMADD*PH).
  /// MXCSR rounding and exception controls apply, status flags may change, and
  /// DAZ/FTZ are ignored. Control bits are preserved; NaNs follow the instruction.
  template<::native::isa Arch> requires NATIVE_ARCH_REQUIRES(Arch)
  native_nodiscard native_inline simd<fp16,32,Arch> fma(
      simd<fp16,32,Arch> a,simd<fp16,32,Arch> b,simd<fp16,32,Arch> c) noexcept {
    return simd<fp16,32,Arch>::from_native(
      detail::avx512_fp16_backend::fma_half(a.to_native(),b.to_native(),c.to_native()));
  }
}

#pragma clang attribute pop
#undef NATIVE_ARCH_REQUIRES
