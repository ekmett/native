// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

// Textual macros only: this header can precede or follow `import simd;`.
// Its expansions require the exported simd ISA metadata. Clang is intentional.
#ifndef __clang__
#error simd source target variants require Clang
#endif

// Register a custom source name with one literal, for example:
//   #define SIMD_TARGET_compact "avx2,f16c"
// The same literal drives compiler attributes, canonical tags and admission.
// Only positive feature names in simd's registry are accepted; CPU names and
// arbitrary C++ aliases cannot be reverse-engineered into compiler attributes.
// The empty literal gives scalar metadata; its scope below deliberately applies
// no target attribute. Clang reserves "default" for function multiversioning.
#define SIMD_TARGET_scalar ""
#define SIMD_TARGET_avx2 "avx2,fma,bmi2"
#define SIMD_TARGET_avx512 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl"
#define SIMD_TARGET_avx512_bf16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512bf16"
#define SIMD_TARGET_avx512_fp16 "avx2,fma,bmi2,avx512f,avx512dq,avx512bw,avx512vl,avx512fp16"
#define SIMD_TARGET_neon "neon"
#define SIMD_TARGET_neon_fp16 "neon,fullfp16"
#define SIMD_TARGET_neon_bf16 "neon,bf16"

#define SIMD_DETAIL_TARGET_CAT_I(a,b) a##b
#define SIMD_DETAIL_TARGET_CAT(a,b) SIMD_DETAIL_TARGET_CAT_I(a,b)
#define SIMD_DETAIL_TARGET_STRING_I(...) #__VA_ARGS__
#define SIMD_DETAIL_TARGET_STRING(...) SIMD_DETAIL_TARGET_STRING_I(__VA_ARGS__)
#define SIMD_DETAIL_TARGET_PRAGMA(...) _Pragma(SIMD_DETAIL_TARGET_STRING(__VA_ARGS__))
#define SIMD_TARGET_STRING(name) SIMD_DETAIL_TARGET_CAT(SIMD_TARGET_,name)
#define SIMD_TARGET_TYPE(name) ::simd::isa<::simd::target_features(SIMD_TARGET_STRING(name))>

// The named pragma stack prevents accidentally popping an unrelated user's
// clang attribute stack. This does not generate preprocessor #include lines.
#define SIMD_DETAIL_TARGET_SECOND(a,b,...) b
#define SIMD_DETAIL_TARGET_PROBE() unused,1
#define SIMD_DETAIL_TARGET_SCALAR_scalar SIMD_DETAIL_TARGET_PROBE()
#define SIMD_DETAIL_TARGET_IS_SCALAR_I(...) SIMD_DETAIL_TARGET_SECOND(__VA_ARGS__,0)
#define SIMD_DETAIL_TARGET_IS_SCALAR(name) \
  SIMD_DETAIL_TARGET_IS_SCALAR_I(SIMD_DETAIL_TARGET_CAT(SIMD_DETAIL_TARGET_SCALAR_,name))
#define SIMD_DETAIL_TARGET_PUSH_0(name) \
  SIMD_DETAIL_TARGET_PRAGMA(clang attribute simd_source_target.push(__attribute__((target(SIMD_TARGET_STRING(name)))), apply_to=function))
#define SIMD_DETAIL_TARGET_PUSH_1(name) \
  SIMD_DETAIL_TARGET_PRAGMA(clang attribute simd_source_target.push)
#define SIMD_TARGET_PUSH(name) \
  SIMD_DETAIL_TARGET_CAT(SIMD_DETAIL_TARGET_PUSH_,SIMD_DETAIL_TARGET_IS_SCALAR(name))(name)
#define SIMD_TARGET_POP() SIMD_DETAIL_TARGET_PRAGMA(clang attribute simd_source_target.pop)

// A list uses X(name, ...), forwarding its extra arguments to X:
//   #define targets(X,...) X(avx512,__VA_ARGS__) X(avx2,__VA_ARGS__)
// A reusable body owns the complete function declaration and definition:
//   #define body(name,tag) void name(tag, float * p) { /* ... */ }
//   SIMD_TARGET_VARIANTS(kernel,targets,body)
// Each chosen tag has a distinct ordinary overloaded function symbol. List
// canonical feature sets must be unique. Definitions need normal C++ ODR rules.
#define SIMD_DETAIL_TARGET_EMIT(tag,name,body) \
  static_assert(::simd::architecture<SIMD_TARGET_TYPE(tag)>); \
  SIMD_TARGET_PUSH(tag) \
  body(name,SIMD_TARGET_TYPE(tag)) \
  SIMD_TARGET_POP()
