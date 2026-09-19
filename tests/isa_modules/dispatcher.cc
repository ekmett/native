// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <cstdio>
#include <cstring>
#include <cstdint>
import simd.x86;
#if (!SIMD_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
extern "C" int backend_avx2(float const*,float const*,float*);
extern "C" int backend_avx512(float const*,float const*,float*);
extern "C" __declspec(dllexport) __declspec(noinline) unsigned supported_backends(){
 auto cpu=simd::observe_x86_capabilities();
 auto avx2=simd::classify_isa(cpu,simd::avx2);
 auto avx512=simd::classify_isa(cpu,simd::avx512);
 unsigned result=unsigned(avx2.admitted())|(unsigned(avx512.admitted())<<1);
 std::printf("AVX2: %s; AVX512: %s; supported=%u\n",avx2.reason(),avx512.reason(),result);
 return result;
}
int main(int argc,char**argv){
 unsigned supported=supported_backends();unsigned choice=(supported&2)?2:(supported&1)?1:0;
 if(argc>1){if(std::strcmp(argv[1],"avx2")==0)choice=1;else if(std::strcmp(argv[1],"avx512")==0)choice=2;else return 2;}
 if((choice&supported)==0){std::puts("requested backend unsupported");return 3;}
 // Select one coarse entry once. The kernels contain no backend dispatch.
 auto entry=choice==2?backend_avx512:backend_avx2;
 float a[48],b[48],out[48];for(int i=0;i<48;++i){a[i]=float(i);b[i]=float(i)*2.f;}
 int lanes=entry(a,b,out);for(int i=0;i<48;++i)if(out[i]!=float(2*i*i+3*i))return 4;
 std::printf("selected=%s lanes=%d all48 passed\n",choice==2?"avx512":"avx2",lanes);
 return lanes==(choice==2?16:8)?0:5;
}
