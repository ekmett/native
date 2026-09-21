// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_HOST_X86 || defined(NATIVE_DOXYGEN)
export namespace native {
/** \defgroup x86_vnni VNNI integer dot products
 * Each 32-bit accumulator lane receives the sum of four byte products or two
 * word products from the corresponding adjacent input group. Non-saturating
 * forms wrap modulo 2^32. Saturating forms clamp the complete sum, including
 * the accumulator, without first wrapping or clamping the product sum.
 * All saturation is signed except dpbuuds and dpwuuds, which use unsigned
 * accumulators and unsigned saturation. These register operations do not
 * change integer flags or floating-point status.
 *
 * Unmasked 128/256-bit core operations use AVX-VNNI when Arch contains
 * avxvnni; otherwise they require AVX512F, AVX512VL and AVX512VNNI.
 * 512-bit core operations require AVX512F and AVX512VNNI. Masked forms always
 * require those EVEX features, with AVX512VL for 128/256 bits.
 * INT8/INT16 extensions have only unmasked 128/256-bit forms and require
 * their own independent feature. Compiler prerequisite closure and CPU/OS
 * admission are separate from the exact instruction constraints below.
 *
 * Mask bit i selects 32-bit result lane i. Merge forms retain accumulator
 * lanes; zero forms clear inactive lanes. Bits above the lane count are
 * ignored. Callers must enable a compatible target and admit its CPU and OS
 * state requirements before execution. There is no runtime dispatch.
 * \{ */