#define SIMD_TARGET_VARIANTS(name,list,body) list(SIMD_DETAIL_TARGET_EMIT,name,body)

// Snapshot inherited translation-unit features separately from the requested
// tag identity. A target attribute adds features; it does not erase the project
// minimum. An unregistered stronger minimum is rejected instead of under-admitted.
#ifdef __MMX__
#define SIMD_DETAIL_MIN_MMX (1ull<<0)
#else
#define SIMD_DETAIL_MIN_MMX 0ull
#endif
#ifdef __SSE__
#define SIMD_DETAIL_MIN_SSE (1ull<<1)
#else
#define SIMD_DETAIL_MIN_SSE 0ull
#endif
#ifdef __SSE2__
#define SIMD_DETAIL_MIN_SSE2 (1ull<<2)
#else
#define SIMD_DETAIL_MIN_SSE2 0ull
#endif
#ifdef __SSE3__
#define SIMD_DETAIL_MIN_SSE3 (1ull<<3)
#else
#define SIMD_DETAIL_MIN_SSE3 0ull
#endif
#ifdef __SSSE3__
#define SIMD_DETAIL_MIN_SSSE3 (1ull<<4)
#else
#define SIMD_DETAIL_MIN_SSSE3 0ull
#endif
#ifdef __SSE4_1__
#define SIMD_DETAIL_MIN_SSE41 (1ull<<5)
#else
#define SIMD_DETAIL_MIN_SSE41 0ull
#endif
#ifdef __SSE4_2__
#define SIMD_DETAIL_MIN_SSE42 (1ull<<6)
#else
#define SIMD_DETAIL_MIN_SSE42 0ull
#endif
#ifdef __POPCNT__
#define SIMD_DETAIL_MIN_POPCNT (1ull<<7)
#else
#define SIMD_DETAIL_MIN_POPCNT 0ull
#endif
#ifdef __AVX__
#define SIMD_DETAIL_MIN_AVX (1ull<<8)
#else
#define SIMD_DETAIL_MIN_AVX 0ull
#endif
#ifdef __AVX2__
#define SIMD_DETAIL_MIN_AVX2 (1ull<<9)
#else
#define SIMD_DETAIL_MIN_AVX2 0ull
#endif
#ifdef __FMA__
#define SIMD_DETAIL_MIN_FMA (1ull<<10)
#else
#define SIMD_DETAIL_MIN_FMA 0ull
#endif
#ifdef __F16C__
#define SIMD_DETAIL_MIN_F16C (1ull<<11)
#else
#define SIMD_DETAIL_MIN_F16C 0ull
#endif
#ifdef __BMI__
#define SIMD_DETAIL_MIN_BMI1 (1ull<<12)
#else
#define SIMD_DETAIL_MIN_BMI1 0ull
#endif
#ifdef __BMI2__
#define SIMD_DETAIL_MIN_BMI2 (1ull<<13)
#else
#define SIMD_DETAIL_MIN_BMI2 0ull
#endif
#ifdef __AVX512F__
#define SIMD_DETAIL_MIN_AVX512F (1ull<<14)
#else
#define SIMD_DETAIL_MIN_AVX512F 0ull
#endif
#ifdef __AVX512DQ__
#define SIMD_DETAIL_MIN_AVX512DQ (1ull<<15)
#else
#define SIMD_DETAIL_MIN_AVX512DQ 0ull
#endif
#ifdef __AVX512BW__
#define SIMD_DETAIL_MIN_AVX512BW (1ull<<16)
#else
#define SIMD_DETAIL_MIN_AVX512BW 0ull
#endif
#ifdef __AVX512VL__
#define SIMD_DETAIL_MIN_AVX512VL (1ull<<17)
#else
#define SIMD_DETAIL_MIN_AVX512VL 0ull
#endif
#ifdef __AVX512BF16__
#define SIMD_DETAIL_MIN_AVX512BF16 (1ull<<18)
#else
#define SIMD_DETAIL_MIN_AVX512BF16 0ull
#endif
#ifdef __AVX512FP16__
#define SIMD_DETAIL_MIN_AVX512FP16 (1ull<<19)
#else
#define SIMD_DETAIL_MIN_AVX512FP16 0ull
#endif
#ifdef __ARM_NEON
#define SIMD_DETAIL_MIN_NEON (1ull<<20)
#else
#define SIMD_DETAIL_MIN_NEON 0ull
#endif
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) || defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC)
#define SIMD_DETAIL_MIN_NEON_FP16 (1ull<<21)
#else
#define SIMD_DETAIL_MIN_NEON_FP16 0ull
#endif
#ifdef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
#define SIMD_DETAIL_MIN_NEON_BF16 (1ull<<22)
#else
#define SIMD_DETAIL_MIN_NEON_BF16 0ull
#endif
#ifdef __AES__
#define SIMD_DETAIL_MIN_AES (1ull<<23)
#else
#define SIMD_DETAIL_MIN_AES 0ull
#endif
#ifdef __PCLMUL__
#define SIMD_DETAIL_MIN_PCLMUL (1ull<<24)
#else
#define SIMD_DETAIL_MIN_PCLMUL 0ull
#endif
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) && (defined(__x86_64__) || defined(_M_X64))
#define SIMD_DETAIL_MIN_CX16 (1ull<<25)
#else
#define SIMD_DETAIL_MIN_CX16 0ull
#endif
#ifdef __AVX512CD__
#define SIMD_DETAIL_MIN_AVX512CD (1ull<<26)
#else
#define SIMD_DETAIL_MIN_AVX512CD 0ull
#endif
#ifdef __AVX512IFMA__
#define SIMD_DETAIL_MIN_AVX512IFMA (1ull<<27)
#else
#define SIMD_DETAIL_MIN_AVX512IFMA 0ull
#endif

