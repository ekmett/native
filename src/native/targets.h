// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

// Textual macros only: this header can precede or follow `import native;`.
// Its expansions require the exported native ISA metadata. Clang is intentional.
#ifndef __clang__
#error native source target variants require Clang
#endif

// Register a custom source name with one literal, for example:
//   #define NATIVE_TARGET_compact "avx2,f16c"
// The same literal drives compiler attributes, ISA values and admission.
// Only positive feature names in native's registry are accepted; CPU names and
// arbitrary C++ aliases cannot be reverse-engineered into compiler attributes.
// The empty literal gives scalar metadata; its scope below deliberately applies
// no target attribute. Clang reserves "default" for function multiversioning.
#define NATIVE_TARGET_scalar ""
#define NATIVE_TARGET_avx2 "avx2,fma"
#define NATIVE_TARGET_avx512 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl"
#define NATIVE_TARGET_avx512_bf16 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"
#define NATIVE_TARGET_avx512_fp16 "avx2,fma,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"
#define NATIVE_TARGET_neon "neon"
#define NATIVE_TARGET_neon_fp16 "neon,fullfp16"
#define NATIVE_TARGET_neon_bf16 "neon,bf16"

#define NATIVE_DETAIL_TARGET_CAT_I(a,b) a##b
#define NATIVE_DETAIL_TARGET_CAT(a,b) NATIVE_DETAIL_TARGET_CAT_I(a,b)
#define NATIVE_DETAIL_TARGET_STRING_I(...) #__VA_ARGS__
#define NATIVE_DETAIL_TARGET_STRING(...) NATIVE_DETAIL_TARGET_STRING_I(__VA_ARGS__)
#define NATIVE_DETAIL_TARGET_PRAGMA(...) _Pragma(NATIVE_DETAIL_TARGET_STRING(__VA_ARGS__))
#define NATIVE_TARGET_STRING(name) NATIVE_DETAIL_TARGET_CAT(NATIVE_TARGET_,name)
#define NATIVE_TARGET_ISA(name) \
  (::native::detail::source_isa<::native::target_features(NATIVE_TARGET_STRING(name))>)

// The named pragma stack prevents accidentally popping an unrelated user's
// clang attribute stack. This does not generate preprocessor #include lines.
// Generate outside other ISA target scopes: Clang combines nested requirements,
// which predefines cannot observe. Intentional nesting must record the outer
// requirements in NATIVE_TARGET_EXTRA_MINIMUM for admission.
#define NATIVE_DETAIL_TARGET_SECOND(a,b,...) b
#define NATIVE_DETAIL_TARGET_PROBE() unused,1
#define NATIVE_DETAIL_TARGET_SCALAR_scalar NATIVE_DETAIL_TARGET_PROBE()
#define NATIVE_DETAIL_TARGET_IS_SCALAR_I(...) NATIVE_DETAIL_TARGET_SECOND(__VA_ARGS__,0)
#define NATIVE_DETAIL_TARGET_IS_SCALAR(name) \
  NATIVE_DETAIL_TARGET_IS_SCALAR_I(NATIVE_DETAIL_TARGET_CAT(NATIVE_DETAIL_TARGET_SCALAR_,name))
#define NATIVE_DETAIL_TARGET_PUSH_0(name) \
  NATIVE_DETAIL_TARGET_PRAGMA(clang attribute native_source_target.push(__attribute__((target(NATIVE_TARGET_STRING(name)))), apply_to=function))
#define NATIVE_DETAIL_TARGET_PUSH_1(name) \
  NATIVE_DETAIL_TARGET_PRAGMA(clang attribute native_source_target.push)
#define NATIVE_TARGET_PUSH(name) \
  NATIVE_DETAIL_TARGET_CAT(NATIVE_DETAIL_TARGET_PUSH_,NATIVE_DETAIL_TARGET_IS_SCALAR(name))(name)
#define NATIVE_TARGET_POP() NATIVE_DETAIL_TARGET_PRAGMA(clang attribute native_source_target.pop)

