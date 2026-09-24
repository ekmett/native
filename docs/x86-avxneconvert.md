<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# AVX-NE-CONVERT

`import native.x86.avxneconvert;` supplies all 14 AVX-NE-CONVERT instruction
variants. `native.x86` and `native` reexport the module. Public values use
`native::simd`, `native::fp16` and `native::bf16`; intrinsic vector types remain
inside the implementation. Every operation is `noexcept`.

Each row supports `N == 4` and `N == 8`, corresponding to the 128-bit and
256-bit binary32 forms. Pointer operations take explicit `<Arch, N>` arguments;
`cvtneps_bf16` deduces both from its vector argument.

| Operation | Input | Result | Accessible input memory |
| --- | --- | --- | --- |
| `bcstnebf16_ps<Arch,N>(p)` | `bf16 const *` | `simd<float,N,Arch>` | One BF16 object, 2 bytes |
| `bcstnesh_ps<Arch,N>(p)` | `fp16 const *` | `simd<float,N,Arch>` | One FP16 object, 2 bytes |
| `cvtneebf16_ps<Arch,N>(p)` | `bf16 const *` | Even-indexed elements as `simd<float,N,Arch>` | `2*N` BF16 objects, 16 or 32 bytes |
| `cvtneeph_ps<Arch,N>(p)` | `fp16 const *` | Even-indexed elements as `simd<float,N,Arch>` | `2*N` FP16 objects, 16 or 32 bytes |
| `cvtneobf16_ps<Arch,N>(p)` | `bf16 const *` | Odd-indexed elements as `simd<float,N,Arch>` | `2*N` BF16 objects, 16 or 32 bytes |
| `cvtneoph_ps<Arch,N>(p)` | `fp16 const *` | Odd-indexed elements as `simd<float,N,Arch>` | `2*N` FP16 objects, 16 or 32 bytes |
| `cvtneps_bf16(v)` | `simd<float,N,Arch>` | `simd<bf16,N,Arch>` | No pointer input |

Pointers must be nonnull and aligned for their 2-byte element type. No 16-byte
or 32-byte alignment is required. The even/odd forms require the **entire**
interleaved memory operand, including elements not selected for the result.
Constant evaluation checks that same extent. The four-lane narrowing result
has four logical BF16 lanes in a 128-bit register, with all upper bits zero;
`store`/`store_bits` write only those four lanes. The eight-lane result fills the
128-bit register. These storage shapes provide no BF16 arithmetic.

Runtime calls require `x86_feature::avxneconvert`, AVX, enabled XMM/YMM OS state,
and a matching caller compiler target. The instruction feature is CPUID leaf 7,
subleaf 1, EDX bit 5. Clang's `avxneconvert` compiler target additionally enables
AVX2; `target_features<x86>("avxneconvert")` records that compiler prerequisite.
A translation unit built with `-mavxneconvert` also records the feature in
`NATIVE_BASELINE` and its inherited `NATIVE_TARGET_MINIMUM`.
A function target does not change its vector architecture tag. Admission belongs
in the baseline caller, before entering the targeted function:

```cpp
#include <native/attributes.h>
import native.x86.avxneconvert;

constexpr auto requirements = native::target_features<native::x86>("avxneconvert");

native_target("avxneconvert")
void widen_even(native::bf16 const * input, float * output) {
  // input points to at least sixteen BF16 objects, output to eight floats.
  native::cvtneebf16_ps<requirements,8>(input).store(output);
}
// Call widen_even only when
// native::classify_isa(native::observe_x86_capabilities(), requirements).admitted().
```

Constant evaluation implements the same result bits. Strong tags use the
`constexpr` instruction wrapper; weaker tags with the necessary vector storage
have `consteval`-only overloads, rejecting runtime inputs. Baseline SSE2 storage
supports the four-lane forms, and AVX storage supports eight lanes.

## Numerical contract

These operations neither consult nor update MXCSR and raise no floating-point
exceptions. BF16 widening shifts the unchanged 16-bit representation left by
16 bits: it preserves subnormals, signed zeros, infinities and signaling-NaN
encodings. FP16 widening is exact for finite inputs, including subnormals, and
preserves the sign and high payload bits while quieting NaNs.

Binary32-to-BF16 narrowing uses round-to-nearest, ties-to-even, independently
of MXCSR rounding. Binary32 subnormal inputs become signed zero; BF16 subnormal
outputs are flushed to zero. Normal finite values round to the nearest BF16
encoding, including overflow to signed infinity. NaNs retain the sign and high
payload bits and have the BF16 quiet bit set.

These semantics follow the current [Intel SDM revision 089, Volume 2C,
VBCSTNEBF162PS through VCVTNEPS2BF16](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf),
with BF16 widening defined by `make_fp32` in VDPBF16PS. Older extension references
claimed BF16 widening flushes subnormals; that sentence is absent from the current
SDM, whose operation preserves their bits. F16C has a different floating-point
exception and MXCSR contract; these APIs do not change it.

[The focused fixture](../tests/x86_avxneconvert/README.md) checks module imports,
constant semantics, constraints, memory extent, and exact raw/public codegen.
Its hardware test runs only after CPU/OS admission and reports a skip otherwise.
