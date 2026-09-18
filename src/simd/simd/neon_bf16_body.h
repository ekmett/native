#define SIMD_ARCH_CONCEPT ::simd::detail::neon_bf16_architecture
#pragma clang attribute push(__attribute__((target("neon,bf16"))), apply_to=function)
export namespace simd {
  /// \ingroup vectors
  /// One 128-bit register of BF16 representations. Loads, stores and bit bridges
  /// preserve every encoding. This does not add elementwise BF16 arithmetic or
  /// change the scalar bf16 conversion contract. Only the 8-lane shape is provided.
  /// The application must admit that CPU/OS profile before entering compiled code.
  /// Every storage operation preserves subnormal, signed-zero and NaN encodings;
  /// none performs a floating-point conversion or quiets a signaling NaN.
  template<SIMD_ARCH_CONCEPT Arch> struct vec<bf16,8,Arch> {
    /// Scalar storage element; each lane retains all 16 representation bits.
    using value_type = bf16;
    /// The distinct compile-time NEON_BF16 instruction-profile tag.
    using architecture = Arch;
    /// Requirements of this value implementation; additional tag features remain available.
    using required_architecture = simd::neon_bf16;
    /// This one-register vector type, for generic register-based algorithms.
    using register_type = vec;
    /// Native 128-bit BF16 register representation; native bridges copy bits.
    using native_type = bfloat16x8_t;
    /// Unsigned 16-bit lanes in the same profile and lane order.
    using bits_type = vec<std::uint16_t,8,architecture>;
    /// Full-register predicate with zero or all-one 16-bit lanes.
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
    /// Number of logical BF16 lanes, with no padding lanes.
    static constexpr std::size_t lanes = 8;
  private:
    native_type value_;
  public:
    /// Default initialization leaves storage unspecified; braces zero it.
    vec() noexcept = default;
    /// Broadcast the exact representation of value to all 8 lanes.
    simd_inline explicit vec(bf16 value) noexcept
      : value_(std::bit_cast<native_type>(bits_type(value.to_bits()).to_native())) {}
    /// Copy array element i into lane i without conversion or representation changes.
    simd_inline explicit vec(std::array<bf16,lanes> const & values) noexcept : vec(load(values.data())) {}
    /// Construct all 8 lanes from BF16 values in argument order, preserving their bits.
    template<class... T> requires(sizeof...(T) == lanes && (std::same_as<T,bf16> && ...))
    simd_inline vec(T... values) noexcept : vec(std::array<bf16,lanes>{values...}) {}
    /// Select this profile explicitly and forward to the matching constructor.
    /// The tag changes neither the argument contract nor runtime ISA admission.
    template<class... T> requires std::constructible_from<vec,T...>
    simd_inline vec(architecture, T &&... values) noexcept(std::is_nothrow_constructible_v<vec,T...>)
      : vec(std::forward<T>(values)...) {}
    /// Adopt a native register without conversion or representation changes.
    simd_inline vec(native_type value) noexcept : value_(value) {}
    /// Project the native register for direct intrinsic interoperability.
    simd_nodiscard simd_inline operator native_type() const noexcept { return value_; }
    // Native interoperability must not add elementwise BF16 arithmetic.
#include "simd/simd/bf16_reject_operators.h"
    /// Return all lane bits as a native register, without conversion or lane reordering.
    simd_nodiscard simd_inline native_type to_native() const noexcept { return value_; }
    /// Copy a native BF16 register into this vector, preserving every representation bit.
    simd_nodiscard static simd_inline vec from_native(native_type value) noexcept {
      vec result; result.value_ = value; return result;
    }
    /// Return the 16-bit representation of each lane in an unsigned vector.
    simd_nodiscard simd_inline bits_type bits() const noexcept {
      return bits_type::from_native(std::bit_cast<typename bits_type::native_type>(value_));
    }
    /// Synonym for bits(); this is a representation bridge, not a numeric conversion.
    simd_nodiscard simd_inline bits_type to_bits() const noexcept { return bits(); }
    /// Interpret each unsigned lane as a BF16 representation without changing its bits.
    simd_nodiscard static simd_inline vec from_bits(bits_type value) noexcept {
      return from_native(std::bit_cast<native_type>(value.to_native()));
    }
    /// Read exactly 8 accessible uint16_t objects into corresponding BF16 lane bits.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    simd_nodiscard static simd_inline vec load_bits(std::uint16_t const * p) noexcept {
      return from_bits(bits_type::load(p));
    }
    /// Write every lane representation to 8 accessible uint16_t objects in lane order.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    simd_inline void store_bits(std::uint16_t * p) const noexcept { bits().store(p); }
    /// Read exactly 8 accessible BF16 objects, preserving every encoding.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for BF16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    simd_nodiscard static simd_inline vec load_memory(bf16 const * p) noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      native_type value; std::memcpy(&value, p, sizeof(value)); return from_native(value);
    }
    /// Write all lane representations to exactly 8 accessible BF16 objects.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for BF16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    simd_inline void store_memory(bf16 * p) const noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      std::memcpy(p, &value_, sizeof(value_));
    }
    /// Load 8 BF16 objects with the default alignment contract of load_memory().
    simd_nodiscard static simd_inline vec load(bf16 const * p) noexcept { return load_memory(p); }
    /// Store 8 BF16 objects with the default alignment contract of store_memory().
    simd_inline void store(bf16 * p) const noexcept { store_memory(p); }
    /// Synonym for load(); no register-width alignment is required.
    simd_nodiscard static simd_inline vec loadu(bf16 const * p) noexcept { return load(p); }
    /// Synonym for store(); no register-width alignment is required.
    simd_inline void storeu(bf16 * p) const noexcept { store(p); }
    /// Read exactly the first n accessible BF16 objects, where n <= 8.
    /// Copy their representations to lanes [0,n); remaining lanes receive fill's
    /// exact representation. The default fill is positive zero. No access occurs
    /// for n == 0, when p may be null; otherwise p must address n BF16 objects.
    simd_nodiscard static simd_inline vec load_partial(bf16 const * p, std::size_t n,
        bf16 fill = bf16::from_bits(0)) noexcept {
      assert(n <= lanes);
      std::array<bf16,lanes> values; values.fill(fill);
      if (n) std::memcpy(values.data(), p, n * sizeof(bf16));
      return load(values.data());
    }
    /// Write the representations of lanes [0,n) to exactly n accessible BF16
    /// objects, where n <= 8. Memory outside that prefix is untouched. No access
    /// occurs for n == 0, when p may be null; otherwise p must address n objects.
    simd_inline void store_partial(bf16 * p, std::size_t n) const noexcept {
      assert(n <= lanes);
      if (n) std::memcpy(p, &value_, n * sizeof(bf16));
    }
  };

  /// Deduce BF16 element type, argument-count lanes, and the explicit BF16 profile.
  /// Arguments must all be BF16 values. Only 8 lanes have an implementation;
  /// deduction of another lane count does not make that shape available.
  template<SIMD_ARCH_CONCEPT Arch, class... T> requires(std::same_as<T,bf16> && ...)
  vec(Arch,bf16,T...) -> vec<bf16,1+sizeof...(T),Arch>;

  /// Native BFDOT: output i combines adjacent products a[2*i]*b[2*i] and
  /// a[2*i+1]*b[2*i+1], then adds accumulator[i]. This follows Arm's native
  /// instruction, not the x86 dot2 rounding/order contract or scalar conversions.
  /// With EBF absent/clear, products, their sum and accumulation round separately
  /// to odd with overflow to infinity; denormal inputs/results flush to zero.
  /// AH affects only the default NaN sign in this mode. With FEAT_EBF16 and
  /// FPCR.EBF set, the pair is fused and rounded before separate accumulation;
  /// FPCR.RMode/FZ/AH/FIZ govern standard single-precision behavior. Both modes
  /// return default NaNs, ignore exception enables, and leave FPSR unchanged.
  /// No operation changes FPCR. Applications own FPCR and ISA admission; this
  /// API makes no reproducible cross-ISA or cross-FPCR result promise.
  template<SIMD_ARCH_CONCEPT Arch>
  simd_nodiscard simd_inline vec<float,4,Arch> dot2(
      vec<bf16,8,Arch> a, vec<bf16,8,Arch> b,
      vec<float,4,Arch> accumulator) noexcept {
    return vec<float,4,Arch>::from_native(
      detail::neon_bf16_backend::dot2_native(a.to_native(), b.to_native(), accumulator.to_native()));
  }
}

#pragma clang attribute pop
#undef SIMD_ARCH_CONCEPT