// A list uses X(name, ...), forwarding its extra arguments to X:
//   #define targets(X,...) X(avx512,__VA_ARGS__) X(avx2,__VA_ARGS__)
// A reusable body owns the complete function declaration and definition:
//   #define body(name,ISA) template<::native::isa A> requires(A==ISA) void name(float * p) { /* ... */ }
//   NATIVE_TARGET_VARIANTS(kernel,targets,body)
// Each chosen ISA constrains a distinct function template. List feature sets
// must be unique. Definitions need normal C++ ODR rules.
#define NATIVE_DETAIL_TARGET_EMIT(tag,name,body) \
  NATIVE_TARGET_PUSH(tag) \
  body(name,NATIVE_TARGET_ISA(tag)) \
  NATIVE_TARGET_POP()
#define NATIVE_TARGET_VARIANTS(name,list,body) list(NATIVE_DETAIL_TARGET_EMIT,name,body)

// Snapshot inherited translation-unit features separately from the requested
// ISA value. A target attribute adds features; it does not erase the project
// minimum. An unregistered stronger minimum is rejected instead of under-admitted.
#ifdef __MMX__
#define NATIVE_DETAIL_MIN_MMX (::native::isa(::native::x86_feature::mmx))
#else
#define NATIVE_DETAIL_MIN_MMX (::native::isa{})
#endif
#ifdef __SSE__
#define NATIVE_DETAIL_MIN_SSE (::native::isa(::native::x86_feature::sse))
#else
#define NATIVE_DETAIL_MIN_SSE (::native::isa{})
#endif
#ifdef __SSE2__
#define NATIVE_DETAIL_MIN_SSE2 (::native::isa(::native::x86_feature::sse2))
#else
#define NATIVE_DETAIL_MIN_SSE2 (::native::isa{})
#endif
#ifdef __SSE3__
#define NATIVE_DETAIL_MIN_SSE3 (::native::isa(::native::x86_feature::sse3))
#else
#define NATIVE_DETAIL_MIN_SSE3 (::native::isa{})
#endif
#ifdef __SSSE3__
#define NATIVE_DETAIL_MIN_SSSE3 (::native::isa(::native::x86_feature::ssse3))
#else
#define NATIVE_DETAIL_MIN_SSSE3 (::native::isa{})
#endif
#ifdef __SSE4_1__
#define NATIVE_DETAIL_MIN_SSE41 (::native::isa(::native::x86_feature::sse41))
#else
#define NATIVE_DETAIL_MIN_SSE41 (::native::isa{})
#endif
#ifdef __SSE4_2__
#define NATIVE_DETAIL_MIN_SSE42 (::native::isa(::native::x86_feature::sse42))
#else
#define NATIVE_DETAIL_MIN_SSE42 (::native::isa{})
#endif
#ifdef __POPCNT__
#define NATIVE_DETAIL_MIN_POPCNT (::native::isa(::native::x86_feature::popcnt))
#else
#define NATIVE_DETAIL_MIN_POPCNT (::native::isa{})
#endif
#ifdef __AVX__
#define NATIVE_DETAIL_MIN_AVX (::native::isa(::native::x86_feature::avx))
#else
#define NATIVE_DETAIL_MIN_AVX (::native::isa{})
#endif
#ifdef __AVX2__
#define NATIVE_DETAIL_MIN_AVX2 (::native::isa(::native::x86_feature::avx2))
#else
#define NATIVE_DETAIL_MIN_AVX2 (::native::isa{})
#endif
#ifdef __FMA__
#define NATIVE_DETAIL_MIN_FMA (::native::isa(::native::x86_feature::fma))
#else
#define NATIVE_DETAIL_MIN_FMA (::native::isa{})
#endif
#ifdef __F16C__
#define NATIVE_DETAIL_MIN_F16C (::native::isa(::native::x86_feature::f16c))
#else
#define NATIVE_DETAIL_MIN_F16C (::native::isa{})
#endif
#ifdef __BMI__
#define NATIVE_DETAIL_MIN_BMI1 (::native::isa(::native::x86_feature::bmi1))
#else
#define NATIVE_DETAIL_MIN_BMI1 (::native::isa{})
#endif
#ifdef __BMI2__
#define NATIVE_DETAIL_MIN_BMI2 (::native::isa(::native::x86_feature::bmi2))
#else
#define NATIVE_DETAIL_MIN_BMI2 (::native::isa{})
#endif
#ifdef __AVX512F__
#define NATIVE_DETAIL_MIN_AVX512F (::native::isa(::native::x86_feature::avx512f))
#else
#define NATIVE_DETAIL_MIN_AVX512F (::native::isa{})
#endif
#ifdef __AVX512DQ__
#define NATIVE_DETAIL_MIN_AVX512DQ (::native::isa(::native::x86_feature::avx512dq))
#else
#define NATIVE_DETAIL_MIN_AVX512DQ (::native::isa{})
#endif
#ifdef __AVX512BW__
#define NATIVE_DETAIL_MIN_AVX512BW (::native::isa(::native::x86_feature::avx512bw))
#else
#define NATIVE_DETAIL_MIN_AVX512BW (::native::isa{})
#endif
#ifdef __AVX512VL__
#define NATIVE_DETAIL_MIN_AVX512VL (::native::isa(::native::x86_feature::avx512vl))
#else
#define NATIVE_DETAIL_MIN_AVX512VL (::native::isa{})
#endif
#ifdef __AVX512BF16__
#define NATIVE_DETAIL_MIN_AVX512BF16 (::native::isa(::native::x86_feature::avx512bf16))
#else
#define NATIVE_DETAIL_MIN_AVX512BF16 (::native::isa{})
#endif
#ifdef __AVX512FP16__
#define NATIVE_DETAIL_MIN_AVX512FP16 (::native::isa(::native::x86_feature::avx512fp16))
#else
#define NATIVE_DETAIL_MIN_AVX512FP16 (::native::isa{})
#endif
#ifdef __ARM_NEON
#define NATIVE_DETAIL_MIN_NEON (::native::isa(::native::arm_feature::neon))
#else
#define NATIVE_DETAIL_MIN_NEON (::native::isa{})
#endif
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) || defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC)
#define NATIVE_DETAIL_MIN_NEON_FP16 (::native::isa(::native::arm_feature::neon_fp16))
#else
#define NATIVE_DETAIL_MIN_NEON_FP16 (::native::isa{})
#endif
#ifdef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
#define NATIVE_DETAIL_MIN_NEON_BF16 (::native::isa(::native::arm_feature::neon_bf16))
#else
#define NATIVE_DETAIL_MIN_NEON_BF16 (::native::isa{})
#endif
#ifdef __AES__
#define NATIVE_DETAIL_MIN_AES (::native::isa(::native::x86_feature::aes))
#else
#define NATIVE_DETAIL_MIN_AES (::native::isa{})
#endif
#ifdef __PCLMUL__
#define NATIVE_DETAIL_MIN_PCLMUL (::native::isa(::native::x86_feature::pclmul))
#else
#define NATIVE_DETAIL_MIN_PCLMUL (::native::isa{})
#endif
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) && (defined(__x86_64__) || defined(_M_X64))
#define NATIVE_DETAIL_MIN_CX16 (::native::isa(::native::x86_feature::cx16))
#else
#define NATIVE_DETAIL_MIN_CX16 (::native::isa{})
#endif
#ifdef __AVX512CD__
#define NATIVE_DETAIL_MIN_AVX512CD (::native::isa(::native::x86_feature::avx512cd))
#else
#define NATIVE_DETAIL_MIN_AVX512CD (::native::isa{})
#endif
#ifdef __AVX512IFMA__
#define NATIVE_DETAIL_MIN_AVX512IFMA (::native::isa(::native::x86_feature::avx512ifma))
#else
#define NATIVE_DETAIL_MIN_AVX512IFMA (::native::isa{})
#endif

