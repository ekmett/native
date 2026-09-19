// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// Intentionally included once for each architecture specialization.
// This is a real specialization family, not aliases to another backend.
template<> struct vec<float,PROBE_LANES,PROBE_ARCH> {
#if PROBE_LANES == 8
  using native_register=__m256;
#else
  using native_register=__m512;
#endif
  native_register value;
  vec()=default;
  inline __attribute__((always_inline)) explicit vec(native_register x):value(x){}
  template<class... X> requires(sizeof...(X)==PROBE_LANES && (std::same_as<X,float>&&...))
  inline __attribute__((always_inline)) vec(X... x):value{x...}{}
  static inline __attribute__((always_inline)) vec load(float const* x){
#if PROBE_LANES == 8
    return vec(_mm256_loadu_ps(x));
#else
    return vec(_mm512_loadu_ps(x));
#endif
  }
  inline __attribute__((always_inline)) void store(float* x)const{
#if PROBE_LANES == 8
    _mm256_storeu_ps(x,value);
#else
    _mm512_storeu_ps(x,value);
#endif
  }
  friend inline __attribute__((always_inline)) vec operator+(vec a,vec b){
#if PROBE_LANES == 8
    return vec(_mm256_add_ps(a.value,b.value));
#else
    return vec(_mm512_add_ps(a.value,b.value));
#endif
  }
  friend inline __attribute__((always_inline)) vec fma(vec a,vec b,vec c){
#if PROBE_LANES == 8
    return vec(_mm256_fmadd_ps(a.value,b.value,c.value));
#else
    return vec(_mm512_fmadd_ps(a.value,b.value,c.value));
#endif
  }
  inline __attribute__((always_inline)) vec&operator+=(vec b){return *this=*this+b;}
};
int PROBE_ENTRY()noexcept;
