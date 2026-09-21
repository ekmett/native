# ISA values and target selection

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

`isa` is a structural feature set that can be a template argument.
`x86_feature` and `arm_feature` name the respective instruction features; `&` combines their requirements by union:

```cpp
using namespace native;

constexpr isa needs = [] {
  using enum x86_feature;
  return avx2 & fma & f16c;
}();

static_assert(needs.has(x86_feature::fma));
static_assert(x86_feature::avx2 <= needs);
static_assert(needs <= avx512);

constexpr isa adjusted = [] {
  auto a = avx2;
  a.f16c = true;
  a.bmi2 = false;
  return a;
}();
static_assert(adjusted.f16c && !adjusted.bmi2);
```

The two enums have independent ordinals: `x86_feature::aes` and
`arm_feature::aes` map to different bits in `isa`. ARM names omit the redundant
`arm_` prefix; existing ISA properties such as `a.arm_aes` retain it to
distinguish the two architectures. `x86_feature_count` and `arm_feature_count`
bound the respective enum ranges. Ordinals and the shared storage layout are
not a stable serialization format.

An out-of-range enum value reads as absent. Requiring it, through construction
or `set(value, true)`, records an invalid requirement so admission fails closed.
`set(value, false)` leaves the set unchanged for an out-of-range value.

The properties read and update the set's bits; they store no additional state.
`a.has(b)` accepts a feature or another `isa`. `a <= b` means every bit of `a`
occurs in `b`, and `<` means strict inclusion. The reverse comparisons have the
corresponding meanings. This is a partial order: distinct singleton features
are incomparable. `&` works for every feature/ISA pairing; there is no `|`
operator.

For function constraints, use `requires(A.has(x86_feature::avx2 & x86_feature::fma))`
or the `target` selector below. Clang 23's Linux/macOS mangler rejects direct
property expressions such as `requires(A.avx2 && A.fma)`; see the
[tooling limits](validation.md).

Default construction gives the empty set, equal to `scalar`. Construction from
one feature sets exactly one bit. It never adds implied features:

```cpp
constexpr isa one = x86_feature::avx2;
static_assert(one.avx2 && !one.avx && !one.fma);
constexpr isa compiler_features = feature_closure(one);
static_assert(compiler_features.avx);
```

`feature_closure` explicitly adds compiler prerequisites. Compiler target parsing
and CPU admission apply that closure. The existing `scalar`, `avx2`, `avx512`,
`avx512_bf16`, `avx512_fp16`, `neon`, `neon_fp16` and `neon_bf16` presets are
`constexpr isa` values whose prerequisite closure is already included. For
example, the `avx2` preset also requests FMA. CPU-model bundles remain
future work.

The feature enumerators `x86_feature::avx512bf16` and `x86_feature::avx512fp16` name single
bits. The presets `avx512_bf16` and `avx512_fp16` include the broader AVX-512
requirements.

## Typed feature sets

`feature_set<x86_feature>` and `feature_set<arm_feature>` store one family's
features. Both are structural types usable as value template arguments, with
`get`, `set`, `has`, equality and public word storage. Their methods accept only
the matching enum. Conversion to `isa` preserves the exact set without adding
compiler prerequisites. Typed sets also work with `target`, subset comparisons
and requirement conjunction through the `arch` concept.

```cpp
constexpr feature_set<x86_feature> detected = x86_feature::avx2;
static_assert(isa(detected) == isa(x86_feature::avx2));
static_assert(!detected.has(x86_feature::avx));
static_assert(target<detected, x86_feature::avx2, scalar> == 0);
```

An invalid enum passed to `set(value, true)` records an invalid set; reading it
returns false and clearing it does nothing. `valid()` also detects unregistered
padding bits written directly into storage. Conversion preserves that invalid
requirement, and normalized CPU admission rejects invalid present/observed sets.

## Select an implementation

`target<A, Choices...>` is an `int`: the zero-based index of the first choice
contained in `A`, or `-1` when none matches. An empty choice pack also returns
`-1`. The `arch` concept admits either feature enum, either typed feature set, or `isa`; generic value
parameters can use `template<arch auto A>`. Vector algorithms normally use
`template<isa A>`:

```cpp
template<isa A>
inline constexpr int operation_target = target<A, avx512, avx2>;

static_assert(operation_target<avx512_bf16> == 0);
static_assert(operation_target<avx2> == 1);
static_assert(operation_target<scalar> == -1);
static_assert(target<x86_feature::avx2, x86_feature::avx2> == 0);

template<isa A> requires(target<A, avx512, avx2> == 1)
void operation(float const * input, float * output);
```

