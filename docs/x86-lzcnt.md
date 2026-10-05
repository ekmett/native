# x86 LZCNT: leading-zero counts

[x86 instruction sets](x86.md)

## Why use it

The highest set bit determines an unsigned integer's magnitude. A leading-zero
count turns that position into a direct calculation for normalization,
bit-width selection and radix bucketing, including a defined result for zero.

## Operations

`import native.x86.lzcnt;` provides `native::lzcnt<Arch>(value)`.
`native.x86` and `native` re-export it. The overloads accept `std::uint16_t`,
`std::uint32_t` or `std::uint64_t` and return the same unsigned type.

Zero returns 16, 32 or 64; a value with its highest bit set returns zero.
All calls are `noexcept`.

## Caveats

Runtime calls need `x86_feature::lzcnt`, a `"lzcnt"` caller target and CPU
admission. LZCNT is independent of BMI and needs no vector OS state. On a CPU
without the feature, its encoding can execute as BSR, with different semantics.

`Arch` is an `isa<x86>`. The scalar default is the provider's
`NATIVE_BASELINE`; a caller target attribute does not change that default.
Feature-bearing overloads are `constexpr` with native runtime paths. Tags
without LZCNT have `consteval` overloads only. The wrappers return the count,
without an instruction-flags contract.

Use `std::countl_zero` when the algorithm should work across architectures;
`lzcnt` is the explicit x86 feature-gated spelling.

See Intel's [LZCNT instruction entry](https://cdrdv2-public.intel.com/922480/253666-092-sdm-vol-2a.pdf#page=696).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
