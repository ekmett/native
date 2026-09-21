// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "native/simd/math/bits.h"
#define NATIVE_BACKEND_BODY "native/simd/math/sincos_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY
#include <array>
#include <cstdlib>

// Keep the historical array kernels in a separate header-only consumer. The
// promoted tests can import native without exposing private backend declarations,
// and their reference does not call the new promoted polynomial graph.
#define DEFINE_TRIG_REFERENCE(Name, Backend, Type) \
  static void Name(float const * input, float * sine, float * cosine, \
      float * paired_sine, float * paired_cosine) { \
    using V = Backend::Type; \
    std::array<V, 1> values{V::loadu(input)}; \
    auto s = Backend::sin(values); \
    auto c = Backend::cos(values); \
    auto pair = Backend::sincos(values); \
    s[0].storeu(sine); c[0].storeu(cosine); \
    pair.first[0].storeu(paired_sine); pair.second[0].storeu(paired_cosine); \
  }
DEFINE_TRIG_REFERENCE(scalar_reference, native::detail::scalar_backend::native, fp32x1)
#if defined(__AVX2__)
DEFINE_TRIG_REFERENCE(native_reference, native::detail::avx2_backend::native, fp32x8)
DEFINE_TRIG_REFERENCE(narrow_reference, native::detail::avx2_backend::native, fp32x4)
#elif defined(__ARM_NEON)
DEFINE_TRIG_REFERENCE(native_reference, native::detail::neon_backend::native, fp32x4)
#endif
#undef DEFINE_TRIG_REFERENCE

extern "C" void promoted_trig_reference(unsigned lanes, float const * input,
    float * sine, float * cosine, float * paired_sine, float * paired_cosine) {
  if (lanes == 1) {
    scalar_reference(input, sine, cosine, paired_sine, paired_cosine);
    return;
  }
#if defined(__AVX2__)
  if (lanes >= 2 && lanes <= 4) {
    std::array<float, 4> padded{}, s{}, c{}, ps{}, pc{};
    for (unsigned i = 0; i < lanes; ++i) padded[i] = input[i];
    narrow_reference(padded.data(), s.data(), c.data(), ps.data(), pc.data());
    for (unsigned i = 0; i < lanes; ++i) {
      sine[i] = s[i]; cosine[i] = c[i];
      paired_sine[i] = ps[i]; paired_cosine[i] = pc[i];
    }
    return;
  }
  if (lanes == 8) {
    native_reference(input, sine, cosine, paired_sine, paired_cosine);
    return;
  }
#elif defined(__ARM_NEON)
  if (lanes == 4) {
    native_reference(input, sine, cosine, paired_sine, paired_cosine);
    return;
  }
#endif
  std::abort();
}