Selection compares the exact sets supplied; it does not add prerequisites.
An explicit final `scalar` matches every set. `A` may contain features beyond
the selected requirement, and the complete caller ISA remains in argument and
result types.

Put stronger requirements before weaker ones. Every pair `i < j` is checked:
if `Choices[i] <= Choices[j]`, the later choice is unreachable and compilation
fails. This rejects duplicates and backward subsumption, even if an earlier
choice already matched or the supplied `A` matches nothing:

```cpp
// Each declaration below is intentionally ill-formed.
// constexpr int shadowed = target<avx512, avx2, avx512>;
// constexpr int duplicate = target<avx2, avx2, avx2>;
// constexpr int unmatched = target<neon, avx2, avx512>;
// constexpr int late = target<avx512, avx512, scalar, avx2>;
```

Incomparable choices may appear in either order; the first matching one wins.
The helper is available through `<native/isa.h>` in C++20 or `import native;`.
It performs compile-time selection only. Give native implementations their
required Clang target attributes, and use `with_isa` for CPU/OS admission before
execution. See the [source-target guide](omnibus.md).

## Extend an overload set

`target` does not close the function's overload set. A new implementation can
use its own disjoint constraint without changing the existing choice pack.
Across CPU families, select the native definitions with preprocessing first:

```cpp
#include <native/config.h>
import native;
using namespace native;

template<isa A>
void double16(float * out, float const * in) = delete;

#if NATIVE_HOST_X86
template<isa A> requires(target<A, avx512, avx2> == 0)
void double16(float * out, float const * in);

template<isa A> requires(target<A, avx512, avx2> == 1)
void double16(float * out, float const * in);

#elif NATIVE_HOST_NEON
template<isa A> requires(A.has(arm_feature::neon))
void double16(float * out, float const * in);
#endif
```

These are constrained function overloads. The deleted primary rejects ISAs
with no implementation. The ISA metadata is available on every host, but
`import native` exposes native vectors for the compilation target. Guard native
headers and target-attribute scopes by CPU family. Constraints cannot hide a
foreign header or defer Clang's processing of a target attribute.

You can instead leave the NEON overload in shared source by making its body
dependent. With NEON as the ARM project minimum, this definition needs no
additional target scope:

```cpp
template<isa A>
  requires(A.has(arm_feature::neon) && requires { sizeof(simd<float, 4, A>); })
void double16(float * out, float const * in) {
  using V = simd<float, 4, A>;
  for (unsigned i = 0; i < 16; i += 4) {
    auto x = V::load(in + i);
    (x + x).store(out + i);
  }
}
```

The `sizeof` requirement checks that this build provides the vector type. On
x86 it rejects the NEON overload without instantiating its body. Keeping `A`
in the vector type makes lookup dependent; replacing it with the concrete
`neon` value requires that native type while parsing the definition. A failed
constraint removes a candidate; an error inside an instantiated body does not.

The NEON case is an ARM implementation of the same interface, not another
variant emitted into an x86 binary. Since the x86 overloads are absent from
the ARM build, its constraint needs no reference to the x86 choice pack.
At the metadata level, `A.has(arm_feature::neon) && target<A, avx512, avx2> == -1`
expresses disjointness even for synthetic mixed feature sets; that expression
does not replace a host guard or CPU/OS admission.

Within one build, an additional overload still needs to avoid ambiguous
overlap with the existing ones. The target helper checks its own choice pack,
not the other function overloads.

Declare every overload before defining a dispatcher template that calls
`double16<A>`. With these pointer arguments, argument-dependent lookup cannot
discover an overload declared later. Native code beyond the project minimum
needs matching Clang target scopes in builds for its CPU family; see the
[direct kernel example](omnibus.md).

Use `has` in these constraints. Adding `!!` to `A.neon` does not remove the
property expression that Clang's Linux/macOS mangler rejects.

## Compiler baseline

`NATIVE_BASELINE` from `<native/targets.h>` is a constant `isa` expression for
the registered instruction features enabled in the current translation unit:

```cpp
#include <native/targets.h>
import native;
using namespace native;

constexpr isa baseline = NATIVE_BASELINE;
```

