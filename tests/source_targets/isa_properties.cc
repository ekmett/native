// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native;
#if NATIVE_ISA_PROPERTIES_INCLUDE
#include <native/isa.h>
#endif

static_assert([] {
  native::isa<native::arm> a;
  a.aes=true;
  a.pmull=true;
  a.crc=true;
  return a.aes && a.pmull && a.crc && !a.sha1;
}());
static_assert([] {
  native::isa<native::x86> a;
  a.aes=true;
  a.popcnt=true;
  a.avx2=true;
  return a.aes && a.popcnt && a.avx2 && !a.fma;
}());
static_assert([] {
  native::isa<native::wasm> a;
  a.simd128=true;
  a.relaxed_simd=true;
  return a.simd128 && a.relaxed_simd;
}());

static_assert(native::avx2.avx2 && native::avx2.fma);
static_assert(native::neon.neon);
static_assert(native::target_features<native::wasm>("relaxed-simd").relaxed_simd);
