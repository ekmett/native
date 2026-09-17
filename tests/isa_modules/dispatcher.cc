// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <intrin.h>
extern "C" int backend_avx2(float const*,float const*,float*);
extern "C" int backend_avx512(float const*,float const*,float*);
extern "C" __declspec(dllexport) __declspec(noinline) unsigned supported_backends(){
 int r[4];__cpuidex(r,0,0);unsigned maximum=unsigned(r[0]);if(maximum<1)return 0;
 __cpuidex(r,1,0);unsigned ecx=unsigned(r[2]);
 if((ecx&((1u<<27)|(1u<<28)|(1u<<12)))!=((1u<<27)|(1u<<28)|(1u<<12)))return 0;
 auto xcr0=_xgetbv(0);if((xcr0&6)!=6||maximum<7)return 0;
 __cpuidex(r,7,0);unsigned ebx=unsigned(r[1]);unsigned result=(ebx&(1u<<5))?1u:0u;
 constexpr unsigned avx512=(1u<<16)|(1u<<17)|(1u<<30)|(1u<<31);
 if((result&1)&&(ebx&avx512)==avx512&&(xcr0&0xe6)==0xe6)result|=2;
 std::printf("leaf7ebx=%u xcr0=%llu supported=%u\n",ebx,static_cast<unsigned long long>(xcr0),result);
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
