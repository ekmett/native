# ARM DotProd instructions

`import native.arm.dotprod;` exposes twelve raw NEON wrappers. The AArch64
`native.arm` and `native` hubs re-export them. These functions operate on native
register types, independent of the high-level `simd` shape and CPU profiles.

| Operation | Accumulator/result | Byte operands | Selected right-hand group |
| --- | --- | --- | --- |
| `sdot<Arch>` | `int32x2_t`, `int32x4_t` | `int8x8_t`, `int8x16_t` | Corresponding four bytes |
| `udot<Arch>` | `uint32x2_t`, `uint32x4_t` | `uint8x8_t`, `uint8x16_t` | Corresponding four bytes |
| `sdot_lane<Arch, Lane>` | Either signed accumulator | Matching-width left operand, either right width | 0–1 for 8 bytes; 0–3 for 16 bytes |
| `udot_lane<Arch, Lane>` | Either unsigned accumulator | Matching-width left operand, either right width | 0–1 for 8 bytes; 0–3 for 16 bytes |

Each accumulator lane receives four signed or unsigned byte products. The sum
wraps modulo 2^32 for both operations. Signed overflow has the instruction's
wrapping result; it is not C++ signed-overflow undefined behavior. Lane forms
reuse the selected four-byte group for every accumulator lane.

Each function requires `Arch.has(arm_feature::dotprod)` and a Clang `dotprod`
function target. Neither a CPU model nor RDM, FP16, I8MM or another architecture
bundle is required. Invalid lane indices and missing feature bits do not
participate in overload resolution. There is no runtime dispatch or fallback
inside an operation.

A caller admits the requested feature together with its translation-unit
minimum before entering the targeted kernel:

```cpp
#include <arm_neon.h>
#include <native/targets.h>
import native.arm.dotprod;

constexpr auto required = native::feature_closure(native::arm_feature::dotprod);

__attribute__((target("dotprod")))
int32x4_t accumulate(int32x4_t a, int8x16_t b, int8x16_t c) {
  return native::sdot<required>(a, b, c);
}

bool available() {
  auto cpu = native::observe_arm_capabilities();
  return native::classify_isa(cpu, required, NATIVE_TARGET_MINIMUM).admitted();
}
```

`tests/arm_dotprod` exercises all twelve overloads and every lane through the
header, granular module and omnibus module. An independent scalar oracle uses
unsigned accumulation and covers directed wraparound plus edge/random inputs.
Baseline code-generation fixtures disable DotProd and RDM globally, enable only
DotProd per function, and require exactly one SDOT/UDOT instruction, native lane
selection, and no helper call. A negative caller without the target must fail
compilation. See [the fixture](../tests/arm_dotprod/README.md) for scope.

The instruction mapping follows the [Arm Advanced SIMD intrinsic
reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#dot-product).
