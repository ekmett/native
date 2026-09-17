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

## NEON utilities and rounding â€” source `0c4c0ae`

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
