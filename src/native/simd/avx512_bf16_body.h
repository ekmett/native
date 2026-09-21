#define NATIVE_ARCH_REQUIRES(A) (::native::avx512_bf16 <= A)
#pragma clang attribute push(__attribute__((target("avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"))), apply_to=function)
export namespace native {
  namespace detail {
    template<std::size_t N,::native::isa<> A> requires NATIVE_ARCH_REQUIRES(A) &&(N==8 || N==16 || N==32)
    struct value_traits<simd<bf16,N,A>> {
      static constexpr isa<> value=avx512_bf16;
      static constexpr bool known=true;
      static constexpr bool aggregate_default=false;
    };
  }
  /// \ingroup vectors
  /// One 128-, 256-, or 512-bit register of BF16 representations. Loads, stores and bit bridges
  /// preserve every encoding. This does not add elementwise BF16 arithmetic or
  /// change the scalar bf16 conversion contract. The 8-, 16-, and 32-lane shapes are provided.
  /// The application must admit that CPU/OS profile before entering compiled code.
  /// Every storage operation preserves subnormal, signed-zero and NaN encodings;
  /// none performs a floating-point conversion or quiets a signaling NaN.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&(N == 8 || N == 16 || N == 32)
  struct simd<bf16,N,Arch> {
    /// Scalar storage element; each lane retains all 16 representation bits.
    using value_type = bf16;
    /// The distinct compile-time AVX512_BF16 instruction profile.
    static constexpr isa<> architecture=Arch;
    /// This one-register vector type, for generic register-based algorithms.
    using register_type = simd;
    /// Native register-width BF16 register representation; native bridges copy bits.
    using native_type = std::conditional_t<N == 8,__m128bh,std::conditional_t<N == 16,__m256bh,__m512bh>>;
    /// Unsigned 16-bit lanes in the same profile and lane order.
    using bits_type = simd<std::uint16_t,N,architecture>;
    /// Compact predicate with bit i selecting BF16 lane i.
    using mask = predicate<N,architecture>;
    /// Generic mask spelling for the compact lane predicate.
    using mask_type = mask;
    /// Explicit predicate spelling for the same compact mask type.
    using predicate_type = mask;
    /// Full-register mask shape with zero or all-one 16-bit lanes.
    using vector_mask_type = simd<mask16,N,architecture>;
    /// Replace the element type while retaining N lanes and this profile.
    /// Unsupported resulting shapes remain incomplete.
    template<class T> using rebind = simd<T,N,architecture>;
    /// Number of logical BF16 lanes, with no padding lanes.
    static constexpr std::size_t lanes = N;
  private:
    native_type value_;
  public:
    /// Default initialization leaves storage unspecified; braces zero it.
    simd() noexcept = default;
    /// Broadcast the exact representation of value to all N lanes.
    native_inline constexpr explicit simd(bf16 value) noexcept
      : value_(std::bit_cast<native_type>(bits_type(value.to_bits()).to_native())) {}
    /// Copy array element i into lane i without conversion or representation changes.
    native_inline constexpr explicit simd(std::array<bf16,lanes> const & values) noexcept : simd(load(values.data())) {}
    /// Construct all N lanes from BF16 values in argument order, preserving their bits.
    template<class... T> requires(sizeof...(T) == lanes && (std::same_as<T,bf16> && ...))
    native_inline constexpr simd(T... values) noexcept : simd(std::array<bf16,lanes>{values...}) {}
    /// Adopt a native register without conversion or representation changes.
    native_inline constexpr simd(native_type value) noexcept : value_(value) {}
    /// Project the native register for direct intrinsic interoperability.
    native_nodiscard native_inline constexpr operator native_type() const noexcept { return value_; }
    // Native interoperability must not add elementwise BF16 arithmetic.
#include "native/simd/bf16_reject_operators.h"
    /// Return all lane bits as a native register, without conversion or lane reordering.
    native_nodiscard native_inline constexpr native_type to_native() const noexcept { return value_; }
    /// Copy a native BF16 register into this vector, preserving every representation bit.
    native_nodiscard static native_inline constexpr simd from_native(native_type value) noexcept {
      simd result; result.value_ = value; return result;
    }
    /// Return the 16-bit representation of each lane in an unsigned vector.
    native_nodiscard native_inline constexpr bits_type bits() const noexcept {
      return bits_type::from_native(std::bit_cast<typename bits_type::native_type>(value_));
    }
    /// Synonym for bits(); this is a representation bridge, not a numeric conversion.
    native_nodiscard native_inline constexpr bits_type to_bits() const noexcept { return bits(); }
    /// Interpret each unsigned lane as a BF16 representation without changing its bits.
    native_nodiscard static native_inline constexpr simd from_bits(bits_type value) noexcept {
      return from_native(std::bit_cast<native_type>(value.to_native()));
    }
    /// Read exactly N accessible uint16_t objects into corresponding BF16 lane bits.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    native_nodiscard static native_inline constexpr simd load_bits(std::uint16_t const * p) noexcept {
      return from_bits(bits_type::load(p));
    }
    /// Write every lane representation to N accessible uint16_t objects in lane order.
    /// No alignment beyond that of uint16_t is required; p must not be null.
    native_inline constexpr void store_bits(std::uint16_t * p) const noexcept { bits().store(p); }
    /// Read exactly N accessible BF16 objects, preserving every encoding.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for BF16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    native_nodiscard static native_inline constexpr simd load_memory(bf16 const * p) noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      if consteval {
        std::array<std::uint16_t,lanes> words{};
        for (std::size_t i=0;i<lanes;++i) words[i]=p[i].to_bits();
        return load_bits(words.data());
      }
      native_type value; std::memcpy(&value, p, sizeof(value)); return from_native(value);
    }
    /// Write all lane representations to exactly N accessible BF16 objects.
    /// Alignment is a nonzero power-of-two byte-alignment promise, not a runtime
    /// check. The default imposes no alignment beyond that required for BF16 objects.
    /// p must not be null.
    template<std::size_t Alignment = 1>
    native_inline constexpr void store_memory(bf16 * p) const noexcept {
      static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
      if consteval {
        std::array<std::uint16_t,lanes> words{}; store_bits(words.data());
        for (std::size_t i=0;i<lanes;++i) p[i]=bf16::from_bits(words[i]);
        return;
      }
      std::memcpy(p, &value_, sizeof(value_));
    }
    /// Load N BF16 objects with the default alignment contract of load_memory().
    native_nodiscard static native_inline constexpr simd load(bf16 const * p) noexcept { return load_memory(p); }
    /// Store N BF16 objects with the default alignment contract of store_memory().
    native_inline constexpr void store(bf16 * p) const noexcept { store_memory(p); }
    /// Synonym for load(); no register-width alignment is required.
    native_nodiscard static native_inline constexpr simd loadu(bf16 const * p) noexcept { return load(p); }
    /// Synonym for store(); no register-width alignment is required.
    native_inline constexpr void storeu(bf16 * p) const noexcept { store(p); }
    /// Read exactly the first n accessible BF16 objects, where n <= N.
    /// Copy their representations to lanes [0,n); remaining lanes receive fill's
    /// exact representation. The default fill is positive zero. No access occurs
    /// for n == 0, when p may be null; otherwise p must address n BF16 objects.
    native_nodiscard static native_inline constexpr simd load_partial(bf16 const * p, std::size_t n,
        bf16 fill = bf16::from_bits(0)) noexcept {
      assert(n <= lanes);
      std::array<bf16,lanes> values; values.fill(fill);
      if consteval {
        for (std::size_t i=0;i<n;++i) values[i]=p[i];
      } else { if (n) std::memcpy(values.data(), p, n * sizeof(bf16)); }
      return load(values.data());
    }
    /// Write the representations of lanes [0,n) to exactly n accessible BF16
    /// objects, where n <= N. Memory outside that prefix is untouched. No access
    /// occurs for n == 0, when p may be null; otherwise p must address n objects.
    native_inline constexpr void store_partial(bf16 * p, std::size_t n) const noexcept {
      assert(n <= lanes);
      if consteval {
        std::array<std::uint16_t,lanes> words{}; store_bits(words.data());
        for (std::size_t i=0;i<n;++i) p[i]=bf16::from_bits(words[i]);
      } else { if (n) std::memcpy(p, &value_, n * sizeof(bf16)); }
    }
  };


  /// Native VDPBF16PS: for each output i, accumulate a[2*i+1]*b[2*i+1]
  /// first, then a[2*i]*b[2*i]. Each FP32 FMA rounds to nearest, ties to even;
  /// input denormals are zero and output denormals are flushed to zero.
  /// MXCSR is neither consulted nor updated, including exception status.
  /// This is not a single-rounding three-term sum. NaN propagation follows
  /// the instruction, with low input lanes taking priority over high lanes.
  template<std::size_t N, ::native::isa<> Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&(N == 8 || N == 16 || N == 32)
  native_nodiscard native_inline simd<float,N/2,Arch> dot2(
      simd<bf16,N,Arch> a, simd<bf16,N,Arch> b,
      simd<float,N/2,Arch> accumulator) noexcept {
    return simd<float,N/2,Arch>::from_native(
      detail::avx512_bf16_backend::dot2_native(a.to_native(), b.to_native(), accumulator.to_native()));
  }
}

#pragma clang attribute pop
#undef NATIVE_ARCH_REQUIRES