#ifdef __LZCNT__
#define NATIVE_DETAIL_MIN_LZCNT (::native::isa(::native::x86_feature::lzcnt))
#else
#define NATIVE_DETAIL_MIN_LZCNT (::native::isa{})
#endif
#ifdef __MOVBE__
#define NATIVE_DETAIL_MIN_MOVBE (::native::isa(::native::x86_feature::movbe))
#else
#define NATIVE_DETAIL_MIN_MOVBE (::native::isa{})
#endif
#ifdef __LAHF_SAHF__
#define NATIVE_DETAIL_MIN_SAHF (::native::isa(::native::x86_feature::sahf))
#else
#define NATIVE_DETAIL_MIN_SAHF (::native::isa{})
#endif
#ifdef __MWAITX__
#define NATIVE_DETAIL_MIN_MWAITX (::native::isa(::native::x86_feature::mwaitx))
#else
#define NATIVE_DETAIL_MIN_MWAITX (::native::isa{})
#endif
#ifdef __WAITPKG__
#define NATIVE_DETAIL_MIN_WAITPKG (::native::isa(::native::x86_feature::waitpkg))
#else
#define NATIVE_DETAIL_MIN_WAITPKG (::native::isa{})
#endif
#ifdef __CRC32__
#define NATIVE_DETAIL_MIN_CRC32 (::native::isa(::native::x86_feature::crc32))
#else
#define NATIVE_DETAIL_MIN_CRC32 (::native::isa{})
#endif
#ifdef __ARM_FEATURE_AES
#define NATIVE_DETAIL_MIN_ARM_AES (::native::isa(::native::arm_feature::aes))
#else
#define NATIVE_DETAIL_MIN_ARM_AES (::native::isa{})
#endif
#ifdef __ARM_FEATURE_SHA2
#define NATIVE_DETAIL_MIN_ARM_SHA2 (::native::isa(::native::arm_feature::sha2))
#else
#define NATIVE_DETAIL_MIN_ARM_SHA2 (::native::isa{})
#endif
#if defined(__ARM_FEATURE_SHA3) || defined(__ARM_FEATURE_SHA512)
#define NATIVE_DETAIL_MIN_ARM_SHA3 (::native::isa(::native::arm_feature::sha3))
#else
#define NATIVE_DETAIL_MIN_ARM_SHA3 (::native::isa{})
#endif
#ifdef __ARM_FEATURE_CRC32
#define NATIVE_DETAIL_MIN_ARM_CRC (::native::isa(::native::arm_feature::crc))
#else
#define NATIVE_DETAIL_MIN_ARM_CRC (::native::isa{})
#endif
#ifdef __ARM_FEATURE_ATOMICS
#define NATIVE_DETAIL_MIN_ARM_LSE (::native::isa(::native::arm_feature::lse))
#else
#define NATIVE_DETAIL_MIN_ARM_LSE (::native::isa{})
#endif
#ifdef __ARM_FEATURE_QRDMX
#define NATIVE_DETAIL_MIN_ARM_RDM (::native::isa(::native::arm_feature::rdm))
#else
#define NATIVE_DETAIL_MIN_ARM_RDM (::native::isa{})
#endif
#ifdef __ARM_FEATURE_FP16_FML
#define NATIVE_DETAIL_MIN_ARM_FP16FML (::native::isa(::native::arm_feature::fp16fml))
#else
#define NATIVE_DETAIL_MIN_ARM_FP16FML (::native::isa{})
#endif
#ifdef __ARM_FEATURE_DOTPROD
#define NATIVE_DETAIL_MIN_ARM_DOTPROD (::native::isa(::native::arm_feature::dotprod))
#else
#define NATIVE_DETAIL_MIN_ARM_DOTPROD (::native::isa{})
#endif
#ifdef __ARM_FEATURE_COMPLEX
#define NATIVE_DETAIL_MIN_ARM_COMPLEX (::native::isa(::native::arm_feature::complxnum))
#else
#define NATIVE_DETAIL_MIN_ARM_COMPLEX (::native::isa{})
#endif
#ifdef __ARM_FEATURE_JCVT
#define NATIVE_DETAIL_MIN_ARM_JSCVT (::native::isa(::native::arm_feature::jsconv))
#else
#define NATIVE_DETAIL_MIN_ARM_JSCVT (::native::isa{})
#endif
#ifdef __ARM_FEATURE_RCPC
#define NATIVE_DETAIL_MIN_ARM_RCPC (::native::isa(::native::arm_feature::rcpc))
#else
#define NATIVE_DETAIL_MIN_ARM_RCPC (::native::isa{})
#endif
#ifdef __ARM_FEATURE_PAUTH
#define NATIVE_DETAIL_MIN_ARM_PAUTH (::native::isa(::native::arm_feature::pauth))
#else
#define NATIVE_DETAIL_MIN_ARM_PAUTH (::native::isa{})
#endif

