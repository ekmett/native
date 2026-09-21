# ARM RDM instructions

RDM combines a signed fixed-point product with an accumulator, then rounds
and saturates the result. `import native.arm.rdm;` provides `sqrdmlah<Arch>`,
`sqrdmlsh<Arch>` and their `_lane<Arch, Lane>` forms. The AArch64 `native.arm`
and `native` hubs re-export them.

Scalar operations use ordinary C++ integers. Vector operands and results use
`native::simd<T, N, Arch>`, including the vector source of a scalar lane operation.
Scalar `sqrdmlah(a,b,c)` and `sqrdmlsh(a,b,c)` calls may omit `Arch`, using the
owning module's `NATIVE_BASELINE`. Runtime calls require that baseline to contain
RDM. This default is captured at module
compilation; an importing function's target attribute does not change it. An
explicit ISA remains available for optional target leaves. Clang 23 reports
`__ARM_FEATURE_QRDMX` for an Armv8.1-A baseline, but not for `armv8-a+rdm`
alone; the latter therefore requires an explicit ISA with the current macro-based
baseline snapshot. Vector forms continue to deduce `Arch` from their operands,
and lane forms retain `<Arch, Lane>` order.
Every vector operand shares the operation's `Arch`. Each add/subtract operation has
eighteen overloads: six shapes, each with an ordinary form and two selectable
right-hand vector widths.

| Accumulator, left operand and result | Right operand in ordinary form | Right operand in lane form |
| --- | --- | --- |
| `int16_t` | `int16_t` | `simd<std::int16_t, 4, Arch>` or `simd<std::int16_t, 8, Arch>` |
| `simd<std::int16_t, 4, Arch>`, `simd<std::int16_t, 8, Arch>` | Same vector type | `simd<std::int16_t, 4, Arch>` or `simd<std::int16_t, 8, Arch>` |
| `int32_t` | `int32_t` | `simd<std::int32_t, 2, Arch>` or `simd<std::int32_t, 4, Arch>` |
| `simd<std::int32_t, 2, Arch>`, `simd<std::int32_t, 4, Arch>` | Same vector type | `simd<std::int32_t, 2, Arch>` or `simd<std::int32_t, 4, Arch>` |

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

Runtime saturation sets the calling thread's sticky FPSR.QC bit. A nonsaturating result
does not clear it. The wrappers do not modify FPCR, clear FPSR or promise
`const`/`pure` semantics. Even an unused result executes the instruction.

All scalar, vector and lane forms support constant evaluation. The constant
branch computes the value using the formula above; it neither observes nor
modifies FPSR and cannot simulate a thread's sticky QC state. An ISA without RDM
exposes only immediate (`consteval`) overloads, with the same exact operand types
and available SIMD storage shapes. Calls with runtime operands are rejected.

Each runtime wrapper requires `Arch.has(arm_feature::rdm)` and a Clang `rdm` function
target. Before calling it, check that
`classify_isa(observe_arm_capabilities(), feature_closure(arm_feature::rdm),
NATIVE_TARGET_MINIMUM)` admits execution. Feature closure adds NEON to these
requirements; DotProd, FP16 and the rest of the Armv8.1-A bundle are unnecessary.
Missing feature bits reject runtime operands, and invalid lanes are always
rejected. The operations have no runtime software fallback or dispatch.

Clang 23's ACLE wrappers and underlying RDM builtins require the broader
`v8.1a` target even though the instructions can be enabled with `rdm` alone.
The implementation therefore uses one exact volatile inline instruction per
wrapper. Halfword lane operands use the instruction's V0–V15 register constraint;
64-bit source vectors are extended with unused lanes to satisfy Clang's 128-bit
constraint. Big-endian 128-bit operands correct Clang's vector-to-byte
inline-assembly conversion; 64-bit operands retain their scalar-register bit
mapping. Lane indices retain ACLE numbering. This does not enable any
additional ISA feature. Generated assembly is compared with ACLE to check the
byte-order handling; native big-endian execution remains untested.

The [tests](../tests/arm_rdm/README.md) cover all thirty-six overloads, every
legal lane, signed ties, cancellation, saturation and FPSR.QC effects. Separate
compilation checks exercise the feature constraints and instruction selection
from a baseline AArch64 translation unit.

Paired assembly checks compare the public `simd` call with its private native
helper under identical target attributes and register signatures. The complete
instruction sequences must match, including moves, loads, stores and calls.
This checks abstraction overhead in the tested leaf contexts; it is not a
benchmark or a guarantee about surrounding application code.

Primary references are the [Arm Advanced SIMD intrinsic
reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html#sqrdmlah-intrinsics-from-armv81-a)
and [LLVM's big-endian NEON representation
notes](https://llvm.org/docs/BigEndianNEON.html).

Generated properties cover all 120 scalar, vector and selected-lane forms.
Forty constant-evaluated cases per form include signed extremes, halfway ties,
minimum-times-minimum cancellation and final saturation. Results agree with an
independent 128-bit formula and actual native execution. Additional runtime
cases accept `NATIVE_TEST_SEED` and `NATIVE_TEST_CASES`, reporting seed, case index
and operands on failure. The separate native status tests retain sticky-QC and
discarded-result checks.
