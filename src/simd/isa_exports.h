// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Include after a named-module declaration; isa.h definitions belong to the GMF.
export namespace simd {
  using ::simd::feature_set;
  using ::simd::feature;
  using ::simd::feature_closure;
  using ::simd::isa_tag;
  using ::simd::isa;
  using ::simd::architecture;
  using ::simd::has_feature;
  using ::simd::has_features;
  using ::simd::scalar;
  using ::simd::avx2;
  using ::simd::avx512;
  using ::simd::avx512_bf16;
  using ::simd::avx512_fp16;
  using ::simd::neon;
  using ::simd::neon_fp16;
  using ::simd::neon_bf16;
  using ::simd::target_features;
  using ::simd::isa_admission;
  using ::simd::classify_isa;
  using ::simd::target_entry;
  using ::simd::isa_list;
  using ::simd::abi_npos;
  using ::simd::abi_lookup;
  using ::simd::requires_abi;
  using ::simd::with_isa;
  using ::simd::operator|;
  using ::simd::operator+;
}
