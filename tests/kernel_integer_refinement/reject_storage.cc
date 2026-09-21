// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
import native;

#if NATIVE_STORAGE_REJECT == 1
constexpr auto arch=native::target_features("avx2,fma,avx512f");
native_target("avx2,fma,avx512f") void rejected(std::uint32_t const* input,std::uint8_t* output) {
  native::reinterpret_bits<std::uint8_t>(native::simd<std::uint32_t,16,arch>::loadu(input)).storeu(output);
}
#elif NATIVE_STORAGE_REJECT == 2
constexpr auto arch=native::target_features("avx2,fma,avx512f");
native_target("avx2,fma,avx512f") void rejected(std::uint32_t const* input,std::uint16_t* output) {
  auto value=native::simd<std::uint32_t,16,arch>::loadu(input);
  native::narrow_concat<std::uint16_t>(value,value).storeu(output);
}
#elif NATIVE_STORAGE_REJECT == 3
constexpr auto arch=native::target_features("avx2,fma,avx512f,avx512dq");
native_target("avx2,fma,avx512f,avx512dq") void rejected(std::uint16_t const* input,std::uint8_t* output) {
  auto value=native::simd<std::uint16_t,32,arch>::loadu(input);
  native::narrow_concat<std::uint8_t>(value,value).storeu(output);
}
#else
#error Select a storage-only operation rejection
#endif