The snapshot reads Clang's resolved feature macros, including features implied
by a CPU model or another compiler option. Explicit negative flags are retained:
`-msse4.2 -mno-popcnt` includes SSE4.2 and CRC32 but excludes POPCNT. Likewise,
`-mavx2` does not establish FMA. No prerequisite closure or additional project
requirement is applied to this value. Unregistered extensions do not invalidate
the registered features that Clang reports.

ARM SIMD features also require enabled vector registers. Architecture macros
that survive `-mgeneral-regs-only` do not establish permission to use those
operations. Scalar FP16 support alone does not establish NEON FP16 support.

This is a compiler permission set, not a runtime CPU query or a complete list of
the instructions in an executable. Normal code may rely on its baseline once
the application has established its deployment requirements. Function target
attributes and target pragmas do not update the snapshot; a function that
explicitly disables features can have a weaker target. Intrinsic target and ABI
constraints still apply.

The expression is textual so it captures the consumer's flags. A constant
exported from a precompiled module would describe that module's compilation.
Keep local bindings local, and pass the structural value explicitly when it
selects a template specialization. Do not use differing compiler baselines to
give a shared inline definition different meanings across translation units.

`NATIVE_TARGET_MINIMUM` remains the conservative source-variant admission
requirement: it adds prerequisite closure, `NATIVE_TARGET_EXTRA_MINIMUM` and
the existing rejection marker for recognized but unregistered extensions.

## Compiler-minimum metadata

`isa_list<...>` and `abi_lookup<A, List>` retain the internal ordered metadata
used by source variants and composed kernels. Use the direct choice pack above
for ordinary target selection. A structural `target_entry{architecture, minimum}`
records an ISA and its inherited compiler minimum:

```cpp
constexpr auto inherited = target_entry{avx2, x86_feature::avx512vl};
using choices = isa_list<inherited, avx2>;
using picked = abi_lookup<avx512, choices>;
static_assert(picked::index == 0);
static_assert(picked::architecture == avx2);
static_assert(picked::minimum == x86_feature::avx512vl);
```

The lookup exposes `matched`, an `int index`, and `isa` values `architecture`,
`minimum` and `required_features`. It includes compiler prerequisite closure
when testing the entry's requested ISA and minimum. Unknown minimum bits are
diagnosed. No match gives `matched == false`, `index == -1` and empty ISA values.

The built-in FP32 exp kernel has five implementation choices because its raw
callees share five declaration scopes. FP32, integer and mask values ignore
unrelated half features; native FP16 and BF16 values require their own extension.
Internal traits handle these built-in requirements. Custom types keep their
declared ISA without an extra metadata protocol.

Composed kernels whose callees have different lists need a common refinement
that preserves each callee's first match. Those checks stay internal. See the
[exp tests](../tests/exp_policy_refinement/README.md) for target, value and codegen
coverage.

## ARM capabilities and compiler targets

ARM crypto features are reported independently: `aes`, `pmull`, `sha1`,
`sha2` (SHA-256), `sha512` and `sha3`. For example, observing AES does not
establish PMULL support. Each feature retains its own `present` and `observed`
bit; an unavailable query stays unknown. On Windows, a positive legacy crypto
query establishes AES, PMULL, SHA1 and SHA256 together. A negative result cannot
identify the missing components, so their individual observations remain unknown.

Compiler target names still describe bundles. `target_features("neon,aes")`
requires both AES and PMULL; `target_features("sha2")` requires SHA1 and SHA256;
`target_features("sha3")` also requires SHA512 and SHA3. These sets include NEON.
The source-target macros retain the same requirements in their admission metadata
and capture the translation unit's inherited crypto features.

`feature_closure(arm_feature::aes)` only adds its register prerequisite, NEON.
An instruction wrapper can therefore express an AES requirement independently,
while the caller checks the full target used to compile its function. Check the
compiler target's set before entering that function, even if its current body
uses only one instruction from the bundle. Hardware-only names such as `pmull`,
`sha1`, `sha512` and `ebf16` are rejected as target strings: LLVM does not provide
matching standalone target switches.

`ebf16` reports enhanced BF16 arithmetic support separately from `neon_bf16`.
It does not set FPCR.EBF or otherwise change floating-point state. Ordinary BF16
admission does not require it; requesting enhanced BF16 also requires BF16.

The mappings follow [LLVM's AArch64 feature definitions](https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/AArch64/AArch64Features.td),
[ARM64 OS capabilities](https://docs.kernel.org/arch/arm64/elf_hwcaps.html), and
[Windows processor-feature queries](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent).
