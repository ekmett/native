# Vector AES rounds

`import native.x86.vaes;` provides AES rounds over one, two or four independent
128-bit states. Link `native::native`; the `native.x86` and `native` hubs also
export the module. Each operation takes `(state, key)` and returns the same
`simd<std::uint8_t,N,Arch>` type, where `N` is 16, 32 or 64.

| Operation | Round |
| --- | --- |
| `vaesenc<Arch>` | SubBytes, ShiftRows, MixColumns, then XOR the round key |
| `vaesenclast<Arch>` | SubBytes, ShiftRows, then XOR the round key |
| `vaesdec<Arch>` | Inverse SubBytes, inverse ShiftRows, inverse MixColumns, then XOR the round key |
| `vaesdeclast<Arch>` | Inverse SubBytes, inverse ShiftRows, then XOR the round key |

Consecutive groups of 16 bytes are independent AES states, each with its own
16-byte round key. State byte `4*column+row` uses the FIPS 197 column order.
No work crosses a 128-bit boundary. Decryption follows the equivalent inverse
cipher ordering of the AESDEC instruction, including its transformed middle
round keys; these functions do not generate a key schedule.

| Register width | Architecture requirements | Caller target |
| --- | --- | --- |
| 128 bits | AES and AVX | `"avx,aes"` |
| 256 bits | VAES and AVX | `"avx,vaes"` |
| 512 bits | VAES and AVX512F | `"avx512f,vaes"` |

Use `target_features<native::x86>(target)` for the selected caller. The
128-bit VEX encoding is available through AES plus AVX and does not require
the VAES feature bit. The 256-bit form uses VEX; the 512-bit form uses EVEX.
Neither requires AVX512VL or AVX512BW. The instructions have no writemasks.
The existing [AES module](x86-aes.md) supplies legacy 128-bit rounds and key
schedule primitives without requiring AVX.

`x86_feature::vaes` is decoded from CPUID leaf 7, subleaf 0, ECX bit 9, with AVX
as a storage prerequisite. Clang 23's `vaes` target additionally enables AES
and AVX2; `target_features` includes that compiler bundle while
`feature_closure(x86_feature::vaes)` expresses the hardware prerequisite.
Admit the full compiler target before entry, including OS AVX state for VEX
and OS AVX-512 state for the 512-bit EVEX form. VAES is not added to the
general `avx2` or `avx512` profiles.

Instruction-bearing tags use `constexpr` overloads with native runtime paths.
Tags lacking an instruction feature use `consteval` overloads and still need
complete storage: SSE2 for 128 bits, AVX for 256 bits and AVX512F for 512 bits,
including prerequisites. Constant evaluation computes exact AES rounds and
preserves the architecture tag. Raw registers, mixed tags and other vector
shapes are rejected. There is no runtime software fallback.

The [fixture](../tests/x86_vaes/README.md) documents independent FIPS table
references, block-isolation tests, admission, rejection controls and raw/public
assembly comparisons. Semantics and encoding requirements follow Intel's
[Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).
Intrinsic signatures follow Clang's
[VAES header](https://clang.llvm.org/doxygen/vaesintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
