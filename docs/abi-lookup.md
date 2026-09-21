# ISA values and target selection

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

`isa<Family>` is a structural set of instruction requirements that can be a
template argument. `isa<>` uses the compiler target's family; `isa<x86>`,
`isa<arm>` and `isa<wasm>` name a family explicitly. Their metadata is available
on every host, even when that host cannot compile or execute the instructions.

`architecture` names the families `x86`, `arm`, `wasm` and `unknown`. The short
constants `x86`, `arm` and `wasm` denote those enum values. `target_arch`, available
from `<native/config.h>`, identifies the compiler target. It says nothing about
optional instructions or runtime CPU capabilities.

## Construct and inspect requirements

Each family accepts its own feature enum. Construction sets exactly the named
bits; `&` combines requirements by union:

```cpp
using namespace native;

constexpr isa<x86> needs = [] {
  using enum x86_feature;
  return avx2 & fma & f16c;
}();
static_assert(needs.has(x86_feature::fma));
static_assert(x86_feature::avx2 <= needs);
static_assert(needs <= avx512);

constexpr isa<arm> crypto = [] {
  isa<arm> a;
  a.aes = true;
  a.pmull = true;
  return a;
}();
static_assert(crypto.has(arm_feature::aes & arm_feature::pmull));
```

The properties belong to their family: `.aes` on `isa<arm>` means ARM AES, while
`.aes` on `isa<x86>` means x86 AES. Foreign properties, enum arguments,
conversions and conjunctions are ill-formed. Even empty sets from different
families cannot be converted or combined.

`isa(x86_feature::avx2)` deduces `isa<x86>`. An explicit `isa<>` keeps the host
family and accepts only that family's enum. `scalar` is the empty host set;
use `isa<arm>{}`, for example, when describing an empty ARM requirement on
another host.

`get`, `set`, `has`, equality and the feature properties operate on exact bits.
`a <= b` means every bit of `a` occurs in `b`; `<` means strict inclusion. The
reverse comparisons have the corresponding meanings. Distinct singleton
features are incomparable. These operations require the same family. There is
no `|` operator for requirements.

An out-of-range enum reads as absent. Requiring it through construction or
`set(value, true)` records an invalid requirement; clearing it does nothing.
`valid()` also detects unregistered padding bits written into the public
`flags` array. Admission rejects invalid requested, present or observed sets.
The enum ordinals and structural storage are not a serialization format.

Construction does not add prerequisites:

```cpp
constexpr isa<x86> one = x86_feature::avx2;
static_assert(one.avx2 && !one.avx && !one.fma);
constexpr auto complete = feature_closure(one);
static_assert(complete.avx);
```

`feature_closure` preserves the family and adds registered instruction and
register prerequisites. Target parsing and runtime admission apply it as well.
The `avx2`, `avx512`, `avx512_bf16` and `avx512_fp16` presets are `isa<x86>`;
`neon`, `neon_fp16` and `neon_bf16` are `isa<arm>`. Each preset includes its
prerequisite closure. The `avx2` preset also requests FMA.

For function constraints, use `requires(A.has(x86_feature::avx2 & x86_feature::fma))`
or the selector below. Clang 23's Linux/macOS mangler rejects direct property
expressions such as `requires(A.avx2 && A.fma)`; see the
[tooling limits](validation.md).

## Select an implementation

`target<A, Choices...>` is the zero-based index of the first choice contained
in `A`, or `-1` when none matches. An empty choice pack also returns `-1`. Every
choice must belong to `A`'s family.

```cpp
template<isa<x86> A>
inline constexpr int operation_target = target<A, avx512, avx2>;

static_assert(operation_target<avx512_bf16> == 0);
static_assert(operation_target<avx2> == 1);
static_assert(operation_target<isa<x86>{}> == -1);
static_assert(target<x86_feature::avx2, x86_feature::avx2> == 0);

template<isa<x86> A> requires(target<A, avx512, avx2> == 1)
void operation(float const * input, float * output);
```

Selection compares the exact sets supplied; it does not add prerequisites.
An empty final choice matches every set in the same family. Additional features
in `A` remain part of argument and result types.

Put stronger requirements before weaker ones. Every pair `i < j` is checked:
if `Choices[i] <= Choices[j]`, the later choice is unreachable and compilation
fails. This rejects duplicates and backward subsumption even when an earlier
choice already matched:

```cpp
// Each declaration is intentionally ill-formed.
// constexpr int shadowed = target<avx512, avx2, avx512>;
// constexpr int duplicate = target<avx2, avx2, avx2>;
// constexpr int late = target<avx512, avx512, isa<x86>{}, avx2>;
// constexpr int foreign = target<neon, avx2>;
```

Incomparable choices may appear in either order; the first matching one wins.
The `arch` concept accepts a feature enum or an `isa` specialization. Generic
metadata code can use `template<arch auto A>` or `template<isa A>`; a native
vector algorithm uses the host type, `template<isa<> A>`.

The helper is available through `<native/isa.h>` in C++20 or `import native;`.
It performs compile-time selection. Native implementations still need matching
compiler targets and CPU/OS admission before execution. See the
[source-target guide](omnibus.md).

## Extend an overload set

A function can add an implementation with its own disjoint constraint without
changing an existing choice pack. Guard native definitions by CPU family:

