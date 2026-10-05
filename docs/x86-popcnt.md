# x86 POPCNT: scalar population counts

[x86 instruction sets](x86.md)

## Why use it

A population count gives the size of a bitset without visiting each bit.
Applied to XOR it gives Hamming distance; applied to AND it counts an
intersection. POPCNT exposes that operation on a single unsigned integer.

## Operations

`import native.x86.popcnt;` provides `native::popcnt<Arch>(value)`.
`native.x86` and `native` re-export it. The overloads accept `std::uint16_t`,
`std::uint32_t` or `std::uint64_t` and return the same unsigned type.

Zero returns zero. An all-one value returns its width. Calls are `noexcept`.

## Caveats

Runtime calls need `x86_feature::popcnt`, a `"popcnt"` caller target and CPU
admission. POPCNT is independent of SSE and BMI and needs no vector OS state.
The 16-bit overload counts a zero-extended value through Clang's 32-bit
intrinsic; the public result remains a 16-bit count.

`Arch` is an `isa<x86>`. Its scalar default is the provider's
`NATIVE_BASELINE`, unaffected by target attributes in the importer.
Feature-bearing overloads are `constexpr` with native runtime paths;
tags without POPCNT have `consteval` overloads only. No instruction-flags or
exact-encoding promise follows from the spelling.

Generic `popcount` covers portable scalar and vector code. Use `popcnt` when
the x86 feature is part of the algorithm's target contract.

See Intel's [POPCNT instruction entry](https://cdrdv2-public.intel.com/782151/253667-sdm-vol-2b.pdf#page=401).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
