# x86 VAES: parallel AES rounds

[x86 instruction sets](x86.md)

## Why use it

Independent AES blocks can share a vector instruction. VAES performs the same
round on one, two or four blocks, each with its own key. This is useful when a
cipher mode or a batch of messages exposes independent block work.

## Operations

`import native.x86.vaes;` provides operations on
`simd<std::uint8_t,N,Arch>` for `N = 16, 32, 64`. Link `native::native`;
`native.x86` and `native` also export the module. Each call takes `(state, key)`
and returns the same vector type.

| Operation | Round |
| --- | --- |
| `vaesenc<Arch>` | SubBytes, ShiftRows, MixColumns, then round-key XOR |
| `vaesenclast<Arch>` | SubBytes, ShiftRows, then round-key XOR |
| `vaesdec<Arch>` | Inverse SubBytes, inverse ShiftRows, inverse MixColumns, then round-key XOR |
| `vaesdeclast<Arch>` | Inverse SubBytes, inverse ShiftRows, then round-key XOR |

Each consecutive 16-byte group is an independent AES state. State byte
`4 * column + row` follows column order. No work crosses a 128-bit boundary.

## Caveats

Decryption uses AESDEC's equivalent inverse cipher ordering and transformed
middle-round keys. There is no key schedule or cipher mode here. The
[AES module](x86-aes.md) supplies legacy rounds and key helpers.

| Register width | Runtime features | Caller target |
| --- | --- | --- |
| 128 bits | AES, AVX | `"avx,aes"` |
| 256 bits | VAES, AVX | `"avx,vaes"` |
| 512 bits | VAES, AVX512F | `"avx512f,vaes"` |

The 128-bit VEX form does not require the VAES bit. The wider forms need neither
AVX512VL nor AVX512BW, and there are no writemasks. Clang's `vaes` target also
enables AES and AVX2; use `target_features<native::x86>(target)` and admit the
whole compiler target, including the required OS vector state, before entry.
VAES is not implied by the general AVX2 or AVX-512 profiles.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags have `consteval` overloads if their register storage exists: SSE2 for
128 bits, AVX for 256 bits or AVX512F for 512 bits, with prerequisites. Results
preserve the exact tag; weaker tags supply no runtime software fallback.

See Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [VAES header](https://clang.llvm.org/doxygen/vaesintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
