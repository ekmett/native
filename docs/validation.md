# Validation

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
