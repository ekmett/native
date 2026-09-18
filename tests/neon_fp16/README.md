# Native NEON FP16 profile

Configure `SIMD_PROFILES=NEON;NEON_FP16` on AArch64 to opt in. Defaults remain
`NEON`. `simd::neon_fp16` is a separate archive and `simd.neon_fp16` is a separate
module. `simd_target_profile(kernel NEON_FP16)` privately adds Clang's `fullfp16`
feature without resetting the configured CPU or minimum architecture. Common
modules, including `simd.numerics` and `simd.arm`, retain one provider at the
configured minimum. Linking archives does not admit an optional instruction set.
The minimum feature probe compiles real FP16 intrinsics: some AArch64 compilers
retain FP16 macros after an explicit `-fullfp16`. A regression compiles the same
probe using the baseline dispatcher's exact effective options. The focused ARM
CI build selects an ARMv8-A minimum without FP16 and enables it for native
kernels and their PCH, while ordinary default builds keep their existing minimum.
LLVM 23 accepts these stronger imports, but rejects importing a common BMI built
with an explicit `-fullfp16` feature disable into a `+fullfp16` translation unit.
Use `-march=armv8-a` (clang-cl: `/clang:-march=armv8-a`) for this capability-absent
minimum on LLVM 23; BMI validation remains enabled. The explicit negative feature
case additionally passed on the development compiler recorded below.

Late options automatically cover literal C++ source paths and the qualified
Ninja C++ PCH sources. A path computed by a source-list generator expression
needs explicit registration with `simd_context_source_profile(target
"/absolute/path/kernel.cc")` after selecting the profile; otherwise conflicting
minimum feature options can still override that source's profile. Registration
uses the compiling target's options, so one file can serve different profiles.

`vec<fp16,8,neon_fp16>` provides exact representation storage, addition,
subtraction, multiplication, unary negation, comparisons, selection, and
`fma(a,b,c)`. FMA computes a*b+c with one half rounding. Arithmetic follows the
calling thread's FPCR, including rounding and FZ16 (half input/output subnormal
flushing), and may update FPSR. It never changes FPCR. NaN payload/sign and
exception behavior follow the native instructions; portable payload propagation
is not promised. Existing scalar `fp16` conversions retain their independent
round-to-nearest, ties-to-even, gradual-underflow contract. Storage and selection
preserve all representations, including signaling NaNs, without arithmetic.
Comparison masks have eight full zero/all-one 16-bit lanes. Only this half shape
is implemented; division, square root, x86 FP16 and ARM BF16 remain future work.

Before entering a translation unit compiled for NEON_FP16, a minimum-profile
caller imports `simd.arm`, calls `observe_arm_capabilities()`, and requires
`classify_arm_profile(cpu, arm_profile::neon_fp16).admitted()`. The classifier
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
half arithmetic against a generated rational oracle in 16 FPCR states, full-mask
comparisons/selections, native code-generation checks and old/new type isolation.
The driver is compiled at the package minimum and returns skip 77 before native
entry on an unsupported CPU/OS. Runtime tests save and restore FPCR/FPSR and clear
AH/AHP while qualifying standard half arithmetic. NaNs compare by class with
quieting required; DN additionally requires the default half NaN encoding.

Regenerate the committed deterministic corpus with
`python3 -B tests/neon_fp16/generate_reference.py --output /tmp/neon-reference`.
Compare the generated header/manifest against this directory. The generator also
runs exhaustive exact format and midpoint checks. It does not use host floating
point to determine arithmetic results.

After installation, physically move the install prefix and configure this
directory as a standalone project with `simd_DIR` pointing into the moved package.
The consumer compiles both granular and omnibus imports (the latter with a PCH),
runs the same admitted native tests, and verifies one BMI provider per common
module. The existing `tests/omnibus` and `tests/half_storage` fixtures also select
and admit the strongest enabled ARM profile.

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
