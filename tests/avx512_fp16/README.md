# Native AVX-512 half profile

Opt in with `SIMD_PROFILES=AVX2;AVX512;AVX512_FP16`. The separate
`simd::avx512_fp16` archive and `import simd.avx512_fp16;` provide
`vec<fp16,32,avx512_fp16>`: one 512-bit register, unsigned bit bridges, exact
representation loads/stores, bounded partial memory, native add/sub/mul/div/FMA and `sqrt(x)` (found by ADL),
ordered comparisons, bitwise sign negation and representation-preserving select.
Its mask is `predicate<32,avx512_fp16>`. Only the 32-lane half shape is provided;
numeric conversion operations remain absent.

Compile the optional kernel with `simd_target_profile(kernel AVX512_FP16)`.
Keep the pointer/scalar entry and dispatcher in separate targets; admit
`x86_profile::avx512_fp16` using `simd.cpuid` before entering the optional kernel.
Admission requires AVX2/FMA/BMI2 and compiler-implied features, AVX512F/DQ/BW/VL,
CPUID.7.0.EDX[23], OSXSAVE and XCR0 XMM/YMM/opmask/ZMM state. Leaf availability
is checked before interpreting stored feature bits. The application's configured
minimum still applies before any dispatcher executes. This checkpoint appends
`leaf7_edx`/`missing_leaf7_edx` to the public capability/admission records;
rebuild producers and consumers together after updating the package.

`simd_target_omnibus(consumer)` applies the union recorded in the installed
`simd::simd` target's `SIMD_OMNIBUS_PROFILES` property. This matters when both
AVX512_BF16 and AVX512_FP16 are selected: neither extension implies the other.
Admit both profiles before executing an omnibus consumer compiled for both.
Granular consumers retain their individual flags; common modules share one BMI.
Providers compile directly without PCH; the fixture also checks a consumer PCH.

## Arithmetic contract

Half arithmetic rounds directly in binary16 according to MXCSR.RC. FMA computes
the exact product and sum before one final half rounding; it does not widen to
binary32. Half subnormal operands/results use gradual underflow regardless of
DAZ/FTZ. Native exception masks and status behavior apply. Control bits are
preserved. NaNs follow the instruction; portable payload/sign propagation is
not promised. Ordered quiet comparisons return false for NaNs; inequality is
their complement. Selection and sign-bit negation do not perform arithmetic.

These rules follow Intel's [AVX512-FP16 Architecture Specification,
347407-001US](https://cdrdv2-public.intel.com/678970/intel-avx512-fp16.pdf), chapter 2
(feature detection), chapter 4 (denormals and rounding), and the instruction
entries for VADDPH, VSUBPH, VMULPH, VDIVPH, VSQRTPH, VFMADD*PH and VCMPPH.
Existing scalar conversions, default profiles and downstream FTZ arithmetic are
unchanged; this is not an RTZ-policy implementation.

## Focused acceptance

The native fixture checks every 16-bit storage encoding, null zero-length tails
and every tail length 0 through 32 against inaccessible guard pages. The
independent [rational/integer oracle](https://github.com/ekmett/simd/blob/main/tests/neon_fp16/generate_reference.py) supplies 2,304
cases: special values, signed boundary witnesses, stratified finite values, FMA
cancellation and deterministic random representations. This fixture uses only
the oracle's gradual-underflow rows, remapping RNE/RUP/RDN/RTZ to MXCSR's order.
It checks 221,184 mixed arithmetic outputs and every square-root encoding
(1,048,576 outputs) over all 16 RC/DAZ/FTZ states, compares NaNs
by quiet classification, verifies invalid/divide-by-zero/precision status
for the relevant operations, and tests
4,108 selection masks. The existing oracle's exact rational self-check covers
all finite encodings and all adjacent finite midpoint boundaries. Division uses
exact rational quotients; square root uses integer roots and midpoint-square
comparisons, with no host floating-point square root.

`simd.avx512_fp16.admission` runs the baseline synthetic-negative matrix even
without FP16 hardware. `none` enters no optional kernel. `codegen` requires one
native 512-bit half instruction per arithmetic entry and native half comparison
and masked selection, rejecting widening and out-of-line calls. The package
fixture compiles both granular and omnibus imports and checks a single provider
for common and optional module BMIs.

```sh
cmake -S . -B build/fp16 -G Ninja -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DSIMD_BUILD_TESTS=ON \
  '-DSIMD_PROFILES=AVX2;AVX512;AVX512_BF16;AVX512_FP16'
cmake --build build/fp16 --target simd simd_test_avx512_fp16 --parallel 2
ctest --test-dir build/fp16 -R 'simd[.]avx512_fp16[.]' --output-on-failure
cmake --install build/fp16 --prefix 'build/install'
cmake -E rename build/install 'build/relocated package'
cmake -S tests/avx512_fp16 -B build/package-fp16 -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release \
  '-Dsimd_DIR=/absolute/path/build/relocated package/lib/cmake/simd'
cmake --build build/package-fp16 --parallel 2
ctest --test-dir build/package-fp16 --output-on-failure
```

Use `clang-cl` and an initialized MSVC SDK environment on Windows. Unsupported
hardware returns 77 and CTest reports a skip; compilation/codegen success is not
native qualification. Ryzen 7950X3D and Core i9-12900K do not supply this native
execution evidence. Hosted x86 CI attempts the admitted native test and retains
its actual result; runner labels do not promise FP16 support.
