// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Include after a named-module declaration; isa.h definitions belong to the GMF.
export namespace native {
  using ::native::is_arm;
  using ::native::is_x86;
  using ::native::is_wasm;
  using ::native::x86_feature;
  using ::native::arm_feature;
  using ::native::wasm_feature;
  using ::native::x86_feature_count;
  using ::native::arm_feature_count;
  using ::native::wasm_feature_count;
  using ::native::feature_closure;
  using ::native::isa;
  using ::native::feature_set;
  using ::native::scalar;
  using ::native::avx2;
  using ::native::avx512;
  using ::native::avx512_bf16;
  using ::native::avx512_fp16;
  using ::native::neon;
  using ::native::neon_fp16;
  using ::native::neon_bf16;
  using ::native::target_features;
  using ::native::isa_admission;
  using ::native::classify_isa;
  using ::native::target_entry;
  using ::native::isa_list;
  using ::native::with_isa;
  using ::native::operator&;
  using ::native::operator<;
  using ::native::operator>;
  using ::native::operator<=;
  using ::native::operator>=;
  using ::native::operator+;
}
