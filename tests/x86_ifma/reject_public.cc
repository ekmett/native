// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.ifma;
#if NATIVE_TEST_FORM == 0
#define NATIVE_TEST_FULL_TARGET "avxifma"
#define NATIVE_TEST_WIDTH 128
#define NATIVE_TEST_HAS_MASK 0
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avxifma"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_FORM == 1
#define NATIVE_TEST_FULL_TARGET "avxifma"
#define NATIVE_TEST_WIDTH 256
#define NATIVE_TEST_HAS_MASK 0
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avxifma"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_FORM == 2
#define NATIVE_TEST_FULL_TARGET "avx512ifma,avx512vl"
#define NATIVE_TEST_WIDTH 128
#define NATIVE_TEST_HAS_MASK 1
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512ifma"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_FORM == 3
#define NATIVE_TEST_FULL_TARGET "avx512ifma,avx512vl"
#define NATIVE_TEST_WIDTH 256
#define NATIVE_TEST_HAS_MASK 1
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512ifma"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#elif NATIVE_TEST_TARGET == 3
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512vl"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#elif NATIVE_TEST_FORM == 4
#define NATIVE_TEST_FULL_TARGET "avx512ifma"
#define NATIVE_TEST_WIDTH 512
#define NATIVE_TEST_HAS_MASK 1
#if NATIVE_TEST_TARGET == 1
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512ifma"
#elif NATIVE_TEST_TARGET == 2
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET ",no-avx512f"
#else
#define NATIVE_TEST_CALLER_TARGET NATIVE_TEST_FULL_TARGET
#endif
#endif
constexpr auto full_arch = native::target_features<native::x86>(NATIVE_TEST_FULL_TARGET);
#if NATIVE_TEST_WEAK
#if NATIVE_TEST_WIDTH == 128
constexpr auto arch = native::target_features<native::x86>("sse2");
#elif NATIVE_TEST_WIDTH == 256
constexpr auto arch = native::target_features<native::x86>("avx");
#else
constexpr auto arch = native::target_features<native::x86>("avx512f");
#endif
#else
constexpr auto arch = full_arch;
#endif
constexpr unsigned lanes = NATIVE_TEST_WIDTH / 64;
#if NATIVE_TEST_SHAPE == 1
#if NATIVE_TEST_WIDTH == 128
using vector = __m128i;
#elif NATIVE_TEST_WIDTH == 256
using vector = __m256i;
#else
using vector = __m512i;
#endif
#elif NATIVE_TEST_SHAPE == 2
using vector = native::simd<std::int64_t, lanes, arch>;
#elif NATIVE_TEST_SHAPE == 3
using vector = native::simd<std::uint64_t, lanes, arch & native::x86_feature::gfni>;
#elif NATIVE_TEST_SHAPE == 4
using vector = native::simd<std::uint32_t, lanes * 2, arch>;
#else
using vector = native::simd<std::uint64_t, lanes, arch>;
#endif
#if NATIVE_TEST_MASK == 1
using mask_type = std::uint64_t;
#elif NATIVE_TEST_MASK == 2
using mask_type = native::predicate<lanes / 2, arch>;
#elif NATIVE_TEST_MASK == 3
using mask_type = native::predicate<lanes, arch & native::x86_feature::gfni>;
#else
using mask_type = native::predicate<lanes, arch>;
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 0) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_madd52lo(vector accumulator, vector a, vector b) noexcept {
  return native::madd52lo<arch>(accumulator, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 1) && NATIVE_TEST_HAS_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_madd52lo(vector accumulator, vector a, vector b, mask_type mask) noexcept {
  return native::mask_madd52lo<arch>(accumulator, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 2) && NATIVE_TEST_HAS_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_madd52lo(vector accumulator, vector a, vector b, mask_type mask) noexcept {
  return native::maskz_madd52lo<arch>(mask, accumulator, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 3) && !NATIVE_TEST_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_madd52hi(vector accumulator, vector a, vector b) noexcept {
  return native::madd52hi<arch>(accumulator, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 4) && NATIVE_TEST_HAS_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_mask_madd52hi(vector accumulator, vector a, vector b, mask_type mask) noexcept {
  return native::mask_madd52hi<arch>(accumulator, mask, a, b);
}
#endif

#if (!defined(NATIVE_TEST_OPERATION) || NATIVE_TEST_OPERATION == 5) && NATIVE_TEST_HAS_MASK
native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_maskz_madd52hi(vector accumulator, vector a, vector b, mask_type mask) noexcept {
  return native::maskz_madd52hi<arch>(mask, accumulator, a, b);
}
#endif
