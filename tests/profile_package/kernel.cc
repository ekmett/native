#if PROFILE_AVX2
import native;
using V=native::vec<float,8,native::avx2>;
#define ENTRY kernel_avx2
#else
import native;
using V=native::vec<float,16,native::avx512>;
#define ENTRY kernel_avx512
#endif
extern "C" int ENTRY(float const *input,float *out) {
  for(unsigned i=0;i<16;i+=V::lanes) {
    auto x=V::load(input+i);
    fma(x,V(2.f),V(1.f)).store(out+i);
  }
  return V::lanes;
}
