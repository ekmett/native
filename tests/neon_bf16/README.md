# Native NEON BF16 profile

Configure `SIMD_PROFILES=NEON;NEON_BF16` on AArch64 to opt in. Defaults stay
`NEON`. `simd::neon_bf16` is a separate archive and `simd.neon_bf16` a separate
module. `simd_target_profile(kernel NEON_BF16)` adds only Clang's `bf16` target
feature, retaining the configured CPU and minimum. Common modules, including
`simd.numerics` and `simd.arm`, keep a single provider at the configured minimum.
Single-module providers do not use PCH. Consumer-owned PCH remains supported.
Use `simd_target_omnibus(target)` for `import simd;` consumers; it applies the
configured feature union, including both FP16 and BF16 when present.

`vec<bf16,8,neon_bf16>` stores one native 128-bit register. Construction,
load/store, native and unsigned bit bridges, and partial loads/stores preserve
all 65,536 representations including signaling NaNs and subnormals. Partial
operations touch exactly the requested prefix (0 through 8); zero-length calls
accept null pointers. The mask alias is eight full 16-bit lanes. There is no
BF16 elementwise arithmetic, comparison, conversion, division or square root.
`dot2(a,b,c)` returns `vec<float,4,neon_bf16>`: destination lane i combines source
lanes 2*i and 2*i+1, then accumulates c[i]. Native bridges do not convert numbers.

## Native arithmetic contract

This operation emits `BFDOT Vd.4S,Vn.8H,Vm.8H`; it does not emulate an FP32 loop.
It follows the executing CPU and caller's FPCR. It is not a portable result
contract across x86/Arm, FPCR modes, or the downstream reproducible FTZ library.
Scalar `bf16` conversion and rounding policies are unchanged.

- With FEAT_EBF16 absent or FPCR.EBF=0, each product, the pair sum, and accumulation
  round separately to odd. Inexact finite rounding forces the low result bit to
  one; overflow produces signed infinity. Input and output subnormals flush to
  zero irrespective of FZ/FIZ. RMode is ignored. AH only selects default NaN sign.
- With FEAT_EBF16 and FPCR.EBF=1, the exact sum of the two products rounds once to
  FP32, followed by a separately rounded FP32 addition to the accumulator.
  RMode selects rounding and FZ controls flushing. With FEAT_AFP, FIZ independently
  flushes input subnormals; AH=1 makes FZ affect outputs only and detects output
  underflow after rounding. AH=0 retains standard input/output FZ behavior.
- Both modes generate default NaNs regardless of DN; with AH=1 the default NaN
  sign is negative. Floating-point exception enables are ignored, FPSR cumulative
  flags remain unchanged, and the API never writes FPCR.

These are instruction semantics, not a new reproducible h16/m16 contract. In
particular the Arm baseline pair association and rounding differ from x86
VDPBF16PS. Applications requiring a fixed environment must establish that FPCR
before the call and restore their own environment when appropriate.

## Admission and validation

A baseline caller imports `simd.arm`, observes capabilities, and requires
`classify_arm_profile(cpu, arm_profile::neon_bf16).admitted()` before optional
entry. NEON_BF16 requires baseline FP/Advanced SIMD and an observed BF16 feature;
it does not require FP16 or EBF16. Unknown/failed observations reject. The public `arm_capabilities` and
`arm_admission` records grow in this checkpoint: rebuild producers and consumers
together rather than mixing old archives/BMIs with new declarations. EBF16 is
reported separately for applications choosing enhanced arithmetic. Any stronger
configured minimum remains the application's startup requirement.

- Linux uses `getauxval(AT_HWCAP)` for baseline and `AT_HWCAP2` for BF16/EBF16.
  SDK headers lacking a capability bit leave that feature unobserved.
- macOS uses `hw.optional.neon`, `hw.optional.arm.FEAT_BF16`, and
  `hw.optional.arm.FEAT_EBF16`. There is no CPU-model guess or instruction probe.
- Windows uses ARM v8/NEON and documented processor-feature selectors 68/69.
  Older HALs return false for unsupported selectors and cannot admit BF16.

Regenerate the deterministic bank with `python3 -B generate_reference.py --output-dir /tmp/bfdot-reference`.
It contains 2,048 exact-rational cases with baseline and eight EBF16 results per
case and performs 326,497 format/midpoint/association selfchecks without host
floating-point arithmetic. AH=FIZ=0 are the oracle domain; targeted controls
cover AH/FIZ separately.

The focused fixture tests synthetic admission, all storage encodings, protected
page tails, reference dot-product cases, FPCR preservation and FPSR nonmutation,
AH/FIZ controls, type isolation and native instruction codegen. The runtime driver
is compiled at the package minimum and returns 77 before native entry if BF16 is
not advertised. EBF=1 is exercised only when EBF16 is observed; AH/FIZ nonzero
states are exercised only when those controls read back as writable.

Install, physically relocate the prefix, then configure this directory with
`simd_DIR` pointing into the moved package. It builds granular and omnibus
consumers, consumer PCH, and verifies one provider per common BMI. Native
execution, compile-only checks, and software-oracle checks are distinct evidence.
M1 does not implement BF16; compiling there and seeing skip 77 is not native BF16
qualification. Hosted ARM runs may qualify it when OS admission succeeds.

Primary sources: [Arm BFDOT instruction reference](https://documentation-service.arm.com/static/67e40f3398aa3c3b6eea6a85),
[Arm SME supplement B3.1.2 and E2.2](https://documentation-service.arm.com/static/62015c6c965f7d118e3f5f4c),
[ACLE intrinsics](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html),
[Linux HWCAP](https://docs.kernel.org/arch/arm64/elf_hwcaps.html),
and [Windows feature selectors](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent).
