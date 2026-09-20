#pragma once
#include "native/attributes.h"
#include "native/isa.h"
#include "native/mask_traits.h"
#include "native/value_traits.h"
#include <array>
#include <span>
#include <concepts>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>

/** \defgroup vectors Vectors and ISA values
 * One register-shaped value, with the element, lane count and architecture in
 * its type. Import `native` and compile the caller for the required ISA.
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
namespace native {
  static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559 &&
    std::numeric_limits<float>::radix == 2 && std::numeric_limits<float>::digits == 24 &&
    std::numeric_limits<float>::min_exponent == -125 && std::numeric_limits<float>::max_exponent == 128,
    "native math requires IEEE 754 binary32 float");

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
    native_nodiscard native_inline native_const consteval operator std::size_t() const noexcept { return K; }
  };
  /// \ingroup vectors
  /// The value form of an immediate: for example, `broadcast(v, ::native::imm<1>)`.
  template <std::size_t K> inline constexpr imm_t<K> imm{};

  /// \ingroup masks
  /// A Boolean lane with exactly zero or all-one object representation.
  /// Numeric arithmetic is deliberately absent. `from_bits` tests nonzero;
  /// it does not preserve arbitrary input bits.
  template<class U> requires (std::is_unsigned_v<U> && !std::same_as<U,bool> && (sizeof(U)==1 || sizeof(U)==2 || sizeof(U)==4 || sizeof(U)==8))
  struct mask_lane {
    using storage_type=U;
    /// Initialize this lane to false.
    native_inline constexpr mask_lane() noexcept = default;
    /// Represent the supplied truth value as zero or all-one bits.
    explicit native_inline constexpr mask_lane(bool value) noexcept : value_(value?U(~U(0)):U(0)) {}
    /// Normalize nonzero bits to a true, all-one lane.
    native_nodiscard static native_inline native_const constexpr mask_lane from_bits(U value) noexcept { return mask_lane(value!=0); }
    /// Return the canonical zero or all-one representation.
    native_nodiscard native_inline native_pure constexpr U to_bits() const noexcept { return value_; }
    /// Return the lane truth value.
    native_nodiscard native_inline native_pure constexpr bool to_bool() const noexcept { return value_!=0; }
    /// Return the lane-wise logical complement, retaining this mask type.
    native_nodiscard friend native_inline native_const constexpr mask_lane operator!(mask_lane a) noexcept { return mask_lane(!a.to_bool()); }
    /// Invert each lane truth value, preserving the mask representation.
    native_nodiscard friend native_inline native_const constexpr mask_lane operator~(mask_lane a) noexcept { return !a; }
    /// Bitwise AND of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr mask_lane operator&(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()&&b.to_bool()); }
    /// Bitwise OR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr mask_lane operator|(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()||b.to_bool()); }
    /// Bitwise XOR of corresponding lane representations.
    native_nodiscard friend native_inline native_const constexpr mask_lane operator^(mask_lane a,mask_lane b) noexcept { return mask_lane(a.to_bool()!=b.to_bool()); }
    /// Compare the two canonical lane truth values and return bool.
    native_nodiscard friend native_inline native_const constexpr bool operator==(mask_lane,mask_lane) noexcept = default;
  private:
    U value_=0;
  };
  using mask8=mask_lane<std::uint8_t>;
  using mask16=mask_lane<std::uint16_t>;
  using mask32=mask_lane<std::uint32_t>;
  using mask64=mask_lane<std::uint64_t>;
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
    template<class V,class U,class=void> struct memory_architecture : value_architecture<V> {};
    template<class V,simd_custom_element U>
      requires std::same_as<typename V::value_type,typename simd_traits<U>::storage_type> &&
        value_architecture<V>::known &&
        value_architecture<typename V::template rebind<U>>::known
    struct memory_architecture<V,U,std::void_t<typename V::template rebind<U>>> {
      static constexpr isa value=value_architecture_v<V> &
        value_architecture_v<typename V::template rebind<U>>;
      static constexpr bool known=true;
    };
    template<class V,class U> inline constexpr isa memory_architecture_v=memory_architecture<V,U>::value;
  }

  namespace detail {
  // Native registers are trivial values: default initialization leaves lanes
  // unspecified; broadcast, load, or value initialization supplies every lane.
  // Partial operations touch exactly n lanes (0 <= n <= lanes).
  template<class V, std::size_t L> struct register_memory;
  }
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
  template<class T, std::size_t N, isa Arch> struct vec;
  /// \ingroup masks
  /// A compact predicate for a shape supported by the selected architecture.
  /// Bit `i` describes lane `i`; bits above the logical lane count are cleared.
  template<std::size_t N, isa Arch> struct predicate;

  /// \ingroup masks
  /// Use the vector's selected full-vector or compact comparison mask.
  template<class T, std::size_t N, isa Arch>
    requires requires { typename vec<T, N, Arch>::mask_type; }
  struct mask_traits<vec<T, N, Arch>> {
    using type = typename vec<T, N, Arch>::mask_type;
  };

  /// \ingroup masks
  /// A supported compact predicate is already a logical mask.
  template<std::size_t N, isa Arch>
    requires requires { typename predicate<N, Arch>::native_type; }
  struct mask_traits<predicate<N, Arch>> { using type = predicate<N, Arch>; };

  /// \ingroup masks
  /// Keep the logical lane's zero/all-one representation.
  template<class U> requires requires { typename mask_lane<U>; }
  struct mask_traits<mask_lane<U>> { using type = mask_lane<U>; };

  namespace detail { template<class T,std::size_t N,isa Arch> struct swizzle_access {}; }

  // Native-register conversions must not make different architectures mix.
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>>(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator==(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator!=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator>=(vec<T,N,A>,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator+=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator-=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator*=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator/=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator%=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator&=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator|=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator^=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
  void operator<<=(vec<T,N,A> &,vec<U,M,B>) = delete;
  /// Reject mixed architectures and mismatched short-vector widths before native conversions can participate.
  template<class T,class U,std::size_t N,std::size_t M,isa A,isa B> requires(A!=B || (N!=M && (N==2 || N==3 || M==2 || M==3)))
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

}



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
