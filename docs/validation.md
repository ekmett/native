# Validation

The tests cover value semantics, memory boundaries, module identity and installed
package consumption. Counts below belong to the configurations stated beside
them; sanitizer, assembly and downstream results retain their separate scope.

The short-vector and named-swizzle checkpoint was exercised on Windows x86-64
and an Apple M3 with upstream Clang 23.1.1, CMake 4.4.3 and Ninja. These are CPU
and compiler checks; they do not establish GPU behavior or throughput.

| Configuration | Result |
| --- | --- |
| Windows, AVX2 fixtures, both x86 module producers, PCH and ThinLTO | 28 tests passed with exceptions off; 28 with exceptions on |
| Windows, relocated installed module package | 1 test passed for each exception configuration |
| Windows, AVX-512 ordinary swizzles | Header and module consumers passed |
| Windows, AVX2 and AVX-512 swizzles with AddressSanitizer | All four consumers passed |
| M3, NEON, PCH and ThinLTO | 23 tests passed |
| M3, relocated installed module package | 1 test passed |
| M3, ordinary and AddressSanitizer memory fixtures | All four tests passed |
| M3, AddressSanitizer named swizzles | Header and module consumers passed |

The full Windows and M3 suites include the short-vector bitwise flush bridge and
Apple SDK header coexistence tests. Source hashes were captured before and after
each run. The separately retained focused x86 sanitizer and assembly checks used
the same swizzle implementation; later changes added tests and the bit bridge.

## What the swizzle tests establish

`tests/core_regression/swizzle.cc` is compiled through both headers and modules.
It covers logical two- and three-lane float, signed/unsigned 32-bit integer and
mask vectors. It checks constexpr construction, unchanged physical layout,
owning reads, overlapping writes, repeated-index read-only access, const/rvalue
restrictions and exact bit transport. Partial and full loads/stores run against
guarded page boundaries, including zero-length null access.

Arithmetic is compared against the active lanes of physical four-lane vectors.
`short_flush.cc` checks two- and three-lane register and array flushing in both
CPU denormal modes. Signed subnormals become signed zero; every other supplied
word, including NaN payloads, passes through unchanged. The operation leaves the
floating-point environment untouched.

Inactive division lanes do not introduce floating-point exceptions. Unsupported
mixed shapes and architectures are rejected at compile time.

`swizzle_codegen.cc` contains six externally visible probes. In optimized strict
floating-point assembly, all six have no calls or stack references on AVX2,
AVX-512 and NEON. Three-lane x86 transfers use masked loads/stores; NEON transfers
use eight plus four bytes. Reversal and overlapping reversal compile identically.
This inspection says nothing about unmeasured custom-element swizzle overhead.

## Numerical stability and downstream use

The pre-change and post-change Windows raw header/module captures are identical:
425,984 bytes, SHA-256
`b6d62dcc229f4ed98945a1e916c79fa588a0266c1b4b91db668e3812549cc609`.
The 3,072-byte architecture capture agrees between Windows, M3 and relocated
consumers, SHA-256
`92e940676561d462135e70a6acd50eaba0b879b5663fe7dfc5b8dc11878181a0`.
These banks cover their recorded inputs, not every possible floating-point value.

The independent FTZ library exercised the custom-element extension through its
installed package: 24 Windows tests passed with exceptions disabled and enabled;
15 M3 tests passed for each compatibility-alias policy. Its original
2,208-word numerical capture remained identical across both hosts. Additional
static-library consumers passed with PCH and ThinLTO for AVX2, AVX-512 and NEON.
This validates actual downstream module linkage rather than only imports inside
the producer project.

Run the maintained suite with the build recipe in [building](../doc/building.md).
Use `SIMD_TEST_ISA` to select a native test profile and run AVX-512 tests only on
an admitted CPU. Enable `SIMD_ENABLE_ASAN` for sanitizer builds; use a separate
build directory with IPO disabled when inspecting sanitizer behavior. The
bounded M3 runner is `tests/neon/run.py`; the caller supplies any host resource
gate and toolchain paths.

## Binary wide math

Generic `wide` forwards `atan2(y,x)` to an element library's array overload
when available, otherwise to its element operation. Empty packs, ADL batch
selection and exception specifications are part of the maintained tests.
The Windows suite passes 29 tests with exceptions enabled plus the relocated
consumer; M3 passes 24 tests plus the relocated consumer. The raw numerical
captures above are unchanged. These checks used the same Clang 23.1.1 and
CMake 4.4.3 toolchains as the original checkpoint.

The downstream FTZ library additionally passed native log/log1p, tanh and atan2
and separate sin/cos through arrays and wide packs. Its four common-width
output packets contain 2,688,588 words and are byte-identical on AVX2, AVX-512
and M3 NEON. This is evidence for those recorded graphs and inputs, not a
universal floating-point or arbitrary libm guarantee.

## Value utilities, directed rounding and combined packages

The recorded Windows value-utility and rounding configuration passes 35 core
tests and one relocated installed consumer with exceptions enabled, PCH and ThinLTO. Both x86 module
producers are built. Separate installed consumers also exercise AVX2 and
AVX-512 against matching SIMD and FTZ packages.

