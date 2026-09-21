// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.avx512cd;

#if NATIVE_TEST_WIDTH == 512
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512cd");
constexpr auto storage_arch = native::target_features<native::x86>("avx512f");
using register_type = __m512i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512cd"
#elif NATIVE_TEST_WIDTH == 256
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512cd,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("avx");
using register_type = __m256i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512cd,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512cd,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512cd,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512cd"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#elif NATIVE_TEST_TARGET == 4
#define NATIVE_TEST_CALLER_TARGET "sse2,no-avx,no-avx512f,no-avx512cd,no-avx512vl"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif

#if NATIVE_TEST_WEAK
constexpr auto arch = storage_arch;
#else
constexpr auto arch = full_arch;
#endif
constexpr auto other_arch = full_arch & native::x86_feature::gfni;
constexpr std::size_t dlanes = NATIVE_TEST_WIDTH / 32;
constexpr std::size_t qlanes = NATIVE_TEST_WIDTH / 64;

#if NATIVE_TEST_SHAPE == 1
using dvector = native::simd<std::int32_t, dlanes, arch>;
using qvector = native::simd<std::int64_t, qlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using dvector = native::simd<float, dlanes, arch>;
using qvector = native::simd<double, qlanes, arch>;
#elif NATIVE_TEST_SHAPE == 3
using dvector = register_type;
using qvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using dvector = native::simd<std::uint32_t, dlanes, other_arch>;
using qvector = native::simd<std::uint64_t, qlanes, other_arch>;
#else
using dvector = native::simd<std::uint32_t, dlanes, arch>;
using qvector = native::simd<std::uint64_t, qlanes, arch>;
#endif

#if NATIVE_TEST_MASK == 1
using dmask = unsigned;
using qmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using dmask = native::predicate<dlanes * 2, arch>;
using qmask = native::predicate<qlanes * 2, arch>;
#elif NATIVE_TEST_MASK == 3
using dmask = native::predicate<dlanes, other_arch>;
using qmask = native::predicate<qlanes, other_arch>;
#else
using dmask = native::predicate<dlanes, arch>;
using qmask = native::predicate<qlanes, arch>;
#endif

// Target failures use one operation per translation unit because Clang stops
// code generation after its first target mismatch. Other diagnostics are semantic.
#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpconflictd(dvector value) noexcept {
  return native::vpconflictd<arch>(value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpconflictd(dvector source, dmask mask, dvector value) noexcept {
  return native::mask_vpconflictd<arch>(source, mask, value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpconflictd(dmask mask, dvector value) noexcept {
  return native::maskz_vpconflictd<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vplzcntd(dvector value) noexcept {
  return native::vplzcntd<arch>(value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vplzcntd(dvector source, dmask mask, dvector value) noexcept {
  return native::mask_vplzcntd<arch>(source, mask, value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vplzcntd(dmask mask, dvector value) noexcept {
  return native::maskz_vplzcntd<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpconflictq(qvector value) noexcept {
  return native::vpconflictq<arch>(value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpconflictq(qvector source, qmask mask, qvector value) noexcept {
  return native::mask_vpconflictq<arch>(source, mask, value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 8
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpconflictq(qmask mask, qvector value) noexcept {
  return native::maskz_vpconflictq<arch>(mask, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 9) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vplzcntq(qvector value) noexcept {
  return native::vplzcntq<arch>(value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 10
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vplzcntq(qvector source, qmask mask, qvector value) noexcept {
  return native::mask_vplzcntq<arch>(source, mask, value);
}
#endif

#if !defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 11
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vplzcntq(qmask mask, qvector value) noexcept {
  return native::maskz_vplzcntq<arch>(mask, value);
}
#endif