#ifdef __LZCNT__
#define SIMD_DETAIL_MIN_LZCNT (1ull<<28)
#else
#define SIMD_DETAIL_MIN_LZCNT 0ull
#endif
#ifdef __MOVBE__
#define SIMD_DETAIL_MIN_MOVBE (1ull<<29)
#else
#define SIMD_DETAIL_MIN_MOVBE 0ull
#endif
#ifdef __LAHF_SAHF__
#define SIMD_DETAIL_MIN_SAHF (1ull<<30)
#else
#define SIMD_DETAIL_MIN_SAHF 0ull
#endif
#ifdef __ARM_FEATURE_AES
#define SIMD_DETAIL_MIN_ARM_AES (1ull<<31)
#else
#define SIMD_DETAIL_MIN_ARM_AES 0ull
#endif
#ifdef __ARM_FEATURE_SHA2
#define SIMD_DETAIL_MIN_ARM_SHA2 (1ull<<32)
#else
#define SIMD_DETAIL_MIN_ARM_SHA2 0ull
#endif
#if defined(__ARM_FEATURE_SHA3) || defined(__ARM_FEATURE_SHA512)
#define SIMD_DETAIL_MIN_ARM_SHA3 (1ull<<33)
#else
#define SIMD_DETAIL_MIN_ARM_SHA3 0ull
#endif
#ifdef __ARM_FEATURE_CRC32
#define SIMD_DETAIL_MIN_ARM_CRC (1ull<<34)
#else
#define SIMD_DETAIL_MIN_ARM_CRC 0ull
#endif
#ifdef __ARM_FEATURE_ATOMICS
#define SIMD_DETAIL_MIN_ARM_LSE (1ull<<35)
#else
#define SIMD_DETAIL_MIN_ARM_LSE 0ull
#endif
#ifdef __ARM_FEATURE_QRDMX
#define SIMD_DETAIL_MIN_ARM_RDM (1ull<<36)
#else
#define SIMD_DETAIL_MIN_ARM_RDM 0ull
#endif
#ifdef __ARM_FEATURE_FP16_FML
#define SIMD_DETAIL_MIN_ARM_FP16FML (1ull<<37)
#else
#define SIMD_DETAIL_MIN_ARM_FP16FML 0ull
#endif
#ifdef __ARM_FEATURE_DOTPROD
#define SIMD_DETAIL_MIN_ARM_DOTPROD (1ull<<38)
#else
#define SIMD_DETAIL_MIN_ARM_DOTPROD 0ull
#endif
#ifdef __ARM_FEATURE_COMPLEX
#define SIMD_DETAIL_MIN_ARM_COMPLEX (1ull<<39)
#else
#define SIMD_DETAIL_MIN_ARM_COMPLEX 0ull
#endif
#ifdef __ARM_FEATURE_JCVT
#define SIMD_DETAIL_MIN_ARM_JSCVT (1ull<<40)
#else
#define SIMD_DETAIL_MIN_ARM_JSCVT 0ull
#endif
#ifdef __ARM_FEATURE_RCPC
#define SIMD_DETAIL_MIN_ARM_RCPC (1ull<<41)
#else
#define SIMD_DETAIL_MIN_ARM_RCPC 0ull
#endif
#ifdef __ARM_FEATURE_PAUTH
#define SIMD_DETAIL_MIN_ARM_PAUTH (1ull<<42)
#else
#define SIMD_DETAIL_MIN_ARM_PAUTH 0ull
#endif

