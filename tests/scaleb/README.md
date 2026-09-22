# Native scaling admission

`scaleb`, `masked_scaleb` and `masked_scaleb_zero` are instruction wrappers.
They are callable only on native scaling shapes in the supported arithmetic
kernel profiles: AVX512F for one or sixteen lanes, and AVX512VL for two, three,
four or eight lanes. Logical two- and three-lane values use one masked
128-bit instruction, excluding physical padding from execution. Their constant
evaluation remains available on those same admitted shapes.

The fixture checks positive and negative admission through module and textual
interfaces, including standard arrays and `native::wide`. Scalar, AVX2, NEON,
Wasm and AVX-512 widths without their required VL feature do not acquire software
fallbacks. Plain C++ scalar promotion does not manufacture a scaling instruction.

On x86, 24 raw-intrinsic/public pairs cover unmasked, merge and zero forms, both
with and without AVX512VL, and compare complete instruction sequences. The
runtime environment/rounding oracle remains in `tests/core_regression/scaleb.cc`;
its tests skip when the selected profile has no native scaling instruction.

Configure this fixture with an installed `native` package, or use
`-DNATIVE_SCALEB_SOURCE_PROVIDER=ON` to build the provider from this tree.
