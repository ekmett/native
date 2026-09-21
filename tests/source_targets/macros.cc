// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
#include <cstdio>
#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif
#ifdef NATIVE_TARGETS_METADATA_ONLY
import native_target_metadata;
#else
import native.scalar;
import native;
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_TARGET_custom "neon,fullfp16,bf16"
#define SELECTED_TARGETS(X,...) X(neon,__VA_ARGS__) X(neon_fp16,__VA_ARGS__) X(custom,__VA_ARGS__)
#else
#define NATIVE_TARGET_custom "avx2,fma,bmi2,f16c"
#define SELECTED_TARGETS(X,...) X(avx512,__VA_ARGS__) X(avx2,__VA_ARGS__) X(custom,__VA_ARGS__)
#endif

// Both arguments and result cross the implicit native-register bridge. This
// helper belongs to the generated body and receives exactly its target scope.
#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_DOUBLE(value) return vaddq_f32(value,value);
#else
#define NATIVE_DOUBLE(value) \
  if constexpr(sizeof(V)==64) return _mm512_add_ps(value,value); \
  else return _mm256_add_ps(value,value);
#endif

// The metadata probe uses raw registers. The installed test additionally keeps
// real hub wrappers and two-register wide arithmetic around the native bridge.
#ifdef NATIVE_TARGETS_METADATA_ONLY
#define DOUBLE_STEP(name,tag,lanes) \
  using native=float __attribute__((ext_vector_type(lanes))); \
  for(unsigned i=0;i<32;i+=lanes) { \
    native value;__builtin_memcpy(&value,input+i,sizeof(value)); \
    value+=value;value=name##_native<tag>(value); \
    __builtin_memcpy(output+i,&value,sizeof(value)); \
  }
#else
#define DOUBLE_STEP(name,tag,lanes) \
  using V=native::simd<float,lanes,tag>; \
  for(unsigned i=0;i<32;i+=2*lanes) { \
    native::wide<V,2> value{V::load(input+i),V::load(input+i+lanes)}; \
    value=value+value; \
    V first=name##_native<tag>(value.registers[0]); \
    V second=name##_native<tag>(value.registers[1]); \
    first.store(output+i);second.store(output+i+lanes); \
  }
#endif
#define DOUBLE_BODY(name,tag) \
  template<native::isa A,class V> requires(A == tag) \
  __attribute__((always_inline)) inline V name##_native(V value) { \
    NATIVE_DOUBLE(value) \
  } \
  template<native::isa A> requires(A == tag) \
  __attribute__((noinline)) void name(float * output,float const * input) { \
    constexpr unsigned lanes=tag.has(native::x86_feature::avx512f)?16: \
      tag.has(native::arm_feature::neon)?4:8; \
    DOUBLE_STEP(name,tag,lanes) \
  }
NATIVE_TARGET_VARIANTS(source_kernel,SELECTED_TARGETS,DOUBLE_BODY)
#undef DOUBLE_BODY
#undef DOUBLE_STEP
#undef NATIVE_DOUBLE

// The target scope must end at the body, leaving later declarations ordinary.
extern "C" __attribute__((noinline)) float source_after_scope(float x) { return x+x; }

#if __has_feature(address_sanitizer)
// Read from this exact object's disassembly, including externally added ASan
// flags. Instrumented variants still check symbols and native instructions.
extern "C" int source_targets_asan_instrumented() { return 1; }
#endif

#ifndef NATIVE_TARGETS_METADATA_ONLY
int scalar_result(int);

// Ordinary baseline code in the same TU as every attributed native variant.
bool scalar_path() {
  using V=native::simd<float,1,native::scalar>;
  native::wide<V,2> value{V(3.f),V(-4.f)};
  value=value+value;
  float output[2]{};
  value.registers[0].store(output);
  value.registers[1].store(output+1);
  return output[0]==6.f && output[1]==-8.f && scalar_result(4)==5;
}

int main() {
  if(!scalar_path()) return 1;
#if defined(__aarch64__) || defined(_M_ARM64)
  auto cpu=native::observe_arm_capabilities();
#else
  auto cpu=native::observe_x86_capabilities();
#endif
  float input[32],output[32]{};
  for(unsigned i=0;i<32;++i) input[i]=float(i)-17;
  auto correct=[&] {
    for(unsigned i=0;i<32;++i) if(output[i]!=input[i]*4) return false;
    return true;
  };
  auto clear=[&] { for(float & value:output) value=0; };
  unsigned expected=0,executed=0,skipped=0,index=0;
#define RUN_EACH(name,...) \
  ++index; \
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(name),NATIVE_TARGET_MINIMUM).admitted()) { \
    if(!expected) expected=index; \
    clear();source_kernel<NATIVE_TARGET_ISA(name)>(output,input); \
    if(!correct()) return 2; \
    ++executed;std::printf("source target %s: executed\n",#name); \
  } else { \
    ++skipped;std::printf("source target %s: skipped (not admitted)\n",#name); \
  }
  SELECTED_TARGETS(RUN_EACH)
#undef RUN_EACH
  unsigned calls=0;
  native::isa selected_features{};
  clear();
  auto selected=native::with_isa(NATIVE_TARGET_LIST(SELECTED_TARGETS),cpu,[&]<native::isa A> {
    ++calls;selected_features=A;
    source_kernel<A>(output,input);
  });
  if(selected!=(executed!=0) || calls!=unsigned(selected)) return 3;
  if(!selected) return 77; // Scalar ran; this host admits no selected native body.
  if(!correct()) return 4;
  index=0;
#define CHECK_ORDER(name,...) \
  if(++index==expected && selected_features!=NATIVE_TARGET_ISA(name)) return 5;
  SELECTED_TARGETS(CHECK_ORDER)
#undef CHECK_ORDER
  std::printf("scalar path passed; %u variants executed, %u skipped; ordered dispatch passed\n",executed,skipped);
  return 0;
}
#endif
