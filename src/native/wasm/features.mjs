// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// Complete, import-free modules. Validation does not instantiate or execute them.
// https://webassembly.github.io/spec/core/binary/instructions.html
// https://github.com/WebAssembly/relaxed-simd/blob/main/proposals/relaxed-simd/Overview.md
const header = [0, 97, 115, 109, 1, 0, 0, 0];
const simd128 = [
  ...header,
  1, 5, 1, 0x60, 0, 1, 0x7b, // type: () -> v128
  3, 2, 1, 0,                // one function, type 0
  10, 22, 1, 20, 0,          // code: one 20-byte body, no locals
  0xfd, 0x0c,                // v128.const i32x4 0 0 0 0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0x0b,
];
const relaxed_simd = [
  ...header,
  1, 7, 1, 0x60, 2, 0x7b, 0x7b, 1, 0x7b, // type: (v128, v128) -> v128
  3, 2, 1, 0,
  10, 11, 1, 9, 0,                       // code: one 9-byte body, no locals
  0x20, 0, 0x20, 1,                      // local.get 0; local.get 1
  0xfd, 0x80, 0x02,                      // i8x16.relaxed_swizzle (ULEB128 0x100)
  0x0b,
];

/** Fresh probe bytes for native embedders or diagnostics; the caller owns them. */
export function wasm_feature_probe(feature) {
  if (feature === "simd128") return new Uint8Array(simd128);
  if (feature === "relaxed_simd") return new Uint8Array(relaxed_simd);
  throw new RangeError(`Unknown Wasm feature: ${feature}`);
}

/**
 * Observe the supplied WebAssembly namespace (the current realm by default).
 * Fields match native::wasm_capabilities::raw_observations. Missing APIs, thrown
 * queries and non-boolean results remain unknown. A completed false is observed.
 * No module is instantiated, no function runs, and no loading policy is selected.
 */
export function observe_wasm_capabilities(runtime) {
  const raw = {
    simd128_observed: false, simd128: false,
    relaxed_simd_observed: false, relaxed_simd: false,
  };
  let validate;
  try {
    if (arguments.length === 0) runtime = globalThis.WebAssembly;
    validate = runtime?.validate;
    // A valid empty module checks that this adapter can perform validation.
    if (typeof validate !== "function" || validate.call(runtime, new Uint8Array(header)) !== true)
      return raw;
  } catch {
    return raw;
  }
  for (const feature of ["simd128", "relaxed_simd"]) {
    try {
      const supported = validate.call(runtime, wasm_feature_probe(feature));
      if (typeof supported === "boolean") {
        raw[`${feature}_observed`] = true;
        raw[feature] = supported;
      }
    } catch {
      // A failed query must not become an observed negative or authorize code.
    }
  }
  return raw;
}