#if defined(__SSE4A__) || defined(__XOP__) || defined(__FMA4__) || \
    defined(__AVX512VBMI__) || defined(__AVX512VBMI2__) || defined(__AVX512VNNI__) || \
    defined(__AVX512BITALG__) || defined(__AVX512VPOPCNTDQ__) || defined(__AVX512VP2INTERSECT__) || \
    defined(__GFNI__) || defined(__VAES__) || defined(__VPCLMULQDQ__) || defined(__AVXVNNI__) || \
    defined(__AVXIFMA__) || defined(__AVXNECONVERT__) || defined(__AVXVNNIINT8__) || \
    defined(__AVXVNNIINT16__) || defined(__AMX_TILE__) || defined(__AMX_INT8__) || defined(__AMX_BF16__) || \
    defined(__ARM_FEATURE_SVE) || defined(__ARM_FEATURE_MATMUL_INT8) || defined(__ARM_FEATURE_SME)
#define NATIVE_DETAIL_MIN_UNREGISTERED (::native::target_features("unregistered"))
#else
#define NATIVE_DETAIL_MIN_UNREGISTERED (::native::isa{})
#endif

// Optional additional project contract. This is an ISA value/expression (for
// example ::native::target_features("avx2,f16c")), never a CPU-name guess. The ordinary
// executable's minimum remains a startup precondition: the selector itself must
// already be safe to run. Compiler CPU models can enable features not represented
// by predefines; this macro is not an inference engine for arbitrary -mcpu flags.
#ifndef NATIVE_TARGET_EXTRA_MINIMUM
#define NATIVE_TARGET_EXTRA_MINIMUM (::native::isa{})
#endif
#define NATIVE_TARGET_MINIMUM (::native::feature_closure(NATIVE_TARGET_EXTRA_MINIMUM& \
  NATIVE_DETAIL_MIN_MMX&NATIVE_DETAIL_MIN_SSE&NATIVE_DETAIL_MIN_SSE2&NATIVE_DETAIL_MIN_SSE3& \
  NATIVE_DETAIL_MIN_SSSE3&NATIVE_DETAIL_MIN_SSE41&NATIVE_DETAIL_MIN_SSE42&NATIVE_DETAIL_MIN_POPCNT& \
  NATIVE_DETAIL_MIN_AVX&NATIVE_DETAIL_MIN_AVX2&NATIVE_DETAIL_MIN_FMA&NATIVE_DETAIL_MIN_F16C& \
  NATIVE_DETAIL_MIN_BMI1&NATIVE_DETAIL_MIN_BMI2&NATIVE_DETAIL_MIN_AVX512F&NATIVE_DETAIL_MIN_AVX512DQ& \
  NATIVE_DETAIL_MIN_AVX512BW&NATIVE_DETAIL_MIN_AVX512VL&NATIVE_DETAIL_MIN_AVX512BF16&NATIVE_DETAIL_MIN_AVX512FP16& \
  NATIVE_DETAIL_MIN_NEON&NATIVE_DETAIL_MIN_NEON_FP16&NATIVE_DETAIL_MIN_NEON_BF16& \
  NATIVE_DETAIL_MIN_AES&NATIVE_DETAIL_MIN_PCLMUL&NATIVE_DETAIL_MIN_CX16&NATIVE_DETAIL_MIN_AVX512CD& \
  NATIVE_DETAIL_MIN_AVX512IFMA&NATIVE_DETAIL_MIN_LZCNT&NATIVE_DETAIL_MIN_MOVBE&NATIVE_DETAIL_MIN_SAHF& \
  NATIVE_DETAIL_MIN_MWAITX&NATIVE_DETAIL_MIN_WAITPKG&NATIVE_DETAIL_MIN_CRC32& \
  NATIVE_DETAIL_MIN_ARM_AES&NATIVE_DETAIL_MIN_ARM_SHA2&NATIVE_DETAIL_MIN_ARM_SHA3&NATIVE_DETAIL_MIN_ARM_CRC& \
  NATIVE_DETAIL_MIN_ARM_LSE&NATIVE_DETAIL_MIN_ARM_RDM&NATIVE_DETAIL_MIN_ARM_FP16FML&NATIVE_DETAIL_MIN_ARM_DOTPROD& \
  NATIVE_DETAIL_MIN_ARM_COMPLEX&NATIVE_DETAIL_MIN_ARM_JSCVT&NATIVE_DETAIL_MIN_ARM_RCPC&NATIVE_DETAIL_MIN_ARM_PAUTH& \
  NATIVE_DETAIL_MIN_UNREGISTERED))

#define NATIVE_DETAIL_TARGET_LIST(tag,...) \
  +::native::isa_list<::native::target_entry{NATIVE_TARGET_ISA(tag),NATIVE_TARGET_MINIMUM}>{}
#define NATIVE_TARGET_LIST(list) (::native::isa_list<>{} list(NATIVE_DETAIL_TARGET_LIST,unused))
