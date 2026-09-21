# WebAssembly detector checks

The C++ fixtures exercise the public C++20 header, granular module,
`native.features` and `native` hub. They cover every combination of raw probe
outcomes, stale positives after failed observations, prerequisite admission,
unknown versus observed absence, properties, invalid values, family separation
and ordered selection. The C++ callback adapter covers all 27 combinations of
control, SIMD128 and relaxed-SIMD tri-state answers, including failed control
validation and continued independent queries. Producer-only negative compilation rejects mixed-family
source requirements.

The optional Node.js fixture independently decodes the probe module sections,
checks standardized opcodes and stack signatures, compares C++ and JavaScript
bytes, and validates them in the actual engine. It also covers missing APIs,
exceptions, non-boolean results, mutation, and independent query failures through
injected adapters. It reports actual engine answers without assuming relaxed
SIMD is present. No probe is instantiated or executed. Missing Node.js produces
an explicit skipped JavaScript test.

This directory also configures as a standalone installed-package consumer. The
same four C++ interfaces and JavaScript checks run after physical relocation,
using the installed adapter path exported by the package.

See the [detector guide](../../docs/wasm-features.md) for the public contract.
