# NEON integer instructions and conversions

`import native.arm.neon;`, `import native.arm;` and `import native;` expose typed
NEON integer instructions on AArch64. Operands use `native::simd<T,N,Arch>`;
all operands retain the same `Arch`, which must contain `arm_feature::neon` for
runtime calls. The instruction target is `neon`, with no additional extension
bit. The public names follow the architecture mnemonics used by the other ARM
instruction modules.

| Operations | Shapes and behavior |
|---|---|
| `fcvtzs` | Binary32 to signed 32-bit integer, truncating toward zero; ordinary `float` and `simd<float,N,Arch>` with 1–4 lanes |
| `fcvtzu` | The corresponding unsigned conversion; NaNs and negative inputs produce zero, positive overflow produces `UINT32_MAX` |
| `sqadd`, `uqadd`, `sqsub`, `uqsub` | Saturating signed/unsigned addition and subtraction; 8-, 16-, 32- and 64-bit lanes in 64 or 128 logical bits |
| `sqxtn`, `uqxtn`, `sqxtun` | Saturating signed-to-signed, unsigned-to-unsigned and signed-to-unsigned narrowing; 128 input bits become 64 output bits, with 16→8, 32→16 or 64→32-bit lanes |
| `sqxtn_high`, `uqxtn_high`, `sqxtun_high` | Preserve a 64-bit low argument and append the narrowed source above it, producing 128 bits |
| `sqdmulh`, `sqrdmulh` | Saturating signed doubled multiply-high, without/with rounding; 16- or 32-bit lanes in 64 or 128 logical bits |
| `sshl`, `ushl`, `srshl`, `urshl` | Per-lane signed-count shifts, without/with right-shift rounding; all integer widths in 64 or 128 logical bits |
| `sqshl`, `uqshl`, `sqrshl`, `uqrshl` | The corresponding saturating left-shift forms, without/with right-shift rounding |

A shift-count vector always has signed elements of the same width as the value
lanes, including unsigned value instructions. Only the signed low byte of each
count lane is used. Positive counts shift left; negative counts shift right.
Counts are not reduced modulo the element width. Arithmetic right shifts extend
the sign, logical right shifts insert zero, and rounding adds one when the most
significant discarded bit was set. Excessive shifts retain the architectural
zero/sign-extension, rounding and saturation behavior.

`fcvtzs` returns `std::int32_t` for a
scalar float, or `simd<std::int32_t,N,Arch>` for a vector. NaNs produce zero;
positive overflow and positive infinity produce `INT32_MAX`, while negative
overflow and negative infinity produce `INT32_MIN`. Fractional inputs truncate
toward zero independently of the rounding mode. Unlike an ordinary C++ cast,
the instruction has defined results for every binary32 encoding.

Both conversion names are exported by `native.simd`. The scalar overload
defaults its architecture to the compiler baseline; a one-lane vector also
accepts the scalar architecture. Multi-lane forms require
NEON. Runtime calls use `vcvts_s32_f32` for scalar storage and
`vcvtq_s32_f32` for a four-lane register (`vcvts_u32_f32` and `vcvtq_u32_f32`
for unsigned results), including the existing padded two-
and three-lane shapes. The wrapper adds no comparisons, branches or memory
operations. Floating-point status effects belong to the executed instruction;
constant evaluation computes the same integer result without accessing status.
The separate `convert<std::int32_t>` operation retains its finite,
representable-input precondition.

Binary32 vector comparisons use `FCMEQ`, `FCMGT` and `FCMGE` directly, including
under `-frounding-math`. Binary16 comparisons use the corresponding eight-lane
instructions. `!=` complements equality; `<` and `<=` reverse the operands of
`>` and `>=`. NaNs compare unordered, signed zeros compare equal, and the active
FPCR denormal controls apply. Equality raises invalid for signaling NaNs;
ordered inequalities also raise invalid for quiet NaNs. Existing sticky FPSR
flags are retained, including when the comparison result is discarded. There
are no status-register reads, writes or CPU fences around the comparison.
Constant evaluation computes the mask without accessing machine status.
Two- and three-lane binary32 masks retain their zero-padding normalization.

On little-endian AArch64 a full register comparison adds no instructions beyond
the comparison itself (`!=` also inverts its result). Big-endian Clang 23 needs
six additional register-permutation instructions around these floating-point
assembly wrappers compared with ACLE. That is an exception to zero overhead;
the wrapper does not add loads, stores or scalar comparisons. The endian result
is checked by cross-compilation, not execution on a big-endian machine.

For multiply-high, compute the doubled full product and take its high half;
the rounded form adds half a unit before truncating. The minimum-times-minimum
case saturates to the maximum signed lane. Narrowing consumes 128 logical bits
and saturates each source lane before changing width. The high forms explicitly
preserve their low argument.

```cpp
#include <cstdint>
import native.arm.neon;
constexpr auto requirement = native::feature_closure(native::arm_feature::neon);
using samples = native::simd<std::int16_t, 8, requirement>;

__attribute__((target("neon")))
samples saturated_sum(samples a, samples b) {
  return native::sqadd(a, b);
}
```

Saturating runtime instructions can set sticky `FPSR.QC`, even when their result
is discarded. They preserve existing QC and do not save, clear or restore FPSR.
Exact volatile instructions retain these effects without memory clobbers or
per-operation status-register traffic. The wrappers make no `pure` or `const`
attribute promise. Constant evaluation computes lane values only and does not
observe or modify machine status. Feature-absent constant-only overloads exist
when the underlying storage shape is available; runtime operands are rejected.

The 64-bit two-word shapes retain the library's existing padded storage. Bridges
select only their logical low half and initialize padding without adding
instructions to the paired codegen leaves. One-lane 64-bit forms use the scalar
D-register encodings. Raw NEON types stay in implementation helpers.

The [Arm ACLE intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html)
lists the corresponding instruction forms. `tests/arm_neon` checks 122 concrete
operation/shape forms with constant assertions, an independent wide-integer
reference, randomized native execution, FPSR.QC tests and identical raw/public
assembly streams. It also checks feature, caller-target, shape and signedness
boundaries, preserved architecture tags, and public module imports. The same
fixture is an installed-package consumer through `find_package(native)`.

Big-endian compiler validation compares all 122 instruction shapes and their
storage bridges with Clang's ACLE lowering. It symbolically checks register-bit
permutations and the sticky QC contributions, including low/high narrowing and
padded two-word storage. The 128-bit inline-assembly boundary reverses the complete
register byte order; 64-bit operands already have the ACLE representation.
This is cross-compiled layout evidence, not execution on a big-endian host.

On big-endian AArch64, two-lane 64-bit `sqadd`, `uqadd`, `sqsub`, `uqsub`,
and all eight variable-shift operations are available with a documented exception
to the zero-overhead guarantee. LLVM 23 emits five extra register-permutation
instructions for each of these twelve forms compared with the equivalent ACLE
leaf that retains its QC effect. These are register operations, with no scalar
fallback or extra memory accesses. Little-endian targets do not incur this cost.

The big-endian compiler check validates all 122 shapes and records the extra
instruction counts, rejecting additional overhead beyond these known permutations.
It also verifies the resulting register bits and sticky QC effects against ACLE;
this remains compiler evidence rather than big-endian hardware execution.
Public shape checks cover constant and runtime participation of the twelve forms,
other vector widths and scalar forms.
