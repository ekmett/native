// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.bitalg;

#if NATIVE_TEST_WIDTH == 512
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg");
constexpr auto storage_arch = native::target_features<native::x86>("avx512f");
using register_type = __m512i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512bitalg"
#elif NATIVE_TEST_WIDTH == 256
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("avx");
using register_type = __m256i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512bitalg,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512bitalg,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512bitalg,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512bw"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512bitalg"
#elif NATIVE_TEST_TARGET == 4
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#elif NATIVE_TEST_TARGET == 5
#define NATIVE_TEST_CALLER_TARGET "sse2,no-avx,no-avx512f,no-avx512bw,no-avx512bitalg,no-avx512vl"
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
constexpr std::size_t wlanes = NATIVE_TEST_WIDTH / 16;
constexpr std::size_t qlanes = NATIVE_TEST_WIDTH / 64;

#if NATIVE_TEST_SHAPE == 1
using bvector = native::simd<std::int8_t, blanes, arch>;
using wvector = native::simd<std::int16_t, wlanes, arch>;
using qvector = native::simd<std::int64_t, qlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using bvector = native::simd<std::uint8_t, blanes, other_arch>;
using wvector = native::simd<std::uint16_t, wlanes, other_arch>;
using qvector = native::simd<std::uint64_t, qlanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using bvector = register_type;
using wvector = register_type;
using qvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using bvector = native::simd<std::uint32_t, NATIVE_TEST_WIDTH / 32, arch>;
using wvector = native::simd<std::uint32_t, NATIVE_TEST_WIDTH / 32, arch>;
using qvector = native::simd<std::uint8_t, blanes, arch>;
#else
using bvector = native::simd<std::uint8_t, blanes, arch>;
using wvector = native::simd<std::uint16_t, wlanes, arch>;
using qvector = native::simd<std::uint64_t, qlanes, arch>;
#endif

#if NATIVE_TEST_MASK == 1
using bmask = unsigned;
using wmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using bmask = native::predicate<blanes / 2, arch>;
using wmask = native::predicate<wlanes / 2, arch>;
#elif NATIVE_TEST_MASK == 3
using bmask = native::predicate<blanes, other_arch>;
using wmask = native::predicate<wlanes, other_arch>;
#else
using bmask = native::predicate<blanes, arch>;
using wmask = native::predicate<wlanes, arch>;
#endif

// One operation per target-mismatch translation unit reaches Clang code generation.

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpopcntb(bvector value) noexcept {
  return native::vpopcntb<arch>(value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpopcntb(bvector source, bmask mask, bvector value) noexcept {
  return native::mask_vpopcntb<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpopcntb(bmask mask, bvector value) noexcept {
  return native::maskz_vpopcntb<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpopcntw(wvector value) noexcept {
  return native::vpopcntw<arch>(value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpopcntw(wvector source, wmask mask, wvector value) noexcept {
  return native::mask_vpopcntw<arch>(source, mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpopcntw(wmask mask, wvector value) noexcept {
  return native::maskz_vpopcntw<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpshufbitqmb(qvector value, bvector control) noexcept {
  return native::vpshufbitqmb<arch>(value, control);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpshufbitqmb(bmask mask, qvector value, bvector control) noexcept {
  return native::mask_vpshufbitqmb<arch>(mask, value, control);
}
#endif
