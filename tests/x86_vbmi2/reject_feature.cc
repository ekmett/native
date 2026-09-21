// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <type_traits>
#include <native/x86/vbmi2.h>

#if NATIVE_TEST_WIDTH == 512
using register_type = __m512i;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi2"
#elif NATIVE_TEST_WIDTH == 256
using register_type = __m256i;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi2,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
using register_type = __m128i;
#define NATIVE_TEST_CALLER_TARGET "avx512f,avx512bw,avx512vbmi2,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

// Structural feature omissions never change the caller's full compiler target.
constexpr auto requirements = [] {
  auto value = native::x86_feature::avx512f & native::x86_feature::avx512bw &
    native::x86_feature::avx512vbmi2;
#if NATIVE_TEST_WIDTH != 512
  value.set(native::x86_feature::avx512vl, true);
#endif
#if NATIVE_TEST_FEATURE == 1
  value.set(native::x86_feature::avx512f, false);
#elif NATIVE_TEST_FEATURE == 2
  value.set(native::x86_feature::avx512bw, false);
#elif NATIVE_TEST_FEATURE == 3
  value.set(native::x86_feature::avx512vbmi2, false);
#elif NATIVE_TEST_FEATURE == 4
  value.set(native::x86_feature::avx512vl, false);
#endif
  return value;
}();

using bmask = std::conditional_t<(NATIVE_TEST_WIDTH / 8 <= 8), __mmask8,
  std::conditional_t<(NATIVE_TEST_WIDTH / 8 <= 16), __mmask16,
  std::conditional_t<(NATIVE_TEST_WIDTH / 8 <= 32), __mmask32, __mmask64>>>;
using wmask = std::conditional_t<(NATIVE_TEST_WIDTH / 16 <= 8), __mmask8,
  std::conditional_t<(NATIVE_TEST_WIDTH / 16 <= 16), __mmask16,
  std::conditional_t<(NATIVE_TEST_WIDTH / 16 <= 32), __mmask32, __mmask64>>>;
using dmask = std::conditional_t<(NATIVE_TEST_WIDTH / 32 <= 8), __mmask8,
  std::conditional_t<(NATIVE_TEST_WIDTH / 32 <= 16), __mmask16,
  std::conditional_t<(NATIVE_TEST_WIDTH / 32 <= 32), __mmask32, __mmask64>>>;
using qmask = std::conditional_t<(NATIVE_TEST_WIDTH / 64 <= 8), __mmask8,
  std::conditional_t<(NATIVE_TEST_WIDTH / 64 <= 16), __mmask16,
  std::conditional_t<(NATIVE_TEST_WIDTH / 64 <= 32), __mmask32, __mmask64>>>;

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpcompressb_merge(
  register_type source,
  bmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressb<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpcompressb_zero(
  bmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressb<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
void reject_mask_vpcompressb_store(
  void * destination,
  bmask mask,
  register_type value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressb<requirements>(destination, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpexpandb_merge(
  register_type source,
  bmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpexpandb_zero(
  bmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandb<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpexpandb_load_merge(
  register_type source,
  bmask mask,
  void const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandb<requirements>(source, mask, memory);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpexpandb_load_zero(
  bmask mask,
  void const * memory) noexcept {
#if NATIVE_TEST_WIDTH == 128
  return native::detail::x86_vbmi2::maskz_vpexpandb_128<requirements>(mask, memory);
#elif NATIVE_TEST_WIDTH == 256
  return native::detail::x86_vbmi2::maskz_vpexpandb_256<requirements>(mask, memory);
#else
  return native::detail::x86_vbmi2::maskz_vpexpandb_512<requirements>(mask, memory);
#endif
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpcompressw_merge(
  register_type source,
  wmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::mask_vpcompressw<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpcompressw_zero(
  wmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpcompressw<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
void reject_mask_vpcompressw_store(
  void * destination,
  wmask mask,
  register_type value) noexcept {
  native::detail::x86_vbmi2::mask_vpcompressw<requirements>(destination, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpexpandw_merge(
  register_type source,
  wmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<requirements>(source, mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpexpandw_zero(
  wmask mask,
  register_type value) noexcept {
  return native::detail::x86_vbmi2::maskz_vpexpandw<requirements>(mask, value);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpexpandw_load_merge(
  register_type source,
  wmask mask,
  void const * memory) noexcept {
  return native::detail::x86_vbmi2::mask_vpexpandw<requirements>(source, mask, memory);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpexpandw_load_zero(
  wmask mask,
  void const * memory) noexcept {
#if NATIVE_TEST_WIDTH == 128
  return native::detail::x86_vbmi2::maskz_vpexpandw_128<requirements>(mask, memory);
#elif NATIVE_TEST_WIDTH == 256
  return native::detail::x86_vbmi2::maskz_vpexpandw_256<requirements>(mask, memory);
#else
  return native::detail::x86_vbmi2::maskz_vpexpandw_512<requirements>(mask, memory);
#endif
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldw_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshldw<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldw_merge(
  register_type source,
  wmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldw<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldw_zero(
  wmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldw<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldvw_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvw<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldvw_merge(
  register_type a,
  wmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvw<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldvw_zero(
  wmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvw<requirements>(mask, a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdw_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshrdw<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdw_merge(
  register_type source,
  wmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdw<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdw_zero(
  wmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdw<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdvw_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvw<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdvw_merge(
  register_type a,
  wmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvw<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdvw_zero(
  wmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvw<requirements>(mask, a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldd_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshldd<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldd_merge(
  register_type source,
  dmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldd<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldd_zero(
  dmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldd<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldvd_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvd<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldvd_merge(
  register_type a,
  dmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvd<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldvd_zero(
  dmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvd<requirements>(mask, a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdd_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshrdd<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdd_merge(
  register_type source,
  dmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdd<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdd_zero(
  dmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdd<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdvd_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvd<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdvd_merge(
  register_type a,
  dmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvd<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdvd_zero(
  dmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvd<requirements>(mask, a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldq_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshldq<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldq_merge(
  register_type source,
  qmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldq<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldq_zero(
  qmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldq<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshldvq_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshldvq<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshldvq_merge(
  register_type a,
  qmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshldvq<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshldvq_zero(
  qmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshldvq<requirements>(mask, a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdq_plain(
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::vpshrdq<requirements, 7>(a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdq_merge(
  register_type source,
  qmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdq<requirements, 7>(source, mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdq_zero(
  qmask mask,
  register_type a,
  register_type b) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdq<requirements, 7>(mask, a, b);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_vpshrdvq_plain(
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::vpshrdvq<requirements>(a, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_mask_vpshrdvq_merge(
  register_type a,
  qmask mask,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::mask_vpshrdvq<requirements>(a, mask, b, counts);
}

extern "C" native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
register_type reject_maskz_vpshrdvq_zero(
  qmask mask,
  register_type a,
  register_type b,
  register_type counts) noexcept {
  return native::detail::x86_vbmi2::maskz_vpshrdvq<requirements>(mask, a, b, counts);
}