  // 128-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 4, Arch> dpbusd(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> dpbusd(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> mask_dpbusd(simd<std::int32_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::mask_dpbusd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> maskz_dpbusd(predicate<4, Arch> mask, simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::maskz_dpbusd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 4, Arch> dpbusds(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> dpbusds(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> mask_dpbusds(simd<std::int32_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::mask_dpbusds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> maskz_dpbusds(predicate<4, Arch> mask, simd<std::int32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::maskz_dpbusds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 4, Arch> dpwssd(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> dpwssd(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> mask_dpwssd(simd<std::int32_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::mask_dpwssd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> maskz_dpwssd(predicate<4, Arch> mask, simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::maskz_dpwssd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 4, Arch> dpwssds(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> dpwssds(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> mask_dpwssds(simd<std::int32_t, 4, Arch> accumulator, predicate<4, Arch> mask, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::mask_dpwssds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 4, Arch> maskz_dpwssds(predicate<4, Arch> mask, simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::maskz_dpwssds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  // 256-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 8, Arch> dpbusd(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> dpbusd(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> mask_dpbusd(simd<std::int32_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::mask_dpbusd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> maskz_dpbusd(predicate<8, Arch> mask, simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::maskz_dpbusd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 8, Arch> dpbusds(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> dpbusds(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> mask_dpbusds(simd<std::int32_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::mask_dpbusds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> maskz_dpbusds(predicate<8, Arch> mask, simd<std::int32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::maskz_dpbusds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 8, Arch> dpwssd(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> dpwssd(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> mask_dpwssd(simd<std::int32_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::mask_dpwssd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> maskz_dpwssd(predicate<8, Arch> mask, simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::maskz_dpwssd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX-VNNI.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnni))
  native_nodiscard native_inline native_const native_target("avxvnni")
  simd<std::int32_t, 8, Arch> dpwssds(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation. Uses AVX512VNNI.
  template<isa Arch> requires(!Arch.has(x86_feature::avxvnni) &&
      Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) && Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> dpwssds(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> mask_dpwssds(simd<std::int32_t, 8, Arch> accumulator, predicate<8, Arch> mask, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::mask_dpwssds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni) &&
      Arch.has(x86_feature::avx512vl))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni,avx512vl")
  simd<std::int32_t, 8, Arch> maskz_dpwssds(predicate<8, Arch> mask, simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::maskz_dpwssds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  // 512-bit core operations.
  /// Accumulate products of four unsigned bytes from a and signed bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> dpbusd(simd<std::int32_t, 16, Arch> accumulator, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::dpbusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> mask_dpbusd(simd<std::int32_t, 16, Arch> accumulator, predicate<16, Arch> mask, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::mask_dpbusd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> maskz_dpbusd(predicate<16, Arch> mask, simd<std::int32_t, 16, Arch> accumulator, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::maskz_dpbusd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and signed bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> dpbusds(simd<std::int32_t, 16, Arch> accumulator, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::dpbusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> mask_dpbusds(simd<std::int32_t, 16, Arch> accumulator, predicate<16, Arch> mask, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::mask_dpbusds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpbusds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> maskz_dpbusds(predicate<16, Arch> mask, simd<std::int32_t, 16, Arch> accumulator, simd<std::uint8_t, 64, Arch> a, simd<std::int8_t, 64, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::maskz_dpbusds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> dpwssd(simd<std::int32_t, 16, Arch> accumulator, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::dpwssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> mask_dpwssd(simd<std::int32_t, 16, Arch> accumulator, predicate<16, Arch> mask, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::mask_dpwssd<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssd in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> maskz_dpwssd(predicate<16, Arch> mask, simd<std::int32_t, 16, Arch> accumulator, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::maskz_dpwssd<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> dpwssds(simd<std::int32_t, 16, Arch> accumulator, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::dpwssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes retain accumulator.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> mask_dpwssds(simd<std::int32_t, 16, Arch> accumulator, predicate<16, Arch> mask, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::mask_dpwssds<Arch>(accumulator.to_native(), mask.to_bitset(), a.to_native(), b.to_native()));
  }

  /// dpwssds in active lanes; inactive lanes become zero.
  template<isa Arch> requires(Arch.has(x86_feature::avx512f) && Arch.has(x86_feature::avx512vnni))
  native_nodiscard native_inline native_const native_target("avx512f,avx512vnni")
  simd<std::int32_t, 16, Arch> maskz_dpwssds(predicate<16, Arch> mask, simd<std::int32_t, 16, Arch> accumulator, simd<std::int16_t, 32, Arch> a, simd<std::int16_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 16, Arch>::from_native(detail::x86_vnni::maskz_dpwssds<Arch>(mask.to_bitset(), accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 4, Arch> dpbssd(simd<std::int32_t, 4, Arch> accumulator, simd<std::int8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 8, Arch> dpbssd(simd<std::int32_t, 8, Arch> accumulator, simd<std::int8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbssd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 4, Arch> dpbssds(simd<std::int32_t, 4, Arch> accumulator, simd<std::int8_t, 16, Arch> a, simd<std::int8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 8, Arch> dpbssds(simd<std::int32_t, 8, Arch> accumulator, simd<std::int8_t, 32, Arch> a, simd<std::int8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbssds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 4, Arch> dpbsud(simd<std::int32_t, 4, Arch> accumulator, simd<std::int8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbsud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 8, Arch> dpbsud(simd<std::int32_t, 8, Arch> accumulator, simd<std::int8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbsud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 4, Arch> dpbsuds(simd<std::int32_t, 4, Arch> accumulator, simd<std::int8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpbsuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four signed bytes from a and unsigned bytes from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::int32_t, 8, Arch> dpbsuds(simd<std::int32_t, 8, Arch> accumulator, simd<std::int8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpbsuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::uint32_t, 4, Arch> dpbuud(simd<std::uint32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vnni::dpbuud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::uint32_t, 8, Arch> dpbuud(simd<std::uint32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vnni::dpbuud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::uint32_t, 4, Arch> dpbuuds(simd<std::uint32_t, 4, Arch> accumulator, simd<std::uint8_t, 16, Arch> a, simd<std::uint8_t, 16, Arch> b) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vnni::dpbuuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of four unsigned bytes from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint8))
  native_nodiscard native_inline native_const native_target("avxvnniint8")
  simd<std::uint32_t, 8, Arch> dpbuuds(simd<std::uint32_t, 8, Arch> accumulator, simd<std::uint8_t, 32, Arch> a, simd<std::uint8_t, 32, Arch> b) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vnni::dpbuuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and unsigned words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 4, Arch> dpwsud(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwsud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and unsigned words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 8, Arch> dpwsud(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::uint16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwsud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and unsigned words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 4, Arch> dpwsuds(simd<std::int32_t, 4, Arch> accumulator, simd<std::int16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwsuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two signed words from a and unsigned words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 8, Arch> dpwsuds(simd<std::int32_t, 8, Arch> accumulator, simd<std::int16_t, 16, Arch> a, simd<std::uint16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwsuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and signed words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 4, Arch> dpwusd(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and signed words from b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 8, Arch> dpwusd(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwusd<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and signed words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 4, Arch> dpwusds(simd<std::int32_t, 4, Arch> accumulator, simd<std::uint16_t, 8, Arch> a, simd<std::int16_t, 8, Arch> b) noexcept {
    return simd<std::int32_t, 4, Arch>::from_native(detail::x86_vnni::dpwusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and signed words from b, with signed 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::int32_t, 8, Arch> dpwusds(simd<std::int32_t, 8, Arch> accumulator, simd<std::uint16_t, 16, Arch> a, simd<std::int16_t, 16, Arch> b) noexcept {
    return simd<std::int32_t, 8, Arch>::from_native(detail::x86_vnni::dpwusds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::uint32_t, 4, Arch> dpwuud(simd<std::uint32_t, 4, Arch> accumulator, simd<std::uint16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vnni::dpwuud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and b, modulo 2^32.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::uint32_t, 8, Arch> dpwuud(simd<std::uint32_t, 8, Arch> accumulator, simd<std::uint16_t, 16, Arch> a, simd<std::uint16_t, 16, Arch> b) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vnni::dpwuud<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::uint32_t, 4, Arch> dpwuuds(simd<std::uint32_t, 4, Arch> accumulator, simd<std::uint16_t, 8, Arch> a, simd<std::uint16_t, 8, Arch> b) noexcept {
    return simd<std::uint32_t, 4, Arch>::from_native(detail::x86_vnni::dpwuuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  /// Accumulate products of two unsigned words from a and b, with unsigned 32-bit saturation.
  template<isa Arch> requires(Arch.has(x86_feature::avxvnniint16))
  native_nodiscard native_inline native_const native_target("avxvnniint16")
  simd<std::uint32_t, 8, Arch> dpwuuds(simd<std::uint32_t, 8, Arch> accumulator, simd<std::uint16_t, 16, Arch> a, simd<std::uint16_t, 16, Arch> b) noexcept {
    return simd<std::uint32_t, 8, Arch>::from_native(detail::x86_vnni::dpwuuds<Arch>(accumulator.to_native(), a.to_native(), b.to_native()));
  }

  // Reject implicit register conversions, mixed tags and wrong element types.
  template<isa Arch, class... Args>
  void dpbusd(Args...) = delete;
  template<isa Arch, class... Args>
  void mask_dpbusd(Args...) = delete;
  template<isa Arch, class... Args>
  void maskz_dpbusd(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbusds(Args...) = delete;
  template<isa Arch, class... Args>
  void mask_dpbusds(Args...) = delete;
  template<isa Arch, class... Args>
  void maskz_dpbusds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwssd(Args...) = delete;
  template<isa Arch, class... Args>
  void mask_dpwssd(Args...) = delete;
  template<isa Arch, class... Args>
  void maskz_dpwssd(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwssds(Args...) = delete;
  template<isa Arch, class... Args>
  void mask_dpwssds(Args...) = delete;
  template<isa Arch, class... Args>
  void maskz_dpwssds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbssd(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbssds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbsud(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbsuds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbuud(Args...) = delete;
  template<isa Arch, class... Args>
  void dpbuuds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwsud(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwsuds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwusd(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwusds(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwuud(Args...) = delete;
  template<isa Arch, class... Args>
  void dpwuuds(Args...) = delete;

/// \}

}
#endif
