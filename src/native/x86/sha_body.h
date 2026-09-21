// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
  /// \defgroup x86_sha SHA instruction primitives
  /// Four-dword register values follow the instruction packing, not four
  /// independent hashes. SHA-1 state and SHA-256 state packs run high-to-low;
  /// SHA-256 message words run low-to-high. Runtime calls require SHA and the
  /// matching compiler target. Constant evaluation is exact modulo 32 bits.
  /// Missing-feature forms are consteval-only and still require SSE2 storage.
  /// \{

  /// Four SHA-1 rounds; Selector in [0,3] selects the function and constant.
  template<isa<x86> Arch, unsigned Selector> requires(Arch.has(x86_feature::sha) && Selector < 4)
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha1rnds4(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::rounds1<Selector>(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha1rnds4<Arch, Selector>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch, unsigned Selector> requires(!Arch.has(x86_feature::sha) && Selector < 4 &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha1rnds4(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::rounds1<Selector>(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, unsigned Selector, class... Args>
  void sha1rnds4(Args...) = delete;

  /// Add the next E value, derived from a, to the high dword of b.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha1nexte(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::next_e(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha1nexte<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha1nexte(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::next_e(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha1nexte(Args...) = delete;

  /// First SHA-1 message-schedule step.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha1msg1(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::message1_first(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha1msg1<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha1msg1(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::message1_first(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha1msg1(Args...) = delete;

  /// Final SHA-1 message-schedule step.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha1msg2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::message1_last(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha1msg2<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha1msg2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::message1_last(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha1msg2(Args...) = delete;

  /// Two SHA-256 rounds: a is CDGH, b is ABEF, c contains two message-plus-constant words.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha256rnds2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b, simd<std::uint32_t, 4, Arch> c) noexcept {
    if consteval {
      return detail::x86_sha_constant::rounds256(a, b, c);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha256rnds2<Arch>(a.to_native(), b.to_native(), c.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha256rnds2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b, simd<std::uint32_t, 4, Arch> c) noexcept {
    return detail::x86_sha_constant::rounds256(a, b, c);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha256rnds2(Args...) = delete;

  /// First SHA-256 message-schedule step.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha256msg1(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::message256_first(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha256msg1<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha256msg1(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::message256_first(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha256msg1(Args...) = delete;

  /// Final SHA-256 message-schedule step.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  constexpr simd<std::uint32_t, 4, Arch> sha256msg2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    if consteval {
      return detail::x86_sha_constant::message256_last(a, b);
    } else {
      return simd<std::uint32_t, 4, Arch>::from_native(
        detail::x86_sha::sha256msg2<Arch>(a.to_native(), b.to_native()));
    }
  }

  /// Evaluate constant operands without SHA.
  template<isa<x86> Arch> requires(!Arch.has(x86_feature::sha) &&
      requires { sizeof(simd<std::uint32_t, 4, Arch>); })
  native_nodiscard consteval simd<std::uint32_t, 4, Arch> sha256msg2(
    simd<std::uint32_t, 4, Arch> a, simd<std::uint32_t, 4, Arch> b) noexcept {
    return detail::x86_sha_constant::message256_last(a, b);
  }

  /// Reject raw registers, mixed tags and unsupported vector shapes.
  template<isa<x86> Arch, class... Args>
  void sha256msg2(Args...) = delete;

  /// \}
}
#endif
