#pragma once
#include "simd/attributes.h"
#include <array>
#include <span>
#include <concepts>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>

/** \defgroup vectors Vectors and architecture tags
 * One register-shaped value, with the element, lane count and architecture in
 * its type. Import the matching profile and compile the caller for that ISA.
 * CPU and OS feature admission belongs at the application boundary.
 * \snippet api.cc vector_construction
 */
/** \defgroup masks Masks and selection
 * Comparisons produce `V::mask`. Use that type instead of assuming that every
 * architecture stores a full register of zero/all-one lanes. Mask reductions
 * inspect logical lanes only; padding in short vectors is not part of a result.
 * \snippet api.cc masks
 */
/** \defgroup vector_memory Vector memory
 * Loads select a concrete vector type. Full operations require storage for all
 * logical lanes; partial operations touch exactly the requested prefix.
 * Alignment is a caller promise, not a runtime check.
 * \snippet api.cc memory
 */
/** \defgroup vector_math Raw vector math
 * Raw float operations inherit the caller's floating-point environment.
 * They do not establish FTZ policy or a reproducible scalar type. Use unqualified
 * calls in generic code so the element library can supply its own operations.
 * \snippet api.cc arithmetic
 */
namespace simd {
  static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix == 2 && std::numeric_limits<float>::digits == 24 &&
    std::numeric_limits<float>::min_exponent == -125 && std::numeric_limits<float>::max_exponent == 128,
    "simd math requires IEEE 754 binary32 float");

  using std::int8_t;
  using std::int16_t;
  using std::int32_t;
  using std::int64_t;
  using std::uint8_t;
  using std::uint16_t;
  using std::uint32_t;
  using std::uint64_t;

  template <class T> concept simd_integer_element =
    std::same_as<T,int8_t> || std::same_as<T,int16_t> ||
    std::same_as<T,int32_t> || std::same_as<T,int64_t> ||
    std::same_as<T,uint8_t> || std::same_as<T,uint16_t> ||
    std::same_as<T,uint32_t> || std::same_as<T,uint64_t>;

  /// \ingroup vector_memory
  /// Stable compaction's packed register and number of selected logical lanes.
  template<class V> requires requires { typename V::value_type; } &&
    (std::same_as<typename V::value_type,float> ||
     std::same_as<typename V::value_type,std::int32_t> ||
     std::same_as<typename V::value_type,std::uint32_t>)
  struct compaction_result {
    V value;
    std::size_t count;
  };

  /// \ingroup vectors
  /// Carry a compile-time lane index or instruction immediate as a value.
  template <std::size_t K> struct imm_t {
    static constexpr std::size_t value = K;
    /// Return the immediate value K during constant evaluation.
    simd_nodiscard simd_inline simd_const consteval operator std::size_t() const noexcept { return K; }
  };
  /// \ingroup vectors
  /// The value form of an immediate: for example, `broadcast(v, simd::imm<1>)`.
  template <std::size_t K> inline constexpr imm_t<K> imm{};

  /// \ingroup masks
  /// A Boolean lane with exactly zero or all-one object representation.
  /// Numeric arithmetic is deliberately absent. `from_bits` tests nonzero;
  /// it does not preserve arbitrary input bits.
  template<class U> requires (std::is_unsigned_v<U> && !std::same_as<U,bool> && (sizeof(U)==1 || sizeof(U)==2 || sizeof(U)==4 || sizeof(U)==8))
  struct mask_lane {
    using storage_type=U;
    /// Initialize this lane to false.
    simd_inline constexpr mask_lane() noexcept = default;
    /// Represent the supplied truth value as zero or all-one bits.
    explicit simd_inline constexpr mask_lane(bool value) noexcept : value_(value?U(~U(0)):U(0)) {}
    /// Normalize nonzero bits to a true, all-one lane.
    simd_nodiscard static simd_inline simd_const constexpr mask_lane from_bits(U value) noexcept { return mask_lane(value!=0); }
    /// Return the canonical zero or all-one representation.
    simd_nodiscard simd_inline simd_pure constexpr U to_bits() const noexcept { return value_; }
    /// Return the lane truth value.
    simd_nodiscard simd_inline simd_pure constexpr bool to_bool() const noexcept { return value_!=0; }
    /// Return the lane-wise logical complement, retaining this mask type.
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator!(mask_lane a) noexcept { return mask_lane(!a.to_bool()); }
    /// Invert each lane truth value, preserving the mask representation.
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator~(mask_lane a) noexcept { return !a; }
    /// Bitwise AND of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator&(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()&&b.to_bool()); }
    /// Bitwise OR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator|(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()||b.to_bool()); }
    /// Bitwise XOR of corresponding lane representations.
    simd_nodiscard friend simd_inline simd_const constexpr mask_lane operator^(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()!=b.to_bool()); }
    /// Compare the two canonical lane truth values and return bool.
    simd_nodiscard friend simd_inline simd_const constexpr bool operator==(mask_lane,mask_lane) noexcept = default;
  private:
    U value_=0;
  };
  using mask8=mask_lane<std::uint8_t>;
  using mask16=mask_lane<std::uint16_t>;
  using mask32=mask_lane<std::uint32_t>;
  using mask64=mask_lane<std::uint64_t>;
  namespace detail {
    template<class T> inline constexpr bool is_mask_lane=false;
    template<class U> inline constexpr bool is_mask_lane<mask_lane<U>> = true;
  }
  template<class T> concept simd_mask_element=detail::is_mask_lane<T>;

  // Custom elements supply their storage register and their own value semantics.
  // The core never lifts raw arithmetic into a user-defined element implicitly.
  /// \ingroup vectors
  /// Specialize with `storage_type` to opt an element into vector customization.
  /// The storage type must have a raw vector for each requested shape.
  template <class T> struct simd_traits;
  /// \ingroup vectors
  /// Supply a custom element's constructors, memory and arithmetic semantics.
  /// `Raw` is the selected storage vector; `Self` is the public custom vector.
  /// The core does not infer normalization or arithmetic from the storage type.
  template <class T, class Raw, class Self> struct simd_customization;
  template <class T> concept simd_custom_element = requires {
    typename simd_traits<T>::storage_type;
  };
  namespace detail {
    template <class... X> struct simd_deduced_element { using type = float; };
    template <class X, class... Rest> struct simd_deduced_element<X, Rest...> {
      using type = std::conditional_t<simd_custom_element<std::remove_cvref_t<X>>,
        std::remove_cvref_t<X>, typename simd_deduced_element<Rest...>::type>;
    };
    template <class... X> using simd_deduced_element_t = typename simd_deduced_element<X...>::type;
  }

  namespace detail {
  // Native registers are trivial values: default initialization leaves lanes
  // unspecified; broadcast, load, or value initialization supplies every lane.
  // Partial operations touch exactly n lanes (0 <= n <= lanes).
  template<class V, std::size_t L> struct register_memory {
    static constexpr std::size_t lanes = L;
    simd_nodiscard static simd_inline simd_pure V load_partial(simd_noescape float const * p, std::size_t n, float fill = 0) noexcept {
      alignas(64) std::array<float, L> a; a.fill(fill);
      for (std::size_t i = 0; i < n; ++i) a[i] = p[i];
      return V::load(a.data());
    }
    simd_inline void store_partial(simd_noescape float * p, std::size_t n) const noexcept {
      alignas(64) std::array<float, L> a; static_cast<V const &>(*this).store(a.data());
      for (std::size_t i = 0; i < n; ++i) p[i] = a[i];
    }
  };
  }
  /// \ingroup vectors
  /// Baseline scalar profile; only a one-lane raw vector is provided.
  struct scalar {};
  /// \ingroup vectors
  /// AVX2, FMA and BMI2 profile. The tag performs no runtime detection.
  struct avx2 {};
  /// \ingroup vectors
  /// AVX-512 F/DQ/BW/VL profile, also supporting smaller native shapes.
  struct avx512 {};
  /// \ingroup vectors
  /// AVX-512 F/DQ/BW/VL plus BF16 pairwise dot products, with a distinct vector type.
  /// Import simd.avx512_bf16 and compile with the AVX512_BF16 profile. This tag
  /// neither selects compiler flags nor performs CPU/OS feature admission.
  struct avx512_bf16 {};
  namespace detail {
    template<class A> concept avx512_architecture = std::same_as<A,avx512> ||
      std::same_as<A,avx512_bf16>;
  }
  /// \ingroup vectors
  /// AArch64 NEON profile. The tag does not change the caller's compiler flags.
  struct neon {};
  /// \ingroup vectors
  /// AArch64 NEON plus native FP16 arithmetic, with a distinct vector type.
  /// Import simd.neon_fp16 and compile for NEON_FP16. The tag neither selects
  /// compiler flags nor admits runtime features; use simd.arm before entry.
  struct neon_fp16 {};
  /// AArch64 NEON plus native BF16 dot products. Import simd.neon_bf16 and
  /// compile for NEON_BF16; the tag does not perform runtime ISA admission.
  struct neon_bf16 {};
  namespace detail {
    template<class A> concept neon_architecture = std::same_as<A,neon> ||
      std::same_as<A,neon_fp16> || std::same_as<A,neon_bf16>;
  }
  template<class A> concept architecture = std::same_as<A,scalar> || std::same_as<A,avx2> ||
    detail::avx512_architecture<A> || detail::neon_architecture<A>;
  /** \ingroup vectors
   * \brief A value with `N` logical lanes of `T` for `Arch`.
   * `N` describes lanes, not a count of registers. Unsupported shapes remain
   * incomplete; use wide for several registers. Two- and three-lane native
   * vectors have four-lane physical storage but touch only their logical lanes
   * in memory. Scalar float and full-register integer storage initialize to zero. Default
   * initialization of full native float and short vectors leaves storage unspecified. Use braces,
   * broadcast construction or a load before reading those lanes. Custom elements
   * retain their customization's initialization contract.
   *
   * `value_type`, `lanes`, `architecture`, `mask` and `rebind<U>` describe the
   * selected specialization. Comparisons return masks, not a scalar bool.
   * Named swizzles return owning values; repeated destinations cannot be assigned.
   * \snippet api.cc swizzles
   */
  template<class T, std::size_t N, architecture Arch> struct vec;
  /// \ingroup masks
  /// A compact predicate for a shape supported by the selected architecture.
  /// Bit `i` describes lane `i`; bits above the logical lane count are cleared.
  template<std::size_t N, architecture Arch> struct predicate;
  namespace detail { template<class T,std::size_t N,class Arch> struct swizzle_access; }
  template<simd_custom_element T, std::size_t N, architecture Arch>
  struct simd_empty_bases vec<T,N,Arch> : simd_customization<T,
      vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>, detail::swizzle_access<T,N,Arch> {
    using base = simd_customization<T,vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>;
    using base::base;
    /// Default-construct the element customization; its initialization contract is retained.
    simd_inline constexpr vec() = default;
    using architecture = Arch;
    using mask = typename vec<typename simd_traits<T>::storage_type,N,Arch>::mask;
    using predicate_type = predicate<N,Arch>;
    template<class U> using rebind = vec<U,N,Arch>;
    /// Select this architecture and forward arguments to the corresponding constructor.
    /// Exception behavior is exactly that of the forwarded construction.
    template<class... X> requires std::constructible_from<base,X...>
    simd_inline constexpr vec(Arch, X &&... x)
      noexcept(std::is_nothrow_constructible_v<base,X...>) : base(std::forward<X>(x)...) {}
  };
  /// Deduce the element and lane count from an array, retaining the explicit architecture tag.
  template<architecture Arch, class T, std::size_t N> vec(Arch,std::array<T,N> const &) -> vec<T,N,Arch>;
  /// Deduce integral lanes using their common type and retain the explicit architecture tag.
  template<architecture Arch, class... T> requires(sizeof...(T)>0) && (std::integral<T> && ...)
  vec(Arch,T...) -> vec<std::common_type_t<T...>,sizeof...(T),Arch>;
  /// Deduce a homogeneous mask vector from its lane arguments and explicit architecture.
  template<architecture Arch, class T, class... X> requires simd_mask_element<T> && (std::same_as<T,X> && ...)
  vec(Arch,T,X...) -> vec<T,1+sizeof...(X),Arch>;
  /// Deduce a custom element when present, otherwise float; retain the argument count and explicit architecture.
  template<architecture Arch, class... X> requires(sizeof...(X)>0) &&
    (!(std::integral<X> && ...)) && (!(simd_mask_element<X> && ...)) &&
    (std::convertible_to<X,detail::simd_deduced_element_t<X...>> && ...)
  vec(Arch,X...) -> vec<detail::simd_deduced_element_t<X...>,sizeof...(X),Arch>;

  // Native-register conversions must not make different architectures mix.
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>>(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator==(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator!=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,architecture A,architecture B> requires(!std::same_as<A,B> || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>>=(vec<T,N,A> &,vec<U,M,B>) = delete;

  /// \ingroup vector_memory
  /// Requested access policy. Streaming currently uses ordinary memory access.
  enum class simd_access { ordinary, streaming };
  /// \ingroup vector_memory
  /// Describe a power-of-two byte alignment and an access hint.
  /// `non_temporal` is false: this policy does not promise streaming instructions.
  template <std::size_t Alignment = 1, simd_access Access = simd_access::ordinary>
  struct simd_memory {
    static_assert(Alignment > 0 && (Alignment & (Alignment - 1)) == 0);
    static constexpr std::size_t alignment = Alignment;
    static constexpr simd_access requested_access = Access;
    // Streaming is currently an ordinary-access fallback on every backend.
    static constexpr bool non_temporal = false;
  };
  /// \ingroup vector_memory
  /// Load all `V::lanes` elements using the element's memory customization.
  /// \pre `p` addresses that many readable elements and meets alignment `A`.
  /// Exceptions propagate from the selected `V::load_memory<A>` operation.
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires requires(U const * p) { V::template load_memory<A>(p); }
  simd_nodiscard simd_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template load_memory<A>(p))) {
    return V::template load_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Load custom elements through their typed memory operation, then project raw storage.
  /// The customization owns any normalization; this is not an arbitrary pointer cast.
  template <class V, simd_custom_element U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  simd_nodiscard simd_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template rebind<U>::template load_memory<A>(p).to_native())) {
    return V::template rebind<U>::template load_memory<A>(p).to_native();
  }
  /// \ingroup vector_memory
  /// Store all logical lanes using the element's memory customization.
  /// \pre `p` addresses that many writable elements and meets alignment `A`.
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires requires(V value,U * p) { value.template store_memory<A>(p); }
  simd_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(value.template store_memory<A>(p))) {
    value.template store_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Convert raw storage to the custom element vector before its typed store.
  /// Construction and storage both contribute to the exception specification.
  template <simd_custom_element U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  simd_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(typename V::template rebind<U>(value).template store_memory<A>(p))) {
    typename V::template rebind<U>(value).template store_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Load an array or fixed-extent span whose extent equals the lane count.
  template<class V,class U,std::size_t N> requires(N==V::lanes) &&
    requires(U const * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd(std::array<U,N> const & values)
      noexcept(noexcept(::simd::load_simd<V>(values.data()))) {
    return ::simd::load_simd<V>(values.data());
  }
  /// \ingroup vector_memory
  /// Load an array or fixed-extent span whose extent equals the lane count.
  template<class V,class U,std::size_t N> requires(N==V::lanes) &&
    requires(U * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd(std::span<U,N> values)
      noexcept(noexcept(::simd::load_simd<V>(values.data()))) {
    return ::simd::load_simd<V>(values.data());
  }
  /// \ingroup vector_memory
  /// Load `count` elements and fill the remaining lanes with `fill`.
  /// \pre `count <= V::lanes`; `p` may be null only when `count == 0`.
  /// Uses an element temporary; construction, assignment and loading may throw.
  /// The alignment hint does not extend the readable prefix.
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::default_initializable<U> &&
      std::constructible_from<U,typename V::value_type &> && std::is_copy_assignable_v<U> &&
      requires(U const * p) { ::simd::load_simd<V>(p); }
  simd_nodiscard simd_inline V load_simd_partial(U const * p,std::size_t count,
      typename V::value_type fill={},simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_constructible_v<U,typename V::value_type &> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::simd::load_simd<V>(p))) {
    std::array<U,V::lanes> temporary; temporary.fill(U(fill));
    for(std::size_t i=0;i<count;++i) temporary[i]=p[i];
    return ::simd::load_simd<V>(temporary.data());
  }
  /// \ingroup vector_memory
  /// Store the first `count` logical lanes, leaving the following memory alone.
  /// \pre `count <= V::lanes`; `p` may be null only when `count == 0`.
  /// Element construction, assignment and storage determine the exception guarantee.
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires std::default_initializable<U> && std::is_copy_assignable_v<U> &&
      requires(U * p,V value) { ::simd::store_simd(p,value); }
  simd_inline void store_simd_partial(U * p,V value,std::size_t count,simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::simd::store_simd(p,value))) {
    std::array<U,V::lanes> temporary; ::simd::store_simd(temporary.data(),value);
    for(std::size_t i=0;i<count;++i) p[i]=temporary[i];
  }

}

#include "simd/simd/swizzle.h"

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Declares SIMD element types and pointer access policies.
 */
