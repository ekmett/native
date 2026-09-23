// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/wide.h>
#include <native/wide_math.h>
#include <array>
#include <utility>

#if defined(__wasm__)
using vector = native::simd<float,4,native::feature_closure(native::wasm_feature::simd128)>;
#elif defined(__AVX512F__)
using vector = native::simd<float,16,native::avx512>;
#elif defined(__AVX2__)
using vector = native::simd<float,8,native::avx2>;
#else
using vector = native::simd<float,4,native::neon>;
#endif

template<bool Log, std::size_t... I>
native_inline void evaluate(float * out, float const * in, std::index_sequence<I...>) {
  native::wide values{std::array{vector::load(in + I * vector::lanes)...}};
  if constexpr (Log) {
    auto result = math::log2(values);
    (result.registers[I].store(out + I * vector::lanes), ...);
  } else {
    auto result = math::exp2(values);
    (result.registers[I].store(out + I * vector::lanes), ...);
  }
}

#define BASE2_ENTRY(name, logarithm, count) \
  extern "C" void name##_##count(float * out, float const * in) { \
    evaluate<logarithm>(out, in, std::make_index_sequence<count>{}); \
  }
BASE2_ENTRY(exp2, false, 1)
BASE2_ENTRY(exp2, false, 2)
BASE2_ENTRY(exp2, false, 6)
BASE2_ENTRY(log2, true, 1)
BASE2_ENTRY(log2, true, 2)
BASE2_ENTRY(log2, true, 6)
#undef BASE2_ENTRY
