# x86 AVX-NE-CONVERT: FP16 and BF16 data conversion

[x86 instruction sets](x86.md)

## Why use it

FP16 and BF16 reduce storage and memory traffic while calculations can remain
in binary32. AVX-NE-CONVERT widens those stored values, selects either half of
an interleaved stream, or broadcasts one value directly from memory. It also
narrows binary32 to BF16 with a fixed rounding rule.

## Operations

`import native.x86.avxneconvert;` provides the operations below; `native.x86`
and `native` re-export them. `N` is 4 or 8. Pointer operations take explicit
`<Arch,N>` arguments; `cvtneps_bf16` deduces the shape from its vector.
All calls are `noexcept`.

| Operation | Input | Result | Input extent |
| --- | --- | --- | --- |
| `bcstnebf16_ps<Arch,N>(p)` | `bf16 const *` | `simd<float,N,Arch>` broadcast | One BF16 object |
| `bcstnesh_ps<Arch,N>(p)` | `fp16 const *` | `simd<float,N,Arch>` broadcast | One FP16 object |
| `cvtneebf16_ps<Arch,N>(p)` | `bf16 const *` | Even elements as `simd<float,N,Arch>` | `2*N` BF16 objects |
| `cvtneeph_ps<Arch,N>(p)` | `fp16 const *` | Even elements as `simd<float,N,Arch>` | `2*N` FP16 objects |
| `cvtneobf16_ps<Arch,N>(p)` | `bf16 const *` | Odd elements as `simd<float,N,Arch>` | `2*N` BF16 objects |
| `cvtneoph_ps<Arch,N>(p)` | `fp16 const *` | Odd elements as `simd<float,N,Arch>` | `2*N` FP16 objects |
| `cvtneps_bf16(v)` | `simd<float,N,Arch>` | `simd<bf16,N,Arch>` | Register input |

```cpp
#include <hint.h>
import native.x86.avxneconvert;

constexpr auto arch = native::target_features<native::x86>("avxneconvert");

// Enter after admitting arch; input has sixteen BF16 objects, output eight floats.
hint_target("avxneconvert")
void widen_even(native::bf16 const * input, float * output) {
  native::cvtneebf16_ps<arch,8>(input).store(output);
}
```

The four-lane BF16 result occupies the low bits of a 128-bit register with zero
padding. `store` and `store_bits` write only its four logical lanes. These
storage types do not provide BF16 arithmetic.

## Caveats

Pointer inputs must be nonnull and aligned for their two-byte element type.
The even/odd forms require the entire 16- or 32-byte interleaved operand,
including elements not selected. Constant evaluation requires the same extent.

These instructions neither consult nor update MXCSR and raise no FP exceptions.
BF16 widening shifts the representation left 16 bits, preserving subnormals and
signaling-NaN encodings. FP16 widening is exact for finite inputs, including
subnormals; it preserves the sign and high NaN payload bits while quieting NaNs.

BF16 narrowing uses nearest-even rounding. Binary32 subnormal inputs become
signed zero and BF16 subnormal outputs are flushed to zero. Overflow can round
to signed infinity. NaNs retain sign and high payload bits with the quiet bit
set. [F16C](x86-f16c.md) has different MXCSR and exception semantics.

Runtime calls need AVX-NE-CONVERT, AVX and enabled XMM/YMM state. Clang's
`avxneconvert` target additionally enables AVX2; `target_features` includes it.
Admit the full caller target before entry. Feature-bearing overloads are
`constexpr` with native runtime paths; weaker tags are `consteval`-only, with
SSE2 storage for four lanes or AVX storage for eight lanes.

See the current [Intel conversion instruction definitions](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
