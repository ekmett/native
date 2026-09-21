// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstddef>
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vbmi;

#if NATIVE_TEST_WIDTH == 512
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi");
constexpr auto storage_arch = native::target_features<native::x86>("avx512f");
using register_type = __m512i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi"
#elif NATIVE_TEST_WIDTH == 256
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("avx");
using register_type = __m256i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi,avx512vl"
#elif NATIVE_TEST_WIDTH == 128
constexpr auto full_arch = native::target_features<native::x86>("avx512f,avx512bw,avx512vbmi,avx512vl");
constexpr auto storage_arch = native::target_features<native::x86>("sse2");
using register_type = __m128i;
#define NATIVE_TEST_FULL_TARGET "avx512f,avx512bw,avx512vbmi,avx512vl"
#else
#error NATIVE_TEST_WIDTH must be 128, 256 or 512
#endif

#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512bw"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vbmi"
#elif NATIVE_TEST_TARGET == 4
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#elif NATIVE_TEST_TARGET == 5
#define NATIVE_TEST_CALLER_TARGET "sse2,no-avx,no-avx512f,no-avx512bw,no-avx512vbmi,no-avx512vl"
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
constexpr std::size_t qlanes = NATIVE_TEST_WIDTH / 64;

#if NATIVE_TEST_SHAPE == 1
using bvector = native::simd<std::int8_t, blanes, arch>;
using qvector = native::simd<std::int64_t, qlanes, arch>;
#elif NATIVE_TEST_SHAPE == 2
using bvector = native::simd<std::uint8_t, blanes, other_arch>;
using qvector = native::simd<std::uint64_t, qlanes, other_arch>;
#elif NATIVE_TEST_SHAPE == 3
using bvector = register_type;
using qvector = register_type;
#elif NATIVE_TEST_SHAPE == 4
using bvector = native::simd<std::uint32_t, NATIVE_TEST_WIDTH / 32, arch>;
using qvector = native::simd<std::uint8_t, blanes, arch>;
#else
using bvector = native::simd<std::uint8_t, blanes, arch>;
using qvector = native::simd<std::uint64_t, qlanes, arch>;
#endif

#if NATIVE_TEST_MASK == 1
using bmask = unsigned;
#elif NATIVE_TEST_MASK == 2
using bmask = native::predicate<blanes / 2, arch>;
#elif NATIVE_TEST_MASK == 3
using bmask = native::predicate<blanes, other_arch>;
#else
using bmask = native::predicate<blanes, arch>;
#endif


#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpermb(bvector indices, bvector value) noexcept {
  return native::vpermb<arch>(indices, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpermb(bvector source, bmask mask, bvector indices, bvector value) noexcept {
  return native::mask_vpermb<arch>(source, mask, indices, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpermb(bmask mask, bvector indices, bvector value) noexcept {
  return native::maskz_vpermb<arch>(mask, indices, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpermt2b(bvector a, bvector indices, bvector b) noexcept {
  return native::vpermt2b<arch>(a, indices, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpermt2b(bvector a, bmask mask, bvector indices, bvector b) noexcept {
  return native::mask_vpermt2b<arch>(a, mask, indices, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpermt2b(bmask mask, bvector a, bvector indices, bvector b) noexcept {
  return native::maskz_vpermt2b<arch>(mask, a, indices, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 6) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpermi2b(bvector indices, bvector a, bvector b) noexcept {
  return native::vpermi2b<arch>(indices, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 7)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpermi2b(bvector indices, bmask mask, bvector a, bvector b) noexcept {
  return native::mask_vpermi2b<arch>(indices, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 8)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpermi2b(bmask mask, bvector indices, bvector a, bvector b) noexcept {
  return native::maskz_vpermi2b<arch>(mask, indices, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 9) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_vpmultishiftqb(bvector control, qvector value) noexcept {
  return native::vpmultishiftqb<arch>(control, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 10)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_vpmultishiftqb(bvector source, bmask mask, bvector control, qvector value) noexcept {
  return native::mask_vpmultishiftqb<arch>(source, mask, control, value);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 11)
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_vpmultishiftqb(bmask mask, bvector control, qvector value) noexcept {
  return native::maskz_vpmultishiftqb<arch>(mask, control, value);
}
#endif
