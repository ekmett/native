// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/isa.h"
#include <span>
#include <optional>

namespace native {
  /// \defgroup wasm_capabilities WebAssembly capabilities
  /// Runtime validation observations, independent of CPU features and SIMD types.

  namespace detail {
    // Complete import-free modules; v128.const and i8x16.relaxed_swizzle.
    // The JS bridge carries the same bytes, checked by the consumer tests.
    inline constexpr std::uint8_t wasm_simd128_probe[]{
      0,97,115,109,1,0,0,0, 1,5,1,0x60,0,1,0x7b, 3,2,1,0,
      10,22,1,20,0, 0xfd,0x0c,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0x0b
    };
    inline constexpr std::uint8_t wasm_relaxed_simd_probe[]{
      0,97,115,109,1,0,0,0, 1,7,1,0x60,2,0x7b,0x7b,1,0x7b, 3,2,1,0,
      10,11,1,9,0, 0x20,0,0x20,1,0xfd,0x80,0x02,0x0b
    };
  }

  /// \ingroup wasm_capabilities
  /// A complete module for validating one feature, without executing it.
  /// The returned immutable bytes have static lifetime; unknown enums give an
  /// empty span. Validation must use the settings of the intended runtime.
  constexpr std::span<std::uint8_t const> wasm_feature_probe(wasm_feature feature) noexcept {
    switch(feature) {
      case wasm_feature::simd128: return detail::wasm_simd128_probe;
      case wasm_feature::relaxed_simd: return detail::wasm_relaxed_simd_probe;
    }
    return {};
  }

  /// \ingroup wasm_capabilities
  /// Normalized capabilities of one WebAssembly runtime and its current settings.
  /// Admission reads present/observed. Compilation flags do not establish support.
  struct wasm_capabilities {
    /// Features successfully observed as supported.
    feature_set<wasm_feature> present{};
    /// Features whose observations completed, including negative answers.
    feature_set<wasm_feature> observed{};
    /// Embedder-supplied query results; an unobserved positive never authorizes code.
    struct raw_observations {
      /// True only after a SIMD128 query completes with a boolean answer.
      bool simd128_observed = false;
      /// The runtime accepts standard SIMD128 instructions.
      bool simd128 = false;
      /// True only after a relaxed-SIMD query completes with a boolean answer.
      bool relaxed_simd_observed = false;
      /// The runtime accepts the finalized relaxed-SIMD instruction set.
      bool relaxed_simd = false;
    };
    /// Diagnostic snapshot; editing it does not update present/observed.
    raw_observations raw{};
  };

  /// \ingroup wasm_capabilities
  /// Normalize independent observations without inferring missing answers.
  /// classify_isa separately requires SIMD128 when relaxed SIMD is requested.
  constexpr wasm_capabilities decode_wasm_capabilities(wasm_capabilities::raw_observations raw) noexcept {
    wasm_capabilities result;
    result.raw=raw;
    result.observed.set(wasm_feature::simd128,raw.simd128_observed);
    result.present.set(wasm_feature::simd128,raw.simd128_observed && raw.simd128);
    result.observed.set(wasm_feature::relaxed_simd,raw.relaxed_simd_observed);
    result.present.set(wasm_feature::relaxed_simd,raw.relaxed_simd_observed && raw.relaxed_simd);
    return result;
  }

  /// \ingroup wasm_capabilities
  /// Query an embedder's validator without executing probe instructions.
  /// The nonthrowing callback accepts a byte span and returns optional<bool>:
  /// nullopt means unavailable/failed, false means observed absent. A valid empty
  /// module must first be accepted. Feature queries then proceed independently.
  /// The callback translates engine errors into nullopt; no loader is selected.
  template<class V> requires requires(V & validate,std::span<std::uint8_t const> bytes) {
    { validate(bytes) } noexcept -> std::same_as<std::optional<bool>>;
  }
  constexpr wasm_capabilities observe_wasm_capabilities(V && validate) noexcept {
    auto baseline=validate(wasm_feature_probe(wasm_feature::simd128).first(8));
    if(!baseline || !*baseline) return {};
    auto simd128=validate(wasm_feature_probe(wasm_feature::simd128));
    auto relaxed_simd=validate(wasm_feature_probe(wasm_feature::relaxed_simd));
    return decode_wasm_capabilities({simd128.has_value(),simd128.value_or(false),
      relaxed_simd.has_value(),relaxed_simd.value_or(false)});
  }

  /// \ingroup wasm_capabilities
  /// Return unknown observations: portable C++ has no WebAssembly engine query.
  /// Use decode_wasm_capabilities with observations from the embedder or the
  /// optional JavaScript bridge. No compiler-macro inference or probing occurs.
  constexpr wasm_capabilities observe_wasm_capabilities() noexcept { return {}; }
}
