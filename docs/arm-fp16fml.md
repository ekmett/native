# ARM FHM: FP16 products with FP32 accumulation

[ARM instruction sets](arm.md)

## Why use it

Half-precision inputs reduce storage, but rounding each product to half
precision can lose information before accumulation begins. FHM multiplies
binary16 inputs and accumulates directly into binary32, with one fused FP32
rounding per output lane.

## Operations

Import `native.arm.fp16fml`, or use the `native.arm` or `native` hub.
`fmlal<Arch>(acc,a,b)` adds products; `fmlsl<Arch>(acc,a,b)` subtracts them.
`fmlal2` and `fmlsl2` select the upper half of the input vectors.
All operands share `Arch`.

| Operation | Accumulator/result | Multiplicands | Selected input lanes |
| --- | --- | --- | --- |
| `fmlal`, `fmlsl` | `simd<float,2,Arch>` | Two `simd<fp16,4,Arch>` vectors | 0–1 |
| `fmlal2`, `fmlsl2` | `simd<float,2,Arch>` | Two `simd<fp16,4,Arch>` vectors | 2–3 |
| `fmlal`, `fmlsl` | `simd<float,4,Arch>` | Two `simd<fp16,8,Arch>` vectors | 0–3 |
| `fmlal2`, `fmlsl2` | `simd<float,4,Arch>` | Two `simd<fp16,8,Arch>` vectors | 4–7 |

Each name has a `_lane<Arch,Lane>(acc,a,b)` form. It selects the same lanes of
`a` and broadcasts one scalar half from `b`, which may have four or eight half
lanes independently of the output width. `Lane` is a compile-time immediate
within that source's range.

```cpp
#include <native/targets.h>
import native.arm.fp16fml;

#define NATIVE_TARGET_widen "fp16fml"
constexpr auto widen_isa = NATIVE_TARGET_ISA(widen);

NATIVE_TARGET_PUSH(widen)
void widen(float* output, native::fp16 const* a, native::fp16 const* b) noexcept {
  using input = native::simd<native::fp16,8,widen_isa>;
  using result = native::simd<float,4,widen_isa>;
  native::fmlal<widen_isa>(result(0.0f), input::load_memory(a),
    input::load_memory(b)).store_memory(output);
}
NATIVE_TARGET_POP()
```

## Caveats

Each output is one fused binary32 operation. There is no intermediate binary16
product or separately rounded binary32 multiplication. These are FEAT_FHM
operations, with a different contract from ordinary FP16 and BF16 arithmetic.

Runtime calls require `arm_feature::fp16fml` and a `"fp16fml"` caller target.
Admission includes NEON and FP16. The `neon_fp16` preset, BF16 and FCMA do not
supply FHM. Admit the target requirement and `NATIVE_TARGET_MINIMUM` before
entering the function; importing the module does not enable instructions or
perform dispatch.

Runtime arithmetic follows FPCR, leaves it unchanged, and accumulates applicable
exception flags in FPSR. Signed zeros, NaNs, infinities and subnormals retain
the instruction's behavior under the active controls. The wrappers retain
instruction execution and FPSR effects even for discarded results. They do not
mask hardware traps. Surrounding code that observes or changes the environment
needs the compiler's floating-environment support. The compiler memory barrier
orders memory-based environment accesses and adds no CPU memory fence.

Constant evaluation uses nearest-even rounding, gradual inputs and results,
payload-preserving NaNs, IEEE half precision and masked exceptions, with
DN=AH=AHP=FZ=FZ16=FIZ=EBF=0 and no machine status effects. A tag without FHM
permits only `consteval` calls when the SIMD storage types exist. There is no
runtime software fallback.

See the [Arm Neon Intrinsics Reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#fp16-armv84-a)
and [Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
