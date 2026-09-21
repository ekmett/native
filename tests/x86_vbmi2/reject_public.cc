// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vbmi2;

#if NATIVE_TEST_WIDTH == 512
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi2");
constexpr auto storage_arch = native::target_features<native::x86>("avx512f");
using register_type = __m512i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi2"
#elif NATIVE_TEST_WIDTH == 256
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi2,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("avx");
using register_type = __m256i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi2,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi2,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi2,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512bw"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vbmi2"
#elif NATIVE_TEST_TARGET == 4
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#elif NATIVE_TEST_TARGET == 5
#define NATIVE_TEST_CALLER_TARGET "sse2,no-avx,no-avx512f,no-avx512bw,no-avx512vbmi2,no-avx512vl"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif

#if NATIVE_TEST_WEAK
constexpr auto arch = storage_arch;
#else
constexpr auto arch = full_arch;
#endif
constexpr auto other_arch = full_arch & native::x86_feature::gfni;
constexpr std::size_t blanes = NATIVE_TEST_WIDTH / 8;
#if NATIVE_TEST_SHAPE == 1
using bvector = native::simd<std::int8_t, blanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using bvector = native::simd<std::uint8_t, blanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using bvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using bvector = native::simd<std::uint32_t, NATIVE_TEST_WIDTH / 32, arch>;
#else
using bvector = native::simd<std::uint8_t, blanes, arch>;
#endif
#if NATIVE_TEST_MASK == 1
using bmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using bmask = native::predicate<blanes - 1, arch>;
#elif NATIVE_TEST_MASK == 3
using bmask = native::predicate<blanes, other_arch>;
#else
using bmask = native::predicate<blanes, arch>;
#endif
constexpr std::size_t wlanes = NATIVE_TEST_WIDTH / 16;
#if NATIVE_TEST_SHAPE == 1
using wvector = native::simd<std::int16_t, wlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using wvector = native::simd<std::uint16_t, wlanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using wvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using wvector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch>;
#else
using wvector = native::simd<std::uint16_t, wlanes, arch>;
#endif
#if NATIVE_TEST_MASK == 1
using wmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using wmask = native::predicate<wlanes - 1, arch>;
#elif NATIVE_TEST_MASK == 3
using wmask = native::predicate<wlanes, other_arch>;
#else
using wmask = native::predicate<wlanes, arch>;
#endif
constexpr std::size_t dlanes = NATIVE_TEST_WIDTH / 32;
#if NATIVE_TEST_SHAPE == 1
using dvector = native::simd<std::int32_t, dlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using dvector = native::simd<std::uint32_t, dlanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using dvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using dvector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch>;
#else
using dvector = native::simd<std::uint32_t, dlanes, arch>;
#endif
#if NATIVE_TEST_MASK == 1
using dmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using dmask = native::predicate<dlanes - 1, arch>;
#elif NATIVE_TEST_MASK == 3
using dmask = native::predicate<dlanes, other_arch>;
#else
using dmask = native::predicate<dlanes, arch>;
#endif
constexpr std::size_t qlanes = NATIVE_TEST_WIDTH / 64;
#if NATIVE_TEST_SHAPE == 1
using qvector = native::simd<std::int64_t, qlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using qvector = native::simd<std::uint64_t, qlanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using qvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using qvector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch>;
#else
using qvector = native::simd<std::uint64_t, qlanes, arch>;
#endif
#if NATIVE_TEST_MASK == 1
using qmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using qmask = native::predicate<qlanes - 1, arch>;
#elif NATIVE_TEST_MASK == 3
using qmask = native::predicate<qlanes, other_arch>;
#else
using qmask = native::predicate<qlanes, arch>;
#endif
#if NATIVE_TEST_POINTER == 1
using bpointer = void *;
#elif NATIVE_TEST_POINTER == 2
using bpointer = std::uint32_t *;
#elif NATIVE_TEST_POINTER == 3
using bpointer = std::uint8_t volatile *;
#elif NATIVE_TEST_POINTER == 4
using bpointer = std::uint8_t const *;
#else
using bpointer = std::uint8_t *;
#endif
#if NATIVE_TEST_POINTER == 1
using wpointer = void *;
#elif NATIVE_TEST_POINTER == 2
using wpointer = std::uint32_t *;
#elif NATIVE_TEST_POINTER == 3
using wpointer = std::uint16_t volatile *;
#elif NATIVE_TEST_POINTER == 4
using wpointer = std::uint16_t const *;
#else
using wpointer = std::uint16_t *;
#endif
#if NATIVE_TEST_IMMEDIATE == 1
constexpr unsigned shift_count = 256;
#elif NATIVE_TEST_IMMEDIATE == 2
constexpr unsigned shift_count = unsigned(-1);
#else
constexpr unsigned shift_count = 7;
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpcompressb_merge(
  bvector source,
  bmask mask,
  bvector value) noexcept {
  return native::mask_vpcompressb<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpcompressb_zero(
  bmask mask,
  bvector value) noexcept {
  return native::maskz_vpcompressb<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2) && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpcompressb_store(
  bpointer destination,
  bmask mask,
  bvector value) noexcept {
  return native::mask_vpcompressb<arch>(destination, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpexpandb_merge(
  bvector source,
  bmask mask,
  bvector value) noexcept {
  return native::mask_vpexpandb<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpexpandb_zero(
  bmask mask,
  bvector value) noexcept {
  return native::maskz_vpexpandb<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5) && NATIVE_TEST_POINTER != 4 && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpexpandb_load_merge(
  bvector source,
  bmask mask,
  bpointer memory) noexcept {
  return native::mask_vpexpandb<arch>(source, mask, memory);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6) && !NATIVE_TEST_SHAPE && \
    NATIVE_TEST_POINTER != 4 && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpexpandb_load_zero(
  bmask mask,
  bpointer memory) noexcept {
  return native::maskz_vpexpandb<arch>(mask, memory);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpcompressw_merge(
  wvector source,
  wmask mask,
  wvector value) noexcept {
  return native::mask_vpcompressw<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 8) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpcompressw_zero(
  wmask mask,
  wvector value) noexcept {
  return native::maskz_vpcompressw<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 9) && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpcompressw_store(
  wpointer destination,
  wmask mask,
  wvector value) noexcept {
  return native::mask_vpcompressw<arch>(destination, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 10) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpexpandw_merge(
  wvector source,
  wmask mask,
  wvector value) noexcept {
  return native::mask_vpexpandw<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 11) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpexpandw_zero(
  wmask mask,
  wvector value) noexcept {
  return native::maskz_vpexpandw<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 12) && NATIVE_TEST_POINTER != 4 && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpexpandw_load_merge(
  wvector source,
  wmask mask,
  wpointer memory) noexcept {
  return native::mask_vpexpandw<arch>(source, mask, memory);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 13) && !NATIVE_TEST_SHAPE && \
    NATIVE_TEST_POINTER != 4 && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpexpandw_load_zero(
  wmask mask,
  wpointer memory) noexcept {
  return native::maskz_vpexpandw<arch>(mask, memory);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 14) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldw_plain(
  wvector a,
  wvector b) noexcept {
  return native::vpshldw<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 15) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldw_merge(
  wvector source,
  wmask mask,
  wvector a,
  wvector b) noexcept {
  return native::mask_vpshldw<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 16) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldw_zero(
  wmask mask,
  wvector a,
  wvector b) noexcept {
  return native::maskz_vpshldw<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 17) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldvw_plain(
  wvector a,
  wvector b,
  wvector counts) noexcept {
  return native::vpshldvw<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 18) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldvw_merge(
  wvector a,
  wmask mask,
  wvector b,
  wvector counts) noexcept {
  return native::mask_vpshldvw<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 19) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldvw_zero(
  wmask mask,
  wvector a,
  wvector b,
  wvector counts) noexcept {
  return native::maskz_vpshldvw<arch>(mask, a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 20) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdw_plain(
  wvector a,
  wvector b) noexcept {
  return native::vpshrdw<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 21) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdw_merge(
  wvector source,
  wmask mask,
  wvector a,
  wvector b) noexcept {
  return native::mask_vpshrdw<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 22) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdw_zero(
  wmask mask,
  wvector a,
  wvector b) noexcept {
  return native::maskz_vpshrdw<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 23) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdvw_plain(
  wvector a,
  wvector b,
  wvector counts) noexcept {
  return native::vpshrdvw<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 24) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdvw_merge(
  wvector a,
  wmask mask,
  wvector b,
  wvector counts) noexcept {
  return native::mask_vpshrdvw<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 25) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdvw_zero(
  wmask mask,
  wvector a,
  wvector b,
  wvector counts) noexcept {
  return native::maskz_vpshrdvw<arch>(mask, a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 26) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldd_plain(
  dvector a,
  dvector b) noexcept {
  return native::vpshldd<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 27) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldd_merge(
  dvector source,
  dmask mask,
  dvector a,
  dvector b) noexcept {
  return native::mask_vpshldd<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 28) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldd_zero(
  dmask mask,
  dvector a,
  dvector b) noexcept {
  return native::maskz_vpshldd<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 29) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldvd_plain(
  dvector a,
  dvector b,
  dvector counts) noexcept {
  return native::vpshldvd<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 30) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldvd_merge(
  dvector a,
  dmask mask,
  dvector b,
  dvector counts) noexcept {
  return native::mask_vpshldvd<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 31) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldvd_zero(
  dmask mask,
  dvector a,
  dvector b,
  dvector counts) noexcept {
  return native::maskz_vpshldvd<arch>(mask, a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 32) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdd_plain(
  dvector a,
  dvector b) noexcept {
  return native::vpshrdd<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 33) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdd_merge(
  dvector source,
  dmask mask,
  dvector a,
  dvector b) noexcept {
  return native::mask_vpshrdd<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 34) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdd_zero(
  dmask mask,
  dvector a,
  dvector b) noexcept {
  return native::maskz_vpshrdd<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 35) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdvd_plain(
  dvector a,
  dvector b,
  dvector counts) noexcept {
  return native::vpshrdvd<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 36) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdvd_merge(
  dvector a,
  dmask mask,
  dvector b,
  dvector counts) noexcept {
  return native::mask_vpshrdvd<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 37) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdvd_zero(
  dmask mask,
  dvector a,
  dvector b,
  dvector counts) noexcept {
  return native::maskz_vpshrdvd<arch>(mask, a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 38) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldq_plain(
  qvector a,
  qvector b) noexcept {
  return native::vpshldq<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 39) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldq_merge(
  qvector source,
  qmask mask,
  qvector a,
  qvector b) noexcept {
  return native::mask_vpshldq<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 40) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldq_zero(
  qmask mask,
  qvector a,
  qvector b) noexcept {
  return native::maskz_vpshldq<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 41) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshldvq_plain(
  qvector a,
  qvector b,
  qvector counts) noexcept {
  return native::vpshldvq<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 42) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshldvq_merge(
  qvector a,
  qmask mask,
  qvector b,
  qvector counts) noexcept {
  return native::mask_vpshldvq<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 43) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshldvq_zero(
  qmask mask,
  qvector a,
  qvector b,
  qvector counts) noexcept {
  return native::maskz_vpshldvq<arch>(mask, a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 44) && !NATIVE_TEST_MASK && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdq_plain(
  qvector a,
  qvector b) noexcept {
  return native::vpshrdq<arch, shift_count>(a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 45) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdq_merge(
  qvector source,
  qmask mask,
  qvector a,
  qvector b) noexcept {
  return native::mask_vpshrdq<arch, shift_count>(source, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 46) && !NATIVE_TEST_POINTER
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdq_zero(
  qmask mask,
  qvector a,
  qvector b) noexcept {
  return native::maskz_vpshrdq<arch, shift_count>(mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 47) && !NATIVE_TEST_MASK && \
    !NATIVE_TEST_POINTER && !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshrdvq_plain(
  qvector a,
  qvector b,
  qvector counts) noexcept {
  return native::vpshrdvq<arch>(a, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 48) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshrdvq_merge(
  qvector a,
  qmask mask,
  qvector b,
  qvector counts) noexcept {
  return native::mask_vpshrdvq<arch>(a, mask, b, counts);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 49) && !NATIVE_TEST_POINTER && \
    !NATIVE_TEST_IMMEDIATE
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpshrdvq_zero(
  qmask mask,
  qvector a,
  qvector b,
  qvector counts) noexcept {
  return native::maskz_vpshrdvq<arch>(mask, a, b, counts);
}
#endif
