# ARM DotProd instructions

DotProd accumulates groups of four byte products into 32-bit integer lanes.
`import native.arm.dotprod;` provides twelve wrappers for signed and unsigned
operands; `native.arm` and `native` re-export them on AArch64. Operands and results are `native::simd<T, N, Arch>` values.
All operands share `Arch`; the lane type determines signedness, and `N` is the
number of logical lanes. Raw NEON registers are private implementation details.

| Operation | Accumulator/result | Byte operands | Selected right-hand group |
| --- | --- | --- | --- |
| `sdot<Arch>` | `simd<std::int32_t, 2, Arch>`, `simd<std::int32_t, 4, Arch>` | `simd<std::int8_t, 8, Arch>`, `simd<std::int8_t, 16, Arch>` | Corresponding four bytes |
| `udot<Arch>` | `simd<std::uint32_t, 2, Arch>`, `simd<std::uint32_t, 4, Arch>` | `simd<std::uint8_t, 8, Arch>`, `simd<std::uint8_t, 16, Arch>` | Corresponding four bytes |
| `sdot_lane<Arch, Lane>` | Either signed accumulator | Matching-width left operand, either right width | 0–1 for 8 bytes; 0–3 for 16 bytes |
| `udot_lane<Arch, Lane>` | Either unsigned accumulator | Matching-width left operand, either right width | 0–1 for 8 bytes; 0–3 for 16 bytes |

Each accumulator lane receives four signed or unsigned byte products. The sum
wraps modulo 2^32 for both operations. Signed overflow has the instruction's
wrapping result; it is not C++ signed-overflow undefined behavior. Lane forms
reuse the selected four-byte group for every accumulator lane.

Each function requires `Arch.has(arm_feature::dotprod)` and a Clang `dotprod`
function target. DotProd can be requested independently of RDM, FP16, I8MM or a
CPU-model bundle. Missing feature bits and invalid lane indices are rejected at
compile time; the operations have no runtime dispatch or software fallback.

Before calling the kernel, check both the requested feature and the
translation unit's minimum requirements:

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.dotprod;

constexpr auto required = native::feature_closure(native::arm_feature::dotprod);

__attribute__((target("dotprod")))
native::simd<std::int32_t, 4, required> accumulate(
    native::simd<std::int32_t, 4, required> a,
    native::simd<std::int8_t, 16, required> b,
    native::simd<std::int8_t, 16, required> c) {
  return native::sdot<required>(a, b, c);
}

bool available() {
  auto cpu = native::observe_arm_capabilities();
  return native::classify_isa(cpu, required, NATIVE_TARGET_MINIMUM).admitted();
}
```

The [tests](../tests/arm_dotprod/README.md) exercise all twelve overloads and
every lane through the granular module and hubs. An independent scalar
reference uses unsigned accumulation to check wraparound, boundary values and
random inputs. Assembly checks start with DotProd and RDM disabled, enable
only DotProd per function, and require one SDOT/UDOT instruction with native
lane selection and no helper call. A caller without the target must fail to
compile.

Paired assembly checks compare the public `simd` call with its private native
helper under identical target attributes and register signatures. The complete
instruction sequences must match, including moves, loads, stores and calls.
This checks abstraction overhead in the tested leaf contexts; it is not a
benchmark or a guarantee about surrounding application code.

The instruction mapping follows the [Arm Advanced SIMD intrinsic
reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#dot-product).