```cpp
#include <native/config.h>
import native;
using namespace native;

template<isa<> A>
void double16(float * out, float const * in) = delete;

#if NATIVE_HOST_X86
template<isa<> A> requires(target<A, avx512, avx2> == 0)
void double16(float * out, float const * in);

template<isa<> A> requires(target<A, avx512, avx2> == 1)
void double16(float * out, float const * in);
#elif NATIVE_HOST_NEON
template<isa<> A> requires(A.has(arm_feature::neon))
void double16(float * out, float const * in);
#endif
```

The deleted primary rejects tags with no implementation. Family-typed metadata
is available everywhere, but native vectors and intrinsic declarations belong
to the compilation target. Constraints cannot hide a foreign header or defer
Clang's processing of a target attribute.

Within one family, additional overloads must avoid ambiguous overlap. The
`target` helper checks its own choices, not other function declarations. Declare
every overload before defining a dispatcher that calls `double16<A>`: the
pointer arguments do not let ADL discover later overloads. Implementations beyond
the project minimum need matching target scopes; see the
[direct kernel example](omnibus.md).

## Compiler baseline

`NATIVE_BASELINE` from `<native/targets.h>` is an `isa<>` expression for registered
features enabled in the current translation unit:

```cpp
#include <native/targets.h>
import native;
using namespace native;

constexpr isa<> baseline = NATIVE_BASELINE;
```

It reads Clang's resolved feature macros, including features implied by CPU or
compiler options. Explicit negative flags remain effective:
`-msse4.2 -mno-popcnt` includes SSE4.2 and CRC32 but excludes POPCNT, and `-mavx2`
does not establish FMA. This value adds neither prerequisite closure nor extra
project requirements. Unregistered extensions do not invalidate known bits.

ARM SIMD features also require enabled vector registers. Architecture macros
that survive `-mgeneral-regs-only` do not authorize those operations. Scalar FP16
support alone does not establish NEON FP16 support.

The baseline describes compiler permissions, not the runtime CPU or every
instruction in an executable. Function target attributes and target pragmas do
not update it; a function that explicitly disables features can have a weaker
target. Intrinsic target and ABI constraints still apply.

The expression is textual and captures consumer flags. Defaults declared by an
owning module instead capture that module's baseline once: importing the module
under stronger flags does not change those defaults. Keep local snapshots local;
do not give a shared inline definition different meanings across translation
units by compiling it with different baseline macros.

`NATIVE_TARGET_MINIMUM` is the conservative source-variant admission requirement.
It adds prerequisite closure, `NATIVE_TARGET_EXTRA_MINIMUM` and the rejection
marker for recognized but unregistered extensions. Any extra minimum must belong
to the compiler target's family.

## Compiler-minimum metadata

`isa_list<...>` and `abi_lookup<A, List>` retain ordered metadata for source
variants and composed kernels. Use `target` for ordinary exact-set selection.
A `target_entry<Family>` records requested features and an inherited compiler
minimum from the same family; its constructor deduces the family:

```cpp
constexpr auto inherited = target_entry{avx2, x86_feature::avx512vl};
using choices = isa_list<inherited, avx2>;
using picked = abi_lookup<avx512, choices>;
static_assert(picked::index == 0);
static_assert(picked::architecture == avx2);
static_assert(picked::minimum == x86_feature::avx512vl);
```

The lookup exposes `matched`, an `int index`, and same-family `isa` values
`architecture`, `minimum` and `required_features`. It closes prerequisites when
checking the requested features and minimum. Every list entry must match `A`'s
family, including entries after the first match. Unknown requirement bits are
diagnosed. No match gives `matched == false`, `index == -1` and empty values of
the same family.

Composed kernels whose callees have different lists need a common refinement
that preserves each callee's first match. Those checks stay internal. See the
[exp tests](../tests/exp_policy_refinement/README.md) for value and code-generation
coverage.

## ARM capabilities and compiler targets

ARM crypto features are reported independently: `aes`, `pmull`, `sha1`,
`sha2` (SHA-256), `sha512` and `sha3`. Observing AES does not establish PMULL.
Each retains its own `present` and `observed` bit; a failed query stays unknown.
On Windows, a positive legacy crypto query establishes AES, PMULL, SHA1 and
SHA256 together. A negative result cannot identify the missing components,
so their individual observations remain unknown.

Compiler target names describe bundles. `target_features<arm>("neon,aes")`
requires AES and PMULL; `target_features<arm>("sha2")` requires SHA1 and SHA256;
`target_features<arm>("sha3")` also requires SHA512 and SHA3. These sets include
NEON. Omitting the template argument parses names for `target_arch`; use an
explicit family for foreign metadata. Names from another family are invalid.

`feature_closure(arm_feature::aes)` adds only its register prerequisite, NEON.
An instruction wrapper can therefore express AES independently, while its
caller admits the full target used to compile the containing function.
Hardware-only names `pmull`, `sha1`, `sha512` and `ebf16` are invalid target
strings because LLVM does not provide matching standalone target switches.

`ebf16` reports enhanced BF16 arithmetic support separately from `neon_bf16`.
It does not set FPCR.EBF or change floating-point state. Ordinary BF16 admission
does not require it; requesting enhanced BF16 also requires BF16.

The mappings follow [LLVM's AArch64 feature definitions](https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/AArch64/AArch64Features.td),
[ARM64 OS capabilities](https://docs.kernel.org/arch/arm64/elf_hwcaps.html), and
[Windows processor-feature queries](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent).
