# Optional transcendental throughput fixture

`atan2_bench.cc` compares the public packed-division `math::atan2` kernel with a
private copy of the normalized reciprocal graph: three Newton refinements,
integer exponent reconstruction, and the same reduced polynomial and quadrants.
It is a standalone diagnostic, outside default CTest and performance gating.
The comparison coefficients derive from SLEEF 3.9.0 `atan2kf` through FTZ; the
source retains the full Boost 1.0 copyright and license notice.

## Build and run

Run these commands from the repository root with a C++26 compiler supporting
structured-binding packs. The retained results use Clang 23.1.1. This fixture
uses headers directly and needs no library build or module cache.

Apple ARM64, four binary32 lanes per NEON register:

```sh
native_cxx=clang++
native_sdk="$(xcrun --sdk macosx --show-sdk-path)"
mkdir -p build-transcendentals
"$native_cxx" -std=c++26 -fms-extensions -O3 \
  -nostdinc++ -isystem "$native_sdk/usr/include/c++/v1" -isysroot "$native_sdk" \
  -Isrc tests/transcendentals/atan2_bench.cc \
  -o build-transcendentals/atan2-bench
./build-transcendentals/atan2-bench
```

Select the intended compiler in `native_cxx`; an older system Apple Clang may
not implement the required C++26 features. The retained ARM run used SDK 15.5.

Linux x86-64 with AVX2 and FMA, eight binary32 lanes per register:

```sh
native_cxx=clang++
mkdir -p build-transcendentals
"$native_cxx" -std=c++26 -fms-extensions -O3 -mavx2 -mfma -ffp-contract=off \
  -Isrc tests/transcendentals/atan2_bench.cc \
  -o build-transcendentals/atan2-bench
./build-transcendentals/atan2-bench
```

The recorded x86 run was pinned to P-core 6 of an Intel Core i9-12900K with
`taskset -c 6`; choose an appropriate core on another machine. Run on an idle host,
avoid concurrent builds, and retain all emitted JSON rows. The source does not
change floating-point controls. It requires a Clang/GCC-style compiler because
the measurement loop uses an inline-assembly memory barrier.

For assembly inspection, replace the final executable output with
`-S -o build-transcendentals/atan2-bench.s` in the same compile command. The
`atan2_direct_N` and `atan2_newton_N` symbols expose complete kernels for
`N = 1, 2, 3, 4, 6, 8`. Dispatch uses direct `if constexpr` branches so the fixture
does not introduce an outlined lambda around a large kernel.

## Recorded throughput

Each sample processes 12,288 normal input pairs for 2,048 passes. Both magnitudes
are in `[0.5,1)`, with independently generated signs; all quadrants and reduced
ratios in approximately `[0.5,1]` are exercised. Arrays occupy about 144 KiB in
total. Each register count has a warmup and nine samples in alternating candidate
order. The table reports median nanoseconds per input pair, including input
loads and output stores; a larger reciprocal/division ratio favors division.

| Registers | ARM division | ARM reciprocal | ARM ratio | AVX2 division | AVX2 reciprocal | AVX2 ratio |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | 0.944485 | 2.021470 | 2.140 | 0.694694 | 1.173407 | 1.689 |
| 2 | 1.053794 | 2.130611 | 2.022 | 0.651853 | 1.063377 | 1.631 |
| 3 | 1.023602 | 1.985661 | 1.940 | 0.633135 | 0.998857 | 1.578 |
| 4 | 0.916415 | 1.759239 | 1.920 | 0.637476 | 0.938590 | 1.472 |
| 6 | 0.948184 | 1.915069 | 2.020 | 0.657142 | 0.950299 | 1.446 |
| 8 | 0.967488 | 1.932050 | 1.997 | 0.636572 | 0.990042 | 1.555 |

Division wins at every sampled register count on both hosts. AVX2 reaches its
lowest division median at three registers, with only small differences through
eight registers. The table does not establish a universal optimal batch size or
an application speedup. ARM scheduling variation was visible, particularly at
two and three registers; for example, its two-register division samples ranged
from 0.911143 to 1.556764 ns/pair. Use the full sample arrays when interpreting
small differences between neighboring register counts.

## Instructions and spills

The selected kernel uses one packed divide, eight polynomial FMAs and two
multiplies per register, plus classification and quadrant reconstruction. The
reciprocal alternative uses fourteen FMAs and three multiplies, plus mantissa
and exponent operations. No libm calls, per-lane extraction or scalar lane loops
appear in the inspected full-register or wide wrappers.

The following are static stack-access instruction counts from Clang 23.1.1
`-O3` wrapper assembly. ARM uses NEON; AVX2 assembly was cross compiled on macOS
with SDK 15.5, independently of the Linux timing run. ARM counts SIMD data loads
and stores through SP, excluding D-register ABI saves/restores. AVX2 counts
stack memory operands, including folded reloads. These are not dynamic hardware
counter measurements and need not match the surrounding benchmark loop exactly.

| Registers | ARM division / reciprocal | AVX2 division / reciprocal |
| ---: | ---: | ---: |
| 1 | 0 / 0 | 0 / 0 |
| 2 | 0 / 0 | 12 / 20 |
| 3 | 0 / 0 | 33 / 39 |
| 4 | 2 / 12 | 57 / 62 |
| 6 | 21 / 28 | 111 / 115 |
| 8 | 65 / 68 | 161 / 167 |

Wide stages retain independent register chains, but register pressure causes
substantial spills in larger packs. The measured AVX2 throughput does not show a
dramatic cliff through eight registers despite those spills; this does not make
the spills free or establish behavior for larger packs or other kernels.

## Numerical limits

Timing inputs intentionally avoid axes, nonfinite values, subnormal operands and
tiny output ratios. This fixture does not validate accuracy, signed special
values or FP-mode behavior. The division implementation treats raw subnormal
inputs as signed zero and permits gradual tiny output according to the caller's
FP mode; the reciprocal comparison flushes tiny reconstructed ratios. Those
contracts differ outside the timing bank.

The separate promoted-atan2 regression exercises signed axes and infinities,
NaNs, empty batches, shape preservation, constexpr evaluation and both supported
FP modes. Its ordinary reference is double libm. A separate 84,330-pair MPFR
256-bit qualification observed maximum 2 ULP for the selected graph on ARM and
baseline Wasm, and for the reciprocal graph on ARM under its tiny-output rule.
That is sampled numerical evidence, not an exhaustive bound, bit-agreement claim,
or part of the throughput measurement.