Generic `wide` classification (`isfinite`, `isinf`, `isnan`, `signbit`) preserves
the element operation's actual bool or mask result type. Homogeneous `copysign`
preserves value types. Empty packs, throwing ADL operations and result
construction are covered by the mapper's exception tests. Raw `floor`, `ceil`
and `trunc` cover scalar, short and full vectors, arrays and wide values; an
integer-word oracle checks signed zeros, infinities and integer boundaries
under all four standard rounding modes. These operations select their own
rounding direction. Ordinary x86 native-leaf assembly uses the fixed rounding
instructions without function calls.

The combined installed FTZ consumers pass four value-utility tests, two rounding
tests and four policy-boundary tests, plus one third-static-library test per
ISA. The latter consumers also use PCH and ThinLTO. These focused checks extend
the earlier numerical checkpoint; they do not replace its packet results.
That combined-package record covers Windows. A separate M3 installed FTZ run
passes thirteen focused utility, rounding and policy tests plus one transitive
consumer, using matching dependencies with exceptions enabled, PCH and ThinLTO.
It extends the downstream utility coverage; the earlier large math packets
retain their own qualification scope.

## NEON utilities and rounding — source `0c4c0ae`

On Apple M3, source `0c4c0ae769cb45978c758a4a7eeb3d02fc06e54c` passes
30 core tests and one relocated installed-package consumer with exceptions
enabled, PCH and ThinLTO, using Clang 23.1.1 and CMake 4.4.3. Source hashes match
before and after the run. The architecture capture remains
`92e940676561d462135e70a6acd50eaba0b879b5663fe7dfc5b8dc11878181a0`
in both producer and relocated consumer.

The scalar and NEON directed-rounding fixtures perform 4,312,608 checks across
header and module consumers under all four standard rounding modes. The suite
also includes the generic wide value utilities. This run is ordinary CPU
execution; the earlier sanitizer and assembly results keep their own scope.
No GPU execution or throughput measurement is implied.

The `0c4c0ae` run uses granular imports. The later omnibus has its own
installed-consumer qualification below.

## NEON omnibus — source `53d9a44`

On the same Apple M3 and toolchain, source
`53d9a44910b7f3e504e72e94ed7d01fe655764ad` passes 30 core tests,
one granular relocated-package test and three omnibus consumer tests. The
consumer tests cover archive-only linkage, baseline granular imports and
`import simd;` with NEON vectors. No tests were skipped.

Exceptions and PCH are enabled. Native kernels use ThinLTO; baseline executables
keep IPO disabled. All 215 source files and both tracked symlinks are unchanged.
The granular architecture capture has the same SHA-256 recorded above, and the
compiler logs contain no warnings or errors.

This qualifies the configured NEON-only omnibus on ARM. It adds no GPU,
sanitizer, throughput or mixed-architecture omnibus claim. The retained receipt
archive has SHA-256
`c7b4b20aaacc597fc86f166af41a09c275331963c5b0f1a3941706cde653448d`.

## Linux x86-64 packages

The Linux check uses Ubuntu 22.04, glibc 2.35 and an Intel Core i9-12900K with
LLVM 23.1.1, its bundled libc++ 23, CMake 4.4.3 and Ninja 1.12.1. Exceptions,
producer PCH and ThinLTO are enabled. Both AVX2 and AVX-512 module providers
compile; this CPU admits AVX2 only.

| Source and configuration | Result |
| --- | --- |
| `36db84d`, root suite, AVX2 runtime fixtures and both x86 providers | 34 tests passed; the separate AVX-512 execution test was excluded |
| Exact `4255f00`, relocated combined-profile package | Two archive-only/baseline consumer tests passed; both native omnibus kernels compiled without execution |
| Exact `4255f00`, relocated AVX2-only package | All three omnibus consumer tests passed, including native AVX2 execution |
| `36db84d` mixed-profile installed consumer | Compiled; the AVX-512-required execution test returned the expected skip status 77 |
| FTZ `3e2da97`, using the exact combined SIMD package | 23 AVX2 host tests and one relocated third-library consumer passed |

The root-suite correction changes tests only. The original in-tree
static-string fixture selected an AVX-512-flavored common BMI for its baseline
translation units. It now takes module metadata from `simd::common` and links
the archive file through an explicit build dependency. Public package metadata
and arithmetic sources are unchanged. The portable mixed-profile dispatcher
also passes its focused clang-cl Windows check against the existing installed
package; the CPU and OS admission conditions are unchanged.

All three relocated prefixes retain identical installed-file hashes. Their old
locations are absent, and the installed consumers use no production include or
module source from the source checkout. Source hashes match after execution.
The compatibility alias in this FTZ build selects manual policy; its dual-policy
tests also execute `m32` under gradual/flush controls and admitted `h32` under
flush controls. These are CPU/module checks, without Linux AVX-512 execution,
GPU execution, sanitizer or performance claims.

## Compiler cache

