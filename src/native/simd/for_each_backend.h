// Repeated source inclusion. Every function receives its family target.
#define NATIVE_BACKEND scalar_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET (::native::abi_lookup<::native::scalar,::native::detail::raw_kernel_policies>::index)
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#define NATIVE_DEFAULT_ARCH ::native::scalar
#define NATIVE_HAS_AVX2 0
#define NATIVE_HAS_AVX512F 0
#define NATIVE_HAS_AVX512DQ 0
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 0
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_0));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND avx2_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 4
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 4)
#define NATIVE_DEFAULT_ARCH ::native::avx2
#define NATIVE_HAS_AVX2 1
#define NATIVE_HAS_AVX512F 0
#define NATIVE_HAS_AVX512DQ 0
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_1))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_1));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND avx512_nobw_novl_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 3
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 3)
#define NATIVE_DEFAULT_ARCH ::native::detail::kernel_base
#define NATIVE_HAS_AVX2 1
#define NATIVE_HAS_AVX512F 1
#define NATIVE_HAS_AVX512DQ 1
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_2))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_2));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND avx512_bw_novl_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 1
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 1)
#define NATIVE_DEFAULT_ARCH ::native::detail::kernel_bw
#define NATIVE_HAS_AVX2 1
#define NATIVE_HAS_AVX512F 1
#define NATIVE_HAS_AVX512DQ 1
#define NATIVE_HAS_AVX512BW 1
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_3))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_3));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND avx512_nobw_vl_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 2
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 2)
#define NATIVE_DEFAULT_ARCH ::native::detail::kernel_vl
#define NATIVE_HAS_AVX2 1
#define NATIVE_HAS_AVX512F 1
#define NATIVE_HAS_AVX512DQ 1
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 1
#define NATIVE_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_4))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_4));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_X86 && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND avx512_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 0
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 0)
#define NATIVE_DEFAULT_ARCH ::native::avx512
#define NATIVE_HAS_AVX2 1
#define NATIVE_HAS_AVX512F 1
#define NATIVE_HAS_AVX512DQ 1
#define NATIVE_HAS_AVX512BW 1
#define NATIVE_HAS_AVX512VL 1
#define NATIVE_HAS_ARM_NEON 0
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_5))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_5));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 3)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_NEON && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND neon_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 0
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 0)
#define NATIVE_DEFAULT_ARCH ::native::neon
#define NATIVE_HAS_AVX2 0
#define NATIVE_HAS_AVX512F 0
#define NATIVE_HAS_AVX512DQ 0
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 1
#pragma clang attribute push(__attribute__((target(NATIVE_KERNEL_TARGET_20))), apply_to=function)
static_assert(::native::abi_lookup<NATIVE_DEFAULT_ARCH,::native::detail::raw_kernel_policies>::index == NATIVE_RAW_TARGET);
static_assert(NATIVE_DEFAULT_ARCH==::native::target_features(NATIVE_KERNEL_TARGET_20));
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::memory_kernel_policies>::index == 3)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#endif

#if NATIVE_HOST_WASM && (!defined(NATIVE_PROFILE) || NATIVE_PROFILE != 0)
#define NATIVE_BACKEND wasm_backend
#define NATIVE_BACKEND_NAMESPACE native::detail::NATIVE_BACKEND
#define NATIVE_RAW_TARGET 0
#define NATIVE_ARCH_REQUIRES(A) (::native::abi_lookup<A,::native::detail::raw_kernel_policies>::index == 0)
#define NATIVE_DEFAULT_ARCH (::native::feature_closure(::native::wasm_feature::simd128))
#define NATIVE_HAS_AVX2 0
#define NATIVE_HAS_AVX512F 0
#define NATIVE_HAS_AVX512DQ 0
#define NATIVE_HAS_AVX512BW 0
#define NATIVE_HAS_AVX512VL 0
#define NATIVE_HAS_ARM_NEON 0
#define NATIVE_HAS_WASM_SIMD128 1
#pragma clang attribute push(__attribute__((target("simd128"))), apply_to=function)
#define NATIVE_COMMON_ARCH(...) (::native::abi_lookup<__VA_ARGS__,::native::detail::raw_kernel_policies>::index == 0)
#include NATIVE_BACKEND_BODY
#undef NATIVE_COMMON_ARCH
#pragma clang attribute pop
#undef NATIVE_BACKEND
#undef NATIVE_BACKEND_NAMESPACE
#undef NATIVE_ARCH_REQUIRES
#undef NATIVE_RAW_TARGET
#undef NATIVE_DEFAULT_ARCH
#undef NATIVE_HAS_AVX2
#undef NATIVE_HAS_AVX512F
#undef NATIVE_HAS_AVX512DQ
#undef NATIVE_HAS_AVX512BW
#undef NATIVE_HAS_AVX512VL
#undef NATIVE_HAS_ARM_NEON
#undef NATIVE_HAS_WASM_SIMD128
#endif
