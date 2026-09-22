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
#define SELECTED_TARGETS custom,neon_fp16,neon
template<native::isa<> A> constexpr int selected_target=
  native::target<A,NATIVE_TARGET_ISA(custom),native::neon_fp16,native::neon>;
constexpr auto extra_feature=native::arm_feature::dotprod;
#else
#define NATIVE_TARGET_custom "avx2,fma,bmi2,f16c"
#define SELECTED_TARGETS avx512,custom,avx2
template<native::isa<> A> constexpr int selected_target=
  native::target<A,native::avx512,NATIVE_TARGET_ISA(custom),native::avx2>;
constexpr auto extra_feature=native::x86_feature::gfni;
#endif

#define CHECK_SELECTION(unused,tag) \
  static_assert(selected_target<NATIVE_TARGET_ISA(tag)> >= 0); \
  static_assert(selected_target<NATIVE_TARGET_ISA(tag)&extra_feature> == selected_target<NATIVE_TARGET_ISA(tag)>);
NATIVE_DETAIL_TARGET_MAP(CHECK_SELECTION,unused,SELECTED_TARGETS)
#undef CHECK_SELECTION
static_assert(selected_target<native::scalar> == -1);

#define DOUBLE_DECLARE(name,tag,...) \
  template<native::isa<> A> requires(native::target<A,__VA_ARGS__> == native::target<tag,__VA_ARGS__>) \
  void name(float * output,float const * input);
NATIVE_TARGET_VARIANTS(source_kernel,DOUBLE_DECLARE,SELECTED_TARGETS)
#undef DOUBLE_DECLARE

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
#if defined(__aarch64__) || defined(_M_ARM64)
#define NATIVE_FIXTURE_LANES(tag) 4
#else
#define NATIVE_FIXTURE_LANES(tag) ((tag).has(native::x86_feature::avx512f)?16:8)
#endif
#define DOUBLE_BODY(name,tag,...) \
  template<native::isa<> A,class V> requires(native::target<A,__VA_ARGS__> == native::target<tag,__VA_ARGS__>) \
  __attribute__((always_inline)) inline V name##_native(V value) { \
    NATIVE_DOUBLE(value) \
  } \
  template<native::isa<> A> requires(native::target<A,__VA_ARGS__> == native::target<tag,__VA_ARGS__>) \
  __attribute__((noinline)) void name(float * output,float const * input) { \
    constexpr unsigned lanes=NATIVE_FIXTURE_LANES(tag); \
    DOUBLE_STEP(name,tag,lanes) \
  }
NATIVE_TARGET_VARIANTS(source_kernel,DOUBLE_BODY,SELECTED_TARGETS)
#if defined(__aarch64__) || defined(_M_ARM64)
NATIVE_TARGET_VARIANTS(source_superset,DOUBLE_BODY,custom,neon_fp16,neon)
#else
NATIVE_TARGET_VARIANTS(source_superset,DOUBLE_BODY,avx512,custom,avx2)
#endif
#undef DOUBLE_BODY
#undef NATIVE_FIXTURE_LANES
#undef DOUBLE_STEP
#undef NATIVE_DOUBLE

template<native::isa<> A> concept has_source_kernel=requires {
  source_kernel<A>(nullptr,nullptr);
};
static_assert(!has_source_kernel<native::scalar>);

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
#define RUN_EACH(unused,name) \
  ++index; \
  if(native::classify_isa(cpu,NATIVE_TARGET_ISA(name),NATIVE_TARGET_MINIMUM).admitted()) { \
    if(!expected) expected=index; \
    clear();source_kernel<NATIVE_TARGET_ISA(name)>(output,input); \
    if(!correct()) return 2; \
    clear();source_superset<NATIVE_TARGET_ISA(name)&extra_feature>(output,input); \
    if(!correct()) return 6; \
    ++executed;std::printf("source target %s: executed\n",#name); \
  } else { \
    ++skipped;std::printf("source target %s: skipped (not admitted)\n",#name); \
  }
  NATIVE_DETAIL_TARGET_MAP(RUN_EACH,unused,SELECTED_TARGETS)
#undef RUN_EACH
  unsigned calls=0;
  native::isa<> selected_features{};
  clear();
  auto selected=native::with_isa(NATIVE_TARGET_LIST(SELECTED_TARGETS),cpu,[&]<native::isa<> A> {
    ++calls;selected_features=A;
    source_kernel<A>(output,input);
  });
  if(selected!=(executed!=0) || calls!=unsigned(selected)) return 3;
  if(!selected) return 77; // Scalar ran; this host admits no selected native body.
  if(!correct()) return 4;
  index=0;
#define CHECK_ORDER(unused,name) \
  if(++index==expected && selected_features!=NATIVE_TARGET_ISA(name)) return 5;
  NATIVE_DETAIL_TARGET_MAP(CHECK_ORDER,unused,SELECTED_TARGETS)
#undef CHECK_ORDER
  std::printf("scalar path passed; %u variants executed, %u skipped; ordered dispatch passed\n",executed,skipped);
  return 0;
}
#endif