#if defined(__SSE4A__) || defined(__XOP__) || defined(__FMA4__) || \
    defined(__AVX512VBMI__) || defined(__AVX512VBMI2__) || defined(__AVX512VNNI__) || \
    defined(__AVX512BITALG__) || defined(__AVX512VPOPCNTDQ__) || defined(__AVX512VP2INTERSECT__) || \
    defined(__GFNI__) || defined(__VAES__) || defined(__VPCLMULQDQ__) || defined(__AVXVNNI__) || \
    defined(__AVXIFMA__) || defined(__AVXNECONVERT__) || defined(__AVXVNNIINT8__) || \
    defined(__AVXVNNIINT16__) || defined(__AMX_TILE__) || defined(__AMX_INT8__) || defined(__AMX_BF16__) || \
    defined(__ARM_FEATURE_SVE) || defined(__ARM_FEATURE_MATMUL_INT8) || defined(__ARM_FEATURE_SME)
#define SIMD_DETAIL_MIN_UNREGISTERED (1ull<<63)
#else
#define SIMD_DETAIL_MIN_UNREGISTERED 0ull
#endif

// Optional additional project contract. This is a feature mask/expression (for
// example simd::target_features("avx2,f16c")), never a CPU-name guess. The ordinary
// executable's minimum remains a startup precondition: the selector itself must
// already be safe to run. Compiler CPU models can enable features not represented
// by predefines; this macro is not an inference engine for arbitrary -mcpu flags.
#ifndef SIMD_TARGET_EXTRA_MINIMUM
#define SIMD_TARGET_EXTRA_MINIMUM 0ull
#endif
#define SIMD_TARGET_MINIMUM (::simd::feature_closure(SIMD_TARGET_EXTRA_MINIMUM| \
  SIMD_DETAIL_MIN_MMX|SIMD_DETAIL_MIN_SSE|SIMD_DETAIL_MIN_SSE2|SIMD_DETAIL_MIN_SSE3| \
  SIMD_DETAIL_MIN_SSSE3|SIMD_DETAIL_MIN_SSE41|SIMD_DETAIL_MIN_SSE42|SIMD_DETAIL_MIN_POPCNT| \
  SIMD_DETAIL_MIN_AVX|SIMD_DETAIL_MIN_AVX2|SIMD_DETAIL_MIN_FMA|SIMD_DETAIL_MIN_F16C| \
  SIMD_DETAIL_MIN_BMI1|SIMD_DETAIL_MIN_BMI2|SIMD_DETAIL_MIN_AVX512F|SIMD_DETAIL_MIN_AVX512DQ| \
  SIMD_DETAIL_MIN_AVX512BW|SIMD_DETAIL_MIN_AVX512VL|SIMD_DETAIL_MIN_AVX512BF16|SIMD_DETAIL_MIN_AVX512FP16| \
  SIMD_DETAIL_MIN_NEON|SIMD_DETAIL_MIN_NEON_FP16|SIMD_DETAIL_MIN_NEON_BF16| \
  SIMD_DETAIL_MIN_AES|SIMD_DETAIL_MIN_PCLMUL|SIMD_DETAIL_MIN_CX16|SIMD_DETAIL_MIN_AVX512CD| \
  SIMD_DETAIL_MIN_AVX512IFMA|SIMD_DETAIL_MIN_LZCNT|SIMD_DETAIL_MIN_MOVBE|SIMD_DETAIL_MIN_SAHF| \
  SIMD_DETAIL_MIN_ARM_AES|SIMD_DETAIL_MIN_ARM_SHA2|SIMD_DETAIL_MIN_ARM_SHA3|SIMD_DETAIL_MIN_ARM_CRC| \
  SIMD_DETAIL_MIN_ARM_LSE|SIMD_DETAIL_MIN_ARM_RDM|SIMD_DETAIL_MIN_ARM_FP16FML|SIMD_DETAIL_MIN_ARM_DOTPROD| \
  SIMD_DETAIL_MIN_ARM_COMPLEX|SIMD_DETAIL_MIN_ARM_JSCVT|SIMD_DETAIL_MIN_ARM_RCPC|SIMD_DETAIL_MIN_ARM_PAUTH| \
  SIMD_DETAIL_MIN_UNREGISTERED))

#define SIMD_DETAIL_TARGET_LIST(tag,...) \
  +::simd::isa_list<::simd::target_entry<SIMD_TARGET_TYPE(tag),SIMD_TARGET_MINIMUM>>{}
#define SIMD_TARGET_LIST(list) (::simd::isa_list<>{} list(SIMD_DETAIL_TARGET_LIST,unused))
