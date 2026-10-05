# ARM NEON: saturation, shifts, bit operations and conversions

[ARM instruction sets](arm.md)

## Why use it

Packed integer kernels often need saturation, per-lane shifts or narrowing
rather than C++'s ordinary arithmetic. These NEON operations expose those
choices directly. The floating-to-integer conversions also give defined results
for every binary32 encoding, including overflow and NaNs.

## Operations

Import `native.arm.neon`, or use the `native.arm` or `native` hub.
Vector operands use `native::simd<T,N,Arch>` with a common `Arch`.
The conversion names are also exported by `native.simd`.

| Operations | Shapes and behavior |
| --- | --- |
| `fcvtzs`, `fcvtzu` | Scalar `float` or `simd<float,N,Arch>` with 1–4 lanes to signed/unsigned 32-bit integers, truncating toward zero |
| `sqadd`, `uqadd`, `sqsub`, `uqsub` | Saturating signed/unsigned add and subtract; 8-, 16-, 32- or 64-bit lanes in 64 or 128 logical bits |
| `sqxtn`, `uqxtn`, `sqxtun` | Saturating signed-to-signed, unsigned-to-unsigned or signed-to-unsigned narrowing; 128 input bits to 64 output bits, with 16→8, 32→16 or 64→32-bit lanes |
| `sqxtn_high`, `uqxtn_high`, `sqxtun_high` | Preserve a 64-bit low argument and append the narrowed source, producing 128 bits |
| `sqdmulh`, `sqrdmulh` | Saturating signed doubled multiply-high, without/with rounding; 16- or 32-bit lanes in 64 or 128 logical bits |
| `sshl`, `ushl`, `srshl`, `urshl` | Per-lane signed-count shifts, without/with right-shift rounding; all integer widths in 64 or 128 logical bits |
| `sqshl`, `uqshl`, `sqrshl`, `uqrshl` | Corresponding saturating left shifts, without/with right-shift rounding |
| `clz` | Count leading zeros in signed or unsigned 8-, 16- or 32-bit lanes; zero returns the lane width |
| `cls` | Count leading sign bits after the sign bit in signed 8-, 16- or 32-bit lanes; zero and minus one return width minus one |
| `rbit` | Reverse bits within each signed or unsigned byte lane |
| `rev16`, `rev32`, `rev64` | Reverse integer lanes within each 16-, 32- or 64-bit block; lane width must be smaller than block width |

The six bit operations accept 64 or 128 logical bits and return the same vector
type. `rev32` on halfwords swaps adjacent halfwords without reversing their
internal bits or bytes. `rev64` reverses byte, halfword or word lanes within
each separate 64-bit block. These bit operations have no 64-bit-lane forms.

Shift-count vectors always have signed elements of the same width as the value
lanes. Only the signed low byte of each count is used. Positive counts shift
left; negative counts shift right. Counts are not reduced modulo lane width.
Arithmetic right shifts extend the sign; logical right shifts insert zeros.
Rounding adds one when the most significant discarded bit is set. Excessive
shifts retain the instruction's zero/sign-extension, rounding and saturation
behavior.

`fcvtzs` returns `std::int32_t` or `simd<std::int32_t,N,Arch>`. NaNs produce
zero; positive overflow and infinity produce `INT32_MAX`, negative overflow
and infinity produce `INT32_MIN`. `fcvtzu` returns the corresponding unsigned
type: NaNs and negative inputs produce zero; positive overflow produces
`UINT32_MAX`. Fractional inputs truncate independently of the rounding mode.

Binary32 and binary16 vector comparisons use the floating comparison
instructions. `!=` complements equality; `<` and `<=` reverse the operands
of `>` and `>=`. NaNs compare unordered and signed zeros compare equal.

```cpp
#include <cstdint>
import native.arm.neon;

constexpr auto requirement = native::feature_closure(native::arm_feature::neon);
using samples = native::simd<std::int16_t,8,requirement>;

__attribute__((target("neon")))
samples saturated_sum(samples a, samples b) {
  return native::sqadd(a, b);
}
```

## Caveats

Runtime integer operations require NEON storage, `arm_feature::neon` and a
`"neon"` caller target. Scalar conversions default to the compiler baseline;
a one-lane conversion also accepts scalar storage. Multi-lane conversions
require NEON. The separate `convert<std::int32_t>` retains its finite,
representable-input precondition.

Doubled multiply-high takes the high half of the full doubled product; the
rounded form adds half a unit before truncation. Minimum-times-minimum saturates
to the maximum signed lane. Narrowing saturates before changing width.

Saturating runtime instructions can set sticky `FPSR.QC`, including when results
are discarded. They preserve existing QC and do not save, clear or restore
FPSR. Bit operations leave FPSR unchanged. Conversion and comparison status
effects belong to the executed instruction. Comparisons honor active FPCR
denormal controls; equality raises invalid for signaling NaNs, and ordered
inequalities also raise invalid for quiet NaNs. Existing sticky flags remain
set even when a comparison result is discarded.

Constant evaluation computes values or masks without machine status effects.
Feature-absent `consteval` overloads exist for saturation and shift operations
when their storage shapes are available; runtime operands are rejected. The six
bit operations retain their NEON feature requirement at constant evaluation.

Two- and three-lane binary32 values and masks retain zero-padding normalization.
On little-endian AArch64 a full-register floating comparison needs only the
comparison instruction (`!=` also inverts the result). Big-endian Clang 23
requires six extra register permutations around these wrappers relative to ACLE.
Its two-lane 64-bit saturating add/subtract and eight variable-shift forms require
five extra register permutations per operation. These endian adjustments add no
memory accesses or scalar fallback.

See the [Arm ACLE intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html).
