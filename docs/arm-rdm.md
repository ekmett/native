# ARM RDM instructions

`import native.arm.rdm;` exposes `sqrdmlah<Arch>`, `sqrdmlsh<Arch>` and their
`_lane<Arch, Lane>` forms. The AArch64 `native.arm` and `native` hubs re-export
them. These are raw register instructions independent of high-level `simd`
shapes and CPU profiles.

Each add/subtract operation has eighteen overloads: six scalar/vector shapes,
each with an ordinary form and two selectable right-hand vector widths.

| Accumulator, left operand and result | Right operand in ordinary form | Right operand in lane form |
| --- | --- | --- |
| `int16_t` | `int16_t` | `int16x4_t` or `int16x8_t` |
| `int16x4_t`, `int16x8_t` | Same vector type | `int16x4_t` or `int16x8_t` |
| `int32_t` | `int32_t` | `int32x2_t` or `int32x4_t` |
| `int32x2_t`, `int32x4_t` | Same vector type | `int32x2_t` or `int32x4_t` |

`Lane` must be in the right operand's range. Each result lane follows the signed
fixed-point operation, for element width `w`:

```text
sqrdmlah(a,b,c) = signed_saturate_w(floor((a*2^w + 2*b*c + 2^(w-1)) / 2^w))
sqrdmlsh(a,b,c) = signed_saturate_w(floor((a*2^w - 2*b*c + 2^(w-1)) / 2^w))
```

The full product and accumulator combine before rounding and final saturation.
Ties round toward positive infinity. In particular, the exceptional
minimum-times-minimum product can cancel a negative accumulator without
intermediate saturation. These functions cannot be replaced by a saturating
multiply followed by a separate saturating add/subtract.

Saturation sets the calling thread's sticky FPSR.QC bit. A nonsaturating result
does not clear it. The wrappers do not modify FPCR, clear FPSR or promise
`const`/`pure` semantics. Even an unused result executes the instruction.

Each wrapper requires `Arch.has(arm_feature::rdm)` and a Clang `rdm` function
target. Before entering it, admit
`classify_isa(observe_arm_capabilities(), feature_closure(arm_feature::rdm),
NATIVE_TARGET_MINIMUM)`. The admission includes NEON through feature closure;
DotProd, FP16 and a whole Armv8.1-A feature bundle are not required. Missing
feature bits and invalid lanes reject during overload resolution. There is no
software fallback or runtime dispatch inside the operation.

Clang 23's ACLE wrappers and underlying RDM builtins require the broader
`v8.1a` target even though the instructions can be enabled with `rdm` alone.
The implementation therefore uses one exact volatile inline instruction per
wrapper. Halfword lane operands use the instruction's V0–V15 register constraint;
64-bit source vectors are extended with unused lanes to satisfy Clang's 128-bit
constraint. Big-endian 128-bit operands correct Clang's vector-to-byte
inline-assembly conversion; 64-bit operands retain their scalar-register bit
mapping. Lane indices retain ACLE numbering. This does not enable any
additional ISA feature. The byte-order branch has generated-assembly comparison against ACLE,
not native big-endian runtime qualification.

The [fixture](../tests/arm_rdm/README.md) covers all thirty-six overloads,
every legal lane, signed ties, cancellation, saturation and FPSR.QC effects.
It separately checks feature participation and code generation from a genuine
baseline AArch64 translation unit.

Primary references are the [Arm Advanced SIMD intrinsic
reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#sqrdmlah-intrinsics-from-armv81-a)
and [LLVM's big-endian NEON representation
notes](https://llvm.org/docs/BigEndianNEON.html).
