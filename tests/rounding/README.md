# Directed rounding

`simd::floor`, `simd::ceil` and `simd::trunc` retain the raw float vector type.
Their array overloads accept only raw float vectors. The generic `wide` lift
uses the element operation through ADL and retains its type, construction and
exception behavior. Empty packs do not evaluate an element operation.

The ISA profiles use fixed-direction rounding instructions. Two- and three-lane
vectors operate on their four-lane storage; padding remains positive zero. The
baseline scalar profile uses the standard floating-point function. Direction
never depends on the ambient rounding mode. Raw denormal input handling follows
the active DAZ/FZ environment, as with other raw arithmetic. The numerical test
uses gradual input handling under all four rounding modes, compares every
non-NaN word exactly (including signed zero), and accepts any NaN result for a
NaN input. Exception flags and NaN payload selection are not a new contract.

The integer-only oracle clears fractional significand bits and applies the
required directed integer increment. Its bank includes subnormals, signed zero,
every integral transition exponent, the 2^23 boundary, infinities, signaling and
quiet NaNs, and deterministic random words. Tests exercise every supported width,
empty/single/multiple arrays, and scalar/native-vector wide packs. Header and
installed-module consumers use the same bank. `adl.cc` also checks custom result
types, discarded-result side effects and exception cleanup with exceptions on.

A focused installed check (Clang 23 and CMake 4.4) is:

```sh
cmake -S tests/rounding -B build/rounding -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -Dsimd_DIR=/installed/lib/cmake/simd -DSIMD_ROUNDING_PROFILES="AVX2;AVX512"
cmake --build build/rounding --parallel 2
ctest --test-dir build/rounding --output-on-failure
```

Select `NEON` on ARM64. Native test executables require their selected ISA;
CPU/OS admission is the runner's responsibility. The regular top-level test
build also registers these checks. For the focused exception test, configure
`tests/wide_module` with `SIMD_WIDE_EXCEPTIONS=ON` and run `wide_rounding`.

On Windows, `check_codegen.py --objdump <llvm-objdump> --build build/rounding
--output build/rounding/codegen` inspects the ordinary (non-LTO) consumer objects.
It checks the 54 AVX2/AVX512 header/import leaves for one fixed-direction rounding
instruction and no calls or branches. This is an instruction check, not a timing
claim or ARM execution evidence.
