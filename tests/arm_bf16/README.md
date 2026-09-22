# BF16 instruction tests

This fixture exercises all thirteen overloads and all forty-one concrete
vector/lane forms in `native.arm.bf16`, through the granular module,
ARM hub and main hub. The runtime driver checks observed CPU capabilities
before entering BF16 code. Missing BF16 returns CTest skip code 77. A separate
enhanced test also returns 77 when EBF16 is unavailable, without setting EBF.

The BFDOT bank reuses the independent exact-rational reference in
`tests/neon_bf16`: 2,048 cases across every shape and lane. The matrix bank has
512 cases, including all 64 input-position pairs and cancellation across the
two ordered pair accumulations. Another 512 cases check fused multiply-add.
The Python generator performs no host floating-point arithmetic and runs
326,501 self-checks. `check_reference.py` regenerates the matrix/FMA banks and
compares them byte-for-byte with the marked region in `checks.h`.
`generate_reference.py` updates that region; `--output PATH` emits its contents
to a separate file.

Default BF16 runs under all four rounding settings, both FZ settings and both
DN settings; those controls must not alter its defined arithmetic. Enhanced
BF16 runs the corresponding cases only when advertised. All calls preserve
FPCR. Dot and matrix calls must preserve seeded FPSR bits, including when
results are discarded. Every widening multiply-add form checks ordinary
invalid-operation, overflow, underflow, inexact and input-denormal flags,
sticky prior flags, NaN payload controls and discarded results. AFP checks
run only when AH/FIZ read back as writable. They cover BFMLAL's forced rounding,
flushing and suppressed flags, as well as dot/matrix default-NaN signs.
Changing FPCR between identical calls must produce the appropriate new result.
The main-hub consumer also checks this through the high-level `dot2` API;
its AFP witness failed with the earlier intrinsic-based implementation.

The scalar banks cover AH=FIZ=0; the AFP checks use selected witnesses. They do
not exhaust all FPCR/trap combinations. Default BF16 and AFP cases executed on
Apple M3 with Clang 23. EBF16 was not advertised, so there is no native enhanced
arithmetic result from that run.

Assembly checks compile from a generic Armv8-A baseline with BF16 disabled.
Each public wrapper is paired with a raw helper under the same caller target.
All 41 pairs must have identical instruction sequences: the BF16 primitive
and return only, with no loads, stores, stack references or helper calls.
Indexed four-element sources leave their unused upper half undefined, avoiding
an unnecessary duplication. Ordinary baseline code must remain free of BF16 instructions.
The checks also retain discarded instructions and separate calls across an
FPCR write. Fifteen compiler-failure cases cover missing targets, missing
features, wrong formats and invalid lanes. They test ordinary calls, so a
conversion to another vector type cannot hide a rejected overload.

The big-endian fixture compiles the actual instruction header with minimal
metadata substitutes for a freestanding target. A symbolic interpreter checks
all 41 wrappers' input bytes, selected elements and stored results, plus five
ACLE vector controls. This is compiler-layout validation, not big-endian
hardware execution or an arithmetic oracle.

To check an installed package, install and physically move its prefix, then
configure this directory with `native_DIR` pointing into the moved package.
The standalone build uses only the granular, ARM and main modules; private
instruction-layout and compiler-control fixtures remain source-tree checks.

See the [instruction guide](../../docs/arm-bf16.md) for operand layout and
arithmetic contracts.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

Public calls use `simd<float,N,Arch>` and `simd<bf16,M,Arch>` throughout.
The oracle adapters convert raw fixture storage at that boundary and assert the
result feature tag. Compile failures reject raw vectors and mismatched tags.
The two-lane bridge also verifies zeroed padding. Private instruction helpers
retain the separate compiler layout comparison; public code generation covers
every instruction form through the module API.
