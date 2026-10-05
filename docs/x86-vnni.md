# x86 AVX-VNNI/AVX-512VNNI: integer dot products

[x86 instruction sets](x86.md)

## Why use it

Quantized matrix and convolution kernels repeatedly multiply bytes or words
and accumulate into wider integers. VNNI groups those small products and their
addition into one operation, keeping 32-bit accumulators in vectors. The
signedness extensions support more combinations without rebiasing operands.

## Operations

`import native.x86.vnni;` exports operations through `native::native`.
`native.x86` and `native` re-export the module. Calls take
`<Arch>(accumulator, a, b)` with equal register widths and matching tags.
Each result lane sums four byte products or two word products.

| Operations | Input `a` | Input `b` | Products per lane | Saturation |
| --- | --- | --- | --- | --- |
| `dpbusd`, `dpbusds` | unsigned byte | signed byte | 4 | Signed 32-bit |
| `dpwssd`, `dpwssds` | signed word | signed word | 2 | Signed 32-bit |
| `dpbssd`, `dpbssds` | signed byte | signed byte | 4 | Signed 32-bit |
| `dpbsud`, `dpbsuds` | signed byte | unsigned byte | 4 | Signed 32-bit |
| `dpbuud`, `dpbuuds` | unsigned byte | unsigned byte | 4 | Unsigned 32-bit |
| `dpwsud`, `dpwsuds` | signed word | unsigned word | 2 | Signed 32-bit |
| `dpwusd`, `dpwusds` | unsigned word | signed word | 2 | Signed 32-bit |
| `dpwuud`, `dpwuuds` | unsigned word | unsigned word | 2 | Unsigned 32-bit |

The second name in each pair saturates. The first returns the low 32 bits of
the complete sum. Accumulators and results are `simd<std::int32_t,N,Arch>`,
except the unsigned-by-unsigned pairs, which use `std::uint32_t`. Byte sources
have `4*N` lanes, word sources `2*N`. `N` is 4, 8 or 16 where supported.

The core names `dpbusd`, `dpbusds`, `dpwssd` and `dpwssds` also provide
`mask_NAME<Arch>(accumulator, mask, a, b)` to retain inactive accumulators and
`maskz_NAME<Arch>(mask, accumulator, a, b)` to clear them. Masks are
`predicate<N,Arch>` for the 32-bit result lanes.

## Caveats

Saturation clamps the complete mathematical sum, including the accumulator.
Two products of `-32768 * -32768` total `2147483648`; an accumulator of `-1`
brings that to `INT32_MAX`. Saturating the product sum first loses that result.
The unsigned-by-unsigned saturating forms clamp to `UINT32_MAX`; the others
interpret the accumulator as signed. FP status and integer flags are unchanged.

| Forms | Widths | Runtime features |
| --- | --- | --- |
| Core, unmasked VEX | 128, 256 | AVX-VNNI |
| Core, EVEX including masks | 128, 256 | AVX512F, AVX512VNNI, AVX512VL |
| Core, EVEX including masks | 512 | AVX512F, AVX512VNNI |
| INT8 signedness extensions | 128, 256 | AVX-VNNI-INT8 |
| INT16 signedness extensions | 128, 256 | AVX-VNNI-INT16 |

Short unmasked core calls prefer AVX-VNNI when the tag supports it; otherwise
they use AVX512VNNI. The signedness extensions have neither masks nor 512-bit
forms. Basic AVX-VNNI does not enable them.

Use `target_features` for the matching compiler target: `avxvnni`,
`avx512vnni` with `avx512vl` for short forms, `avxvnniint8` or `avxvnniint16`.
The VEX targets also enable AVX2. Admit all compiler prerequisites and OS vector
state before entry; VEX needs XMM/YMM and EVEX additionally needs opmask/ZMM.

Feature-bearing overloads are `constexpr` with native runtime paths. Weaker
tags have `consteval` overloads when their SSE2, AVX or AVX512F storage exists
for the chosen width. Inputs must use the documented signedness, shape and tag;
there is no software runtime fallback.

See Intel's [instruction reference](https://cdrdv2-public.intel.com/835757/325383-sdm-vol-2abcd.pdf)
and Clang's [core](https://clang.llvm.org/doxygen/avxvnniintrin_8h_source.html),
[INT8](https://clang.llvm.org/doxygen/avxvnniint8intrin_8h_source.html) and
[INT16](https://clang.llvm.org/doxygen/avxvnniint16intrin_8h_source.html) headers.