Source `b6da508` plus the sccache workflow change was checked on macOS 15.5
ARM64 with Clang 23.1.1, CMake 4.4.3, Ninja 1.12.1 and sccache 0.16.0.
The Release producer enabled NEON, tests, PCH and IPO, with exceptions disabled
and `CMAKE_CXX_COMPILER_LAUNCHER=sccache`. A separate local disk cache and server
were used; this was not a GitHub Actions cache-service test.

| Build | Launcher requests | Cache hits | Cache misses | Non-cacheable calls | CTest |
| --- | --- | --- | --- | --- | --- |
| Empty cache | 48 | 0 | 11 | 37 | 36/36 passed |
| Clean rebuild, retained cache | 48 | 11 | 0 | 37 | 36/36 passed |

The eleven cacheable requests comprised ten C++ compilations and one Clang PCH
creation. All 37 bypasses reported `@`: the generated CMake module-map response
files contain quoted paths, which the pinned sccache parser does not expand.
Cache statistics exclude dependency scanning, linking and CMake-synthesized BMI
commands that do not use the launcher. Both passes reported zero cache errors
and zero compilation failures. The warm pass used the same source/build paths
and a Ninja clean before rebuilding; it was not a no-op incremental build.

Installation succeeded. The installed omnibus consumer, configured without a
compiler launcher and with sccache absent from `PATH`, passed all three tests;
installed CMake metadata contains no sccache dependency. PCH, ThinLTO, module
sources and library code were unchanged.

The workflow's YAML and Bash scripts were checked locally. Upstream release
assets were verified to exist for Linux x86-64/ARM64, Windows x64/ARM64 and macOS
ARM64. Native Windows/Linux execution and reuse between hosted workflow runs
remain unverified by this check; each CI lane retains its own cache statistics.
This initial check did not measure wall time or cache every module command;
the follow-up below measures conservative module-map expansion.

### Conservative module-map expansion

The follow-up launcher was compared against plain sccache with the same source,
macOS ARM64 toolchain, NEON configuration, PCH, IPO and two compiler jobs. Each
variant used its own fresh build tree, local cache and server; the warm pass
cleaned its outputs and reset statistics while retaining that variant's cache.

| Launcher | Build | Hits / requests | Misses | Bypasses | Build wall time |
| --- | --- | --- | --- | --- | --- |
| Plain sccache | Cold | 0 / 48 | 11 | 37 | 14.03 s |
| Plain sccache | Clean warm | 11 / 48 | 0 | 37 | 10.88 s |
| Module-map expansion | Cold | 0 / 48 | 48 | 0 | 15.63 s |
| Module-map expansion | Clean warm | 48 / 48 | 0 | 0 | 4.98 s |

All four builds passed 36/36 CTests. Expanded maps enabled caching for all eight
module producer commands and all 39 ordinary C++ commands, alongside the one
PCH request. Both cold and warm passes reported zero cache errors. These
statistics still exclude CMake-generated BMI commands without a launcher,
dependency scanning and linking. Times measure only `cmake --build`, excluding
configuration, cleaning and CTest. They are single local observations, not a
repeated benchmark or a claim about hosted CI performance. The cold normalized
build took longer than the plain cold build. Warm build time fell by 54.2%
(10.88 s to 4.98 s), while cold time rose by 11.4% (14.03 s to 15.63 s).

Seven focused launcher test methods cover accepted generated maps, preservation
of argv and response-file contents, unknown/ambiguous syntax, all whitespace
classes, quotes/escapes, nested response files, missing/non-ASCII files, size
limits, Windows/clang-cl bypass, `E2BIG` fallback and propagation of compiler
output and exit status. All 53 module maps from the earlier build also produced
the same Clang 23 `-###` invocation with original response files and expanded
arguments.

An independent two-file Clang module fixture checked cache invalidation. The
cold build missed twice and its unchanged warm rebuild hit twice. Changing a
header used by the module and then changing its exported constant each forced
both producer and unchanged importer to miss, and the executable observed the
new values (1 to 2 to 3). A final unchanged rebuild hit twice and retained 3.
Timestamp-only header/module changes also allowed cached module reuse with
freshly compiled importers. All seven stages passed with zero cache errors and
unchanged module-map SHA-256 hashes. This tests local cache correctness for
those changes; hosted cache-service reuse and native Linux/Windows execution
retain the limitations above.

### Integration with current main

After integrating upstream `7b44537` (including source-layout changes and the
new scalar `scalef` regression), commit `b366eea` repeated the expanded-map check
with the same toolchain and two compiler jobs. A fresh cache produced 49 misses
in 15.80 s; a clean warm rebuild produced 49 hits in 5.12 s, with no bypasses or
cache errors. Both builds passed all 37 CTests. This is another single local
cold/warm pair, not a new comparison against plain sccache.

The current package was installed, moved to a path containing a space, and
consumed without a compiler launcher or sccache on `PATH`. Its public-header
boundary test and all three omnibus consumer tests passed. Workflow validation
retained all five platforms and both exception settings (ten configurations).
