# Validation

The maintained tests check value semantics, instruction behavior, compiler
constraints and installed-package use. Start with the [build guide](../doc/building.md)
and [test guide](https://github.com/ekmett/native/blob/main/tests/README.md). CTest results, generated instruction
sequences and numerical oracles establish different parts of the contract.

## Running the checks

Configure a build for an ISA admitted by the test host, then build and run CTest:

```sh
cmake --build build/core --parallel
ctest --test-dir build/core --output-on-failure
```

`NATIVE_TEST_ISA` selects the primary arithmetic profile. `NATIVE_PROFILES`
selects additional profile fixtures; the module provider exposes the supported
host implementations at the configured project minimum. Run separate builds
for exception settings and sanitizers. `NATIVE_ENABLE_ASAN=ON` instruments host
memory checks; use a separate build directory with IPO disabled for that run.

Routine validation runs locally. For relevant pull-request changes, the
[native CI workflow](https://github.com/ekmett/native/blob/main/.github/workflows/build.yml)
runs Linux ARM64 and Windows ARM64 with exceptions enabled. Both jobs retain
the complete source tests and physically relocated package checks. Changes to
source, tests, CMake configuration, CI tooling or the compiled README example
trigger these jobs; source and test Markdown alone does not. Native CI does not
repeat automatically after a merge.

Use **Run workflow** on **C++26 modules** for the full ten-job qualification:
Linux x86-64 and ARM64, Windows x64 and ARM64, and macOS ARM64, each with
exceptions enabled and disabled. Routine PR checks therefore do not establish
x86, macOS or exception-disabled coverage for that revision; qualify those
locally or request the full workflow when needed.

Superseded PR runs are canceled. A new manual run cancels an older manual run
on the same ref, while manual qualification remains separate from PR checks.
Diagnostic artifacts expire after three days; retain any evidence needed
longer before it expires.

An optional-instruction test must admit the CPU and operating-system state
before execution. Unsupported hardware is reported as a skip. A compile-only
check or skipped runtime check does not establish native instruction behavior.

The separate [WebAssembly workflow](https://github.com/ekmett/native/blob/main/.github/workflows/wasm.yml)
runs only by manual dispatch. It checks SIMD128 and relaxed SIMD on x86-64 and
ARM64 hosts, including relocated consumers and compiler-minimum boundaries.
Paired probes compare compiled Wasm
bytecode; they do not measure engine JIT machine code or execution overhead.
Separate raw-engine jobs verify the built modules' source revision and hashes
before testing Node and Wasmtime. Engine conformance probes are advisory: failures
produce warnings and job summaries, with full test results retained as artifacts.
They do not fail the workflow or block library qualification. Setup and provenance
checks remain gating. The [relaxed SIMD checks](../tests/wasm_relaxed/README.md)
document the known discrepancies; passing library checks does not establish full
engine conformance.
New manual Wasm runs cancel older runs on the same ref. Library and engine
artifacts share the three-day retention limit.

Documentation checks run for relevant PR and main changes. Successful main
builds still publish GitHub Pages; the deployment artifact expires after one
day, while HTML and diagnostics are retained for three days.

## Values and memory

The [core fixtures](https://github.com/ekmett/native/blob/main/tests/core_regression/README.md) exercise construction,
scalar/vector/wide consistency, integer wrapping, masks, bit transfers and
floating-point boundary cases through headers and module imports.

Short vectors and swizzles have checks for:

- Logical two- and three-lane access in padded native registers.
- Guarded page boundaries, partial tails and zero-length null-pointer access.
- Owning reads, overlapping writes, repeated read indices and rejected writes
  through const values, temporaries or repeated indices.
- Constant construction, physical layout and preservation of transported bits.
- Exclusion of padding lanes from reductions and floating-point exceptions.

The [instruction-storage fixtures](https://github.com/ekmett/native/blob/main/tests/instruction_storage/README.md) cover
register shapes used by individual extensions. They check type and alignment
identity, logical memory bounds, register bit bridges, mask operations and
rejection of unsupported arithmetic. Storage availability does not imply that
all arithmetic operations exist for that type.

## Instructions and constant evaluation

Each instruction family has focused value oracles, feature constraints,
immediate-operand checks and native execution tests. Public vector interfaces
are tested with `simd` values, while scalar forms use their declared C++ types.
Wrong widths, incompatible shapes and unsupported target scopes have actual
compile-failure tests.

Paired code-generation probes compare optimized public calls with raw intrinsic
leaves. They check that wrappers add no instruction sequence, call or memory
traffic. Separate runtime oracles check results and observable status flags;
matching assembly alone is not a numerical or floating-point-environment test.

Constant-evaluation coverage uses `static_assert` with boundary cases and
independent semantic oracles. Where an operation offers a below-feature
polyfill, an immediate-only overload accepts constant inputs; a compile-failure
fixture rejects runtime inputs. Feature-enabled runtime calls retain their
native implementation. The [instruction guide](instructions.md) and individual
family pages describe which operations provide this support.

Generated property tests use reproducible seeds and print the seed, case index
and operand bits on failure. `NATIVE_TEST_SEED` selects a seed and
`NATIVE_TEST_CASES` adjusts the runtime corpus size. Constant corpora also compare
public results evaluated at compile time with hardware results at runtime;
independent integer oracles cover wrapping, saturation, lane selection and
polynomial arithmetic.

## Numerical contracts

Scalar half-format tests cover every 16-bit representation, finite round trips,
adjacent representable midpoints and their binary32 neighbors, overflow and
numeric limits. Bit transport is checked separately from numerical conversion.
The half storage types' integer rounding policy is independent of native half
arithmetic and its floating-point controls.

Directed rounding checks include signed zeros, infinities, integer boundaries
and all four standard rounding modes. Raw scaling fixtures exercise gradual
and flush modes, including separate DAZ and FTZ controls on x86. Test harnesses
save and restore the caller's floating-point environment.

[Profile captures](https://github.com/ekmett/native/blob/main/tests/isa_profiles/README.md) compare headers, module
imports and admitted implementations on a finite bank. Independent references
cover elementary operations; wide exponential results are also compared with
the direct raw kernel. These comparisons do not establish equality for every
floating-point input, arbitrary user element types or unrelated math libraries.

`wide` tests check ADL and array-overload selection, empty packs, result types,
exception specifications and construction. Custom element tests exercise the
same extension interface used by downstream numerical libraries.

## Modules and installed packages

Module fixtures check type identity, baseline imports, explicit stronger tags,
source-target variants and exported customization points. Default ISA arguments
are checked against the owning module's compiler baseline. Changing an enclosing
function target does not change that default. Frozen-BMI probes distinguish
accepted imports from compiler rejection of incompatible target configurations.

Installed-package tests physically move the prefix, including to paths with
spaces, before configuring consumers. They require the original prefix to be
absent, reject source-tree implementation include paths and compare installed
file hashes before and after consumer builds. Granular imports, the `native`
umbrella and separate numerical imports are exercised through the package.

Compiler version, language mode, exception settings and standard-library
configuration must agree across a module boundary. A successful source-tree
build does not replace installed-consumer validation.

## Compiler cache

On POSIX hosts, the compiler launcher expands recognized CMake module-map
response files for sccache and preserves their effective arguments. Unknown or
ambiguous syntax uses the compiler directly.

Windows clang-cl module providers and importers, PCH commands, and commands with
response files use the compiler directly with unchanged arguments. sccache 0.16
does not hash BMI contents for raw clang-cl module-file flags. Ordinary clang-cl
compilations remain cached. Launcher tests cover both platforms, including
whitespace, quoting, missing inputs, fallback behavior and compiler exit status.

Cache fixtures distinguish cold compilation, unchanged reuse and invalidation
following changed module or header inputs. Fresh importers check the observable
module result after each step. Cache statistics exclude commands that do not
pass through the launcher; cache hits alone do not establish a faster build.

### PCH-dependent module invalidation

On POSIX hosts, explicit PCH binaries are included in `SCCACHE_EXTRAFILES`,
alongside existing entries. Unknown response-file or PCH syntax bypasses caching.
Compiler module validation remains enabled.

The PCH fixture changes PCH bytes while retaining equivalent preprocessing,
checks reuse afterward, and also checks unchanged bytes with changed timestamps.
Each step compiles a fresh importer so Clang checks the restored module against
the actual PCH. The fixture uses its own cache and server and retains per-step
hit/miss and error counters. Consumer PCH use remains optional.

## ISA value API tooling

ISA property reads and writes through `import native;` are checked with warnings
treated as errors for all three families, including foreign metadata. Including
`<native/isa.h>` after the import also passes. Internal module fragments import
the ISA owner before parsing shared headers so their property declarations merge.

Clang 23 still reports `-Wmodules-ambiguous-internal-linkage` when the textual
ISA header precedes the import: the local property declaration conflicts with
the imported one. Import first, or use `A.has(native::x86_feature::fma)` for
feature checks in that arrangement. The library does not suppress the warning.

On Linux and macOS, Clang 23 cannot mangle a direct property expression in a
function constraint such as `requires(A.avx2 && A.fma)`. Use
`requires(A.has(x86_feature::avx2 & x86_feature::fma))`, `target<A, ...>`, or a
named concept containing the property expression. Ordinary constant-evaluated
property reads and writes are supported. The tests cover named concepts and
matching declarations and definitions using `has`.

LLVM 23's Windows demangler does not decode the structural array template
arguments in source-variant symbols. Code-generation checks recognize their
mangled names directly; this affects symbol inspection rather than dispatch.
