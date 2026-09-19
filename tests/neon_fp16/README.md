# Native NEON FP16 profile

The AArch64 hub exposes the `neon_fp16` API through `import simd;` at the
configured project minimum. Native operations carry Clang's `fullfp16`
requirement. Use the [source target helper](../../docs/omnibus.md) for variants
in one translation unit, or compile a separate kernel with
`simd_target_profile(kernel NEON_FP16)` as this fixture does. Common modules
keep one provider. Importing the hub does not admit optional instructions.

`vec<fp16,8,neon_fp16>` provides exact representation storage, addition,
subtraction, multiplication, division, `sqrt(x)` (found by ADL), unary negation, comparisons, selection, and
`fma(a,b,c)`. FMA computes a*b+c with one half rounding. Arithmetic follows the
calling thread's FPCR, including rounding and FZ16 (half input/output subnormal
flushing), and may update FPSR. It never changes FPCR. NaN payload/sign and
exception behavior follow the native instructions; portable payload propagation
is not promised. Existing scalar `fp16` conversions retain their independent
round-to-nearest, ties-to-even, gradual-underflow contract. Storage and selection
preserve all representations, including signaling NaNs, without arithmetic.
Comparison masks have eight full zero/all-one 16-bit lanes. Only this half shape
is implemented; native numeric conversions remain future work. Native ARM BF16 storage/dot
products have a separate [NEON_BF16 profile](../neon_bf16/README.md).

Before entering a translation unit compiled for NEON_FP16, a minimum-profile
caller imports `simd.cpu.arm`, calls `observe_arm_capabilities()`, and requires
`classify_isa(cpu, neon_fp16).admitted()`. The classifier
requires baseline FP/Advanced SIMD and scalar/vector half arithmetic. Any stronger
application-configured minimum remains the application's startup requirement.

- Linux observes FP, ASIMD, FPHP and ASIMDHP through `getauxval(AT_HWCAP)`.
- macOS observes `hw.optional.neon` and `hw.optional.arm.FEAT_FP16`. Only an
  unavailable modern key permits `hw.optional.neon_fp16` fallback; false or failed
  observations reject.
- Windows observes ARM v8, NEON and processor feature 67 (FEAT_FP16). Older HALs
  that cannot report feature 67 reject, regardless of possible hardware support.

The fixture runs synthetic admission checks, exhaustive 65,536-encoding storage
roundtrips, protected-page tails (including null zero-length operations), exact
half arithmetic against a generated rational/integer oracle in 32 FPCR states, full-mask
comparisons/selections, native code-generation checks and old/new type isolation.
The driver is compiled at the package minimum and returns skip 77 before native
entry on an unsupported CPU/OS. Runtime tests save and restore FPCR/FPSR and clear
AH/AHP/FIZ while qualifying standard half arithmetic. FZ16 controls half subnormals;
the independent FZ setting is also varied and does not alter these half operations.
NaNs compare by class with
quieting required; DN additionally requires the default half NaN encoding.

Regenerate the committed deterministic corpus with
`python3 -B tests/neon_fp16/generate_reference.py --output /tmp/neon-reference`.
Compare both generated headers and the manifest against this directory. Division
uses exact rational quotients; square root uses integer roots and midpoint-square
comparisons. Every encoding is exercised for square root (2,097,152 outputs),
in addition to 442,368 mixed arithmetic outputs from 2,304 cases. Exact status
witnesses cover division by zero, invalid division/negative square root, exact
square root and inexact division/square root. The generator also
runs exhaustive exact format and midpoint checks. It does not use host floating
point to determine arithmetic results.

After installation, physically move the install prefix and configure this
directory as a standalone project with `simd_DIR` pointing into the moved package.
The consumer compiles hub imports with and without a PCH, runs the same admitted
native tests, and verifies one BMI for the hub and each common module.
`tests/omnibus` also exercises an admitted ARM kernel; `tests/half_storage`
checks the scalar storage API at the project minimum.

Primary references: [Arm ACLE](https://arm-software.github.io/acle/main/acle.html),
[Advanced SIMD intrinsics](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html),
[Linux AArch64 HWCAP](https://www.kernel.org/doc/html/latest/arch/arm64/elf_hwcaps.html),
[Windows processor features](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent),
and [Apple XNU capability sysctls](https://github.com/apple-oss-distributions/xnu/blob/main/bsd/kern/kern_mib.c).

Local qualification on Apple M1/macOS 14.3 used Clang 24 development revision
`f471750e042c`, CMake 4.4.3 and Ninja 1.12.1: native storage/arithmetic/masks,
ASan, the explicit FP16-disabled minimum, relocated consumers, PCH and shared-BMI
checks passed with actual execution. This supplements hosted LLVM 23 qualification.
Windows LLVM 23.1.1 clang-cl also compiled the ARM64 profile, admission, kernels,
PCH and test driver at the ARMv8-A minimum and passed code-generation and
effective-minimum checks with `SIMD_MINIMAL_HAS_NEON_FP16=0`; that local x64 host
did not execute ARM64 code. Its default x86 suite passed 49 tests.
The optional slice leaves issue 15 open for the remaining native-half backends
and operations.
