// Repeated source inclusion. Every function receives its family target.
#define SIMD_BACKEND scalar_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_scalar_architecture
#define SIMD_DEFAULT_ARCH ::simd::scalar
#define SIMD_HAS_AVX2 0
#define SIMD_HAS_AVX512F 0
#define SIMD_HAS_AVX512DQ 0
#define SIMD_HAS_AVX512BW 0
#define SIMD_HAS_AVX512VL 0
#define SIMD_HAS_ARM_NEON 0
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_0));
#define SIMD_COMMON_ARCH_CONCEPT SIMD_ARCH_CONCEPT
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND avx2_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_avx2_architecture
#define SIMD_DEFAULT_ARCH ::simd::avx2
#define SIMD_HAS_AVX2 1
#define SIMD_HAS_AVX512F 0
#define SIMD_HAS_AVX512DQ 0
#define SIMD_HAS_AVX512BW 0
#define SIMD_HAS_AVX512VL 0
#define SIMD_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_1))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_1));
#define SIMD_COMMON_ARCH_CONCEPT SIMD_ARCH_CONCEPT
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND avx512_nobw_novl_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_base_architecture
#define SIMD_DEFAULT_ARCH ::simd::isa<::simd::avx2::features | ::simd::feature::avx512f | ::simd::feature::avx512dq>
#define SIMD_HAS_AVX2 1
#define SIMD_HAS_AVX512F 1
#define SIMD_HAS_AVX512DQ 1
#define SIMD_HAS_AVX512BW 0
#define SIMD_HAS_AVX512VL 0
#define SIMD_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_2))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_2));
#define SIMD_COMMON_ARCH_CONCEPT SIMD_ARCH_CONCEPT
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND avx512_bw_novl_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_bw_architecture
#define SIMD_DEFAULT_ARCH ::simd::isa<::simd::avx2::features | ::simd::feature::avx512f | ::simd::feature::avx512dq | ::simd::feature::avx512bw>
#define SIMD_HAS_AVX2 1
#define SIMD_HAS_AVX512F 1
#define SIMD_HAS_AVX512DQ 1
#define SIMD_HAS_AVX512BW 1
#define SIMD_HAS_AVX512VL 0
#define SIMD_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_3))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_3));
#define SIMD_COMMON_ARCH_CONCEPT SIMD_ARCH_CONCEPT
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND avx512_nobw_vl_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_vl_architecture
#define SIMD_DEFAULT_ARCH ::simd::isa<::simd::avx2::features | ::simd::feature::avx512f | ::simd::feature::avx512dq | ::simd::feature::avx512vl>
#define SIMD_HAS_AVX2 1
#define SIMD_HAS_AVX512F 1
#define SIMD_HAS_AVX512DQ 1
#define SIMD_HAS_AVX512BW 0
#define SIMD_HAS_AVX512VL 1
#define SIMD_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_4))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_4));
#define SIMD_COMMON_ARCH_CONCEPT SIMD_ARCH_CONCEPT
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND avx512_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_avx512_architecture
#define SIMD_DEFAULT_ARCH ::simd::isa<::simd::avx2::features | ::simd::feature::avx512f | ::simd::feature::avx512dq | ::simd::feature::avx512bw | ::simd::feature::avx512vl>
#define SIMD_HAS_AVX2 1
#define SIMD_HAS_AVX512F 1
#define SIMD_HAS_AVX512DQ 1
#define SIMD_HAS_AVX512BW 1
#define SIMD_HAS_AVX512VL 1
#define SIMD_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_5))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_5));
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_avx512_architecture
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif

#if SIMD_HOST_NEON && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
#define SIMD_BACKEND neon_backend
#define SIMD_BACKEND_NAMESPACE simd::detail::SIMD_BACKEND
#define SIMD_ARCH_CONCEPT ::simd::detail::kernel_neon_architecture
#define SIMD_DEFAULT_ARCH ::simd::neon
#define SIMD_HAS_AVX2 0
#define SIMD_HAS_AVX512F 0
#define SIMD_HAS_AVX512DQ 0
#define SIMD_HAS_AVX512BW 0
#define SIMD_HAS_AVX512VL 0
#define SIMD_HAS_ARM_NEON 1
#pragma clang attribute push(__attribute__((target(SIMD_KERNEL_TARGET_20))), apply_to=function)
static_assert(SIMD_ARCH_CONCEPT<SIMD_DEFAULT_ARCH>);
static_assert(SIMD_DEFAULT_ARCH::features==::simd::target_features(SIMD_KERNEL_TARGET_20));
#define SIMD_COMMON_ARCH_CONCEPT ::simd::detail::memory_neon_architecture
#include SIMD_BACKEND_BODY
#undef SIMD_COMMON_ARCH_CONCEPT
#pragma clang attribute pop
#undef SIMD_BACKEND
#undef SIMD_BACKEND_NAMESPACE
#undef SIMD_ARCH_CONCEPT
#undef SIMD_DEFAULT_ARCH
#undef SIMD_HAS_AVX2
#undef SIMD_HAS_AVX512F
#undef SIMD_HAS_AVX512DQ
#undef SIMD_HAS_AVX512BW
#undef SIMD_HAS_AVX512VL
#undef SIMD_HAS_ARM_NEON
#endif
