// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_gfni GFNI
 * Byte arithmetic modulo x^8 + x^4 + x^3 + x + 1 and binary affine maps.
 * Arch records instruction requirements; callers must separately enable and
 * admit a matching target. The vector lane count selects the overload and
 * minimum target requirements.
 *
 * Each matrix operand contains one 8 by 8 binary matrix per 64-bit lane.
 * Output bit i uses matrix byte 7-i within that lane, with input bit j
 * multiplying bit j of that byte. Imm8 is XORed into every result byte.
 * Inverse-affine first takes the field inverse of each input byte (zero maps
 * to zero), then applies the matrix and Imm8; it does not invert the matrix.
 *
 * Mask bit i selects byte i. Merge forms retain src in inactive bytes;
 * zero forms clear them. LLVM's byte-mask intrinsics require AVX512BW;
 * narrower masked forms also require AVX512VL.
 * All operations depend only on their register arguments and have no side effects.
 * \{ */

  // 128-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni))
  native_nodiscard native_inline native_const native_target("gfni")
  simd<std::uint8_t, 16, Arch> gf2p8mulb(simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8mulb<Arch>(a.to_native(), b.to_native()));
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineqb(simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineinvqb(simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8mulb_mask(simd<std::uint8_t, 16, Arch> src, predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8mulb_mask<Arch>(src.to_native(), k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8mulb_maskz(predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8mulb_maskz<Arch>(k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineqb_mask(simd<std::uint8_t, 16, Arch> src, predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineqb_maskz(predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineinvqb_mask(simd<std::uint8_t, 16, Arch> src, predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 16, Arch> gf2p8affineinvqb_maskz(predicate<16, Arch> k, simd<std::uint8_t, 16, Arch> a, simd<std::uint64_t, 2, Arch> matrix) noexcept {
    return simd<std::uint8_t, 16, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }


  // 256-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx))
  native_nodiscard native_inline native_const native_target("avx,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8mulb(simd<std::uint8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8mulb<Arch>(a.to_native(), b.to_native()));
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineqb(simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineinvqb(simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8mulb_mask(simd<std::uint8_t, 32, Arch> src, predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8mulb_mask<Arch>(src.to_native(), k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8mulb_maskz(predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8mulb_maskz<Arch>(k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineqb_mask(simd<std::uint8_t, 32, Arch> src, predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineqb_maskz(predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineinvqb_mask(simd<std::uint8_t, 32, Arch> src, predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Arch.has(x86_feature::avx512vl) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,avx512vl,gfni")
  simd<std::uint8_t, 32, Arch> gf2p8affineinvqb_maskz(predicate<32, Arch> k, simd<std::uint8_t, 32, Arch> a, simd<std::uint64_t, 4, Arch> matrix) noexcept {
    return simd<std::uint8_t, 32, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }


  // 512-bit registers.
  /// Multiply corresponding bytes in GF(2^8).
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f))
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8mulb(simd<std::uint8_t, 64, Arch> a, simd<std::uint8_t, 64, Arch> b) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8mulb<Arch>(a.to_native(), b.to_native()));
  }

  /// Apply the binary matrix in each 64-bit lane and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineqb(simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Invert each field byte, apply its lane's binary matrix, and XOR Imm8.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512f,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineinvqb(simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb<Arch, Imm8>(a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw))
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8mulb_mask(simd<std::uint8_t, 64, Arch> src, predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint8_t, 64, Arch> b) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8mulb_mask<Arch>(src.to_native(), k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Zero inactive bytes after gf2p8mulb.
  template<isa<x86> Arch> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw))
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8mulb_maskz(predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint8_t, 64, Arch> b) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8mulb_maskz<Arch>(k.to_bitset(), a.to_native(), b.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineqb_mask(simd<std::uint8_t, 64, Arch> src, predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineqb_maskz(predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Merge inactive bytes from src after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineinvqb_mask(simd<std::uint8_t, 64, Arch> src, predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_mask<Arch, Imm8>(src.to_native(), k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  /// Zero inactive bytes after gf2p8affineinvqb.
  template<isa<x86> Arch, unsigned Imm8> requires(Arch.has(x86_feature::gfni) && Arch.has(x86_feature::avx512f) &&
      Arch.has(x86_feature::avx512bw) && Imm8 <= 255)
  native_nodiscard native_inline native_const native_target("avx512bw,gfni")
  simd<std::uint8_t, 64, Arch> gf2p8affineinvqb_maskz(predicate<64, Arch> k, simd<std::uint8_t, 64, Arch> a, simd<std::uint64_t, 8, Arch> matrix) noexcept {
    return simd<std::uint8_t, 64, Arch>::from_native(detail::x86_gfni::gf2p8affineinvqb_maskz<Arch, Imm8>(k.to_bitset(), a.to_native(), matrix.to_native()));
  }

  // Reject implicit register conversions, mixed tags and wrong element types.
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void gf2p8mulb(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineqb(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineinvqb(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void gf2p8mulb_mask(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, class... Args>
  void gf2p8mulb_maskz(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineqb_mask(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineqb_maskz(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineinvqb_mask(Args...) = delete;
  /// Reject unsupported signatures, including implicit raw-register conversions.
  template<isa<x86> Arch, unsigned Imm8, class... Args>
  void gf2p8affineinvqb_maskz(Args...) = delete;

/// \}
}
#endif
