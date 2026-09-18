# Retired implementation

These files are retained for source history and attribution. They are not built,
installed, or a supported alternative to the C++26 modules. The active SIMD
implementation is under `src/simd/simd/`; the original `simd.hpp` and
`simd_data.hpp` are retired. The old profiler and test driver remain excluded.

`simd/assert.h` and `simd/concepts.h` are unused legacy helpers, excluded from
installation. The assertion helper depends on the unbundled spdlog library;
its contents are retained unchanged here, not maintained as a public API.

The unfinished pretty-printer draft lives under `ein/pretty/`. It has no
maintained consumers and still refers to the original headers; it is preserved
as a design draft, not an API.

The unused forwarding polyfill has been removed. Use `std::forward_like`
from `<utility>` with the supported modern toolchain.

Historical reproduction should use the original revision and its full source
layout; this directory is not a standalone build of the old project.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
