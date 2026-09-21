# WebAssembly feature observations

`import native.wasm.features;` exposes WebAssembly capability observations and
shared ISA admission without a SIMD implementation or a Wasm SDK dependency.
The module is available on every supported host, including native applications
that embed a WebAssembly engine. `native.features` and `native` also export it.
The C++20 header `<native/wasm/features.h>` provides the same detector API through
`native::headers`; named modules use the library's C++26 toolchain.

`wasm_feature::simd128` describes standard 128-bit SIMD.
`wasm_feature::relaxed_simd` describes the finalized relaxed-SIMD extension.
They occupy a separate feature family from x86 and ARM. Typed feature sets reject
other families, and admission cannot use native CPU support to authorize Wasm
instructions. An engine's configuration can restrict features independently of
its host processor.

## Observations and admission

A `wasm_capabilities` record contains:

- `present`: features successfully observed as supported.
- `observed`: features whose queries completed, including negative answers.
- `raw`: the original `simd128_observed`, `simd128`, `relaxed_simd_observed` and
  `relaxed_simd` booleans, retained for diagnostics.

Supply answers from the intended runtime and normalize them explicitly:

```cpp
import native.wasm.features;

constexpr auto runtime = native::decode_wasm_capabilities({
  .simd128_observed = true, .simd128 = true,
  .relaxed_simd_observed = true, .relaxed_simd = false,
});
static_assert(native::classify_isa(runtime, native::wasm_feature::simd128).admitted());
static_assert(!native::classify_isa(runtime, native::wasm_feature::relaxed_simd).admitted());
```

The decoder does not infer observations or add prerequisite bits. Admission
requires each bit to be both present and observed, and requesting relaxed SIMD
also requires SIMD128. Inconsistent embedder results therefore cannot bypass the
SIMD128 requirement. Editing `raw` after normalization does not change admission.
Empty requirements remain admissible when optional capabilities are unknown.

Native embedders can instead call `observe_wasm_capabilities(validator)`. The
nonthrowing callback receives `std::span<std::uint8_t const>` and returns
`std::optional<bool>`: `std::nullopt` for failed/unavailable queries, `false` for
observed absence, and `true` for support. Translate engine exceptions or error
codes into `std::nullopt` inside the callback. The observer first validates an
empty module, then the two feature probes independently, and normalizes the
results with the same decoder. It does not instantiate or execute the probes.

Portable C++ has no standard engine capability query.
`observe_wasm_capabilities()` explicitly returns an entirely unknown record;
it does not inspect compiler macros, the host CPU or global adapter state.
On Wasm targets, the `native.features` `observe_cpu()` alias has that same unknown
behavior. Supply runtime observations with `decode_wasm_capabilities` when they
are available. A missing or failed query differs from an observed negative even
though both reject the corresponding optional requirement.

## Optional JavaScript bridge

The installed ES module `share/native/wasm/features.mjs` exports
`observe_wasm_capabilities` and `wasm_feature_probe`. Its absolute installed path
is available as `native_WASM_FEATURES_JS` after CMake `find_package(native)`.
No JavaScript runtime is required to build or use the C++ library.

```js
import { observe_wasm_capabilities } from "./features.mjs";
const raw = observe_wasm_capabilities(); // Current realm's WebAssembly namespace.
// Marshal these four booleans through the application's own embedding interface.
```

An explicit argument selects another WebAssembly namespace or validation adapter.
Missing APIs, exceptions and non-boolean answers leave the affected observation
unknown. A valid empty-module control probe must succeed before feature queries.
Each feature probe then runs independently: a failure querying relaxed SIMD does
not erase a completed SIMD128 observation. A completed `false` is an observed
negative. The adapter does not cache results across engines or configurations.

Both C++ `wasm_feature_probe(wasm_feature)` and JavaScript
`wasm_feature_probe("simd128" | "relaxed_simd")` expose complete, import-free
probe modules. The C++ span has static lifetime; JavaScript returns fresh mutable
bytes. An invalid C++ enum produces an empty span, while an unknown JavaScript
name throws `RangeError`. Embedders can validate the C++ bytes directly with their
engine API and pass the outcomes to the decoder.

The SIMD128 probe contains `v128.const`; the relaxed probe contains the finalized
`i8x16.relaxed_swizzle` opcode, encoded as prefix `0xfd` and unsigned LEB128
subopcode `0x100`. There are no imports, memories, exports or start functions.
[WebAssembly validation](https://webassembly.github.io/spec/js-api/#dom-webassembly-validate)
is sufficient to test acceptance of these instruction sets. The bridge never
instantiates a probe or invokes a Wasm function. The
[binary instruction specification](https://webassembly.github.io/spec/core/binary/instructions.html)
and [relaxed-SIMD opcode table](https://github.com/WebAssembly/relaxed-simd/blob/main/proposals/relaxed-simd/Overview.md)
define the probe encodings.

## What admission establishes

An admitted feature requirement means that the supplied observations establish
support in the selected engine configuration. It does not establish support for
other extensions, application imports or resource limits. The engine still
validates an entire final module before instantiation; placing unsupported
instructions in an uncalled function or behind a runtime branch does not hide
them from validation.

Applications own compilation, loading and invocation of their higher-feature
bodies. This API provides no Wasm `native::simd` specialization or loader policy.
Relaxed SIMD admission also does not strengthen numerical semantics: relaxed
instructions retain their specification-defined sets of allowed results.
