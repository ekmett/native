#if PROFILE_AVX2
import simd.avx2;
using V=simd::vec<float,8,simd::avx2>;
#define ENTRY kernel_avx2
#else
import simd.avx512;
using V=simd::vec<float,16,simd::avx512>;
#define ENTRY kernel_avx512
#endif
extern "C" int ENTRY(float const *input,float *out) {
  for(unsigned i=0;i<16;i+=V::lanes) {
    auto x=V::load(input+i);
    fma(x,V(2.f),V(1.f)).store(out+i);
  }
  return V::lanes;
}
