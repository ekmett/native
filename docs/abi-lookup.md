# Targets

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

`target<A, Choices...>` returns the zero-based index of the first choice whose
features are a subset of `A`. Extra flags on `A` do not matter. Put more specific
choices first; an earlier weak choice shadows a later strong one.

```cpp
using namespace simd;

template<class A>
inline constexpr auto exp_target = target<A, avx512, avx2>;

static_assert(exp_target<avx512_bf16> == 0);
static_assert(exp_target<avx2> == 1);

template<architecture A> requires(exp_target<A> == 1)
void operation(A);
```

An empty list or no match gives `target_npos`. An explicit final `scalar` choice
matches any valid tag. Invalid tag types fail substitution. Feature prerequisites
are normalized; list order is preserved.

The helper is available through `<simd/isa.h>` in C++20 or `import simd;`.
It selects an overload. Give each implementation its required Clang target
attributes, and use `with_isa` for CPU admission before running it. The complete
caller tag stays in argument and result types.

A reusable `isa_list` works in place of the pack:

```cpp
using choices = isa_list<avx512, avx2>;
static_assert(target<avx512, choices> == 0);

template<requires_target<0, choices> A>
void operation(A);
```

`requires_target<A, I, Choices...>` is the same check as a constraint, with the
index before the pack so it also works as a constrained type parameter. It is
false for invalid types and for the no-match sentinel.

For selected-type introspection, `abi_lookup<A, choices>` exposes `index`,
`matched`, the original entry as `type`, its `architecture`, and its
`required_features`. No match produces void types and zero feature fields.
A `target_entry<Tag, Minimum>` includes inherited compiler requirements in the
subset test while preserving the original tag and entry. Malformed minimum
bits are diagnosed.

The built-in FP32 exp kernel has five target choices because its raw callees
share five declaration scopes. `exp_target<A>` selects that list directly.
FP32, integer and mask values ignore unrelated half features; native FP16 and
BF16 values require their own extension. Internal traits handle these builtin
requirements. Custom types keep their declared architecture without any extra
metadata protocol.

Composed kernels whose callees have different lists need their common refinement,
not concatenation. Those checks stay internal. See the
[exp tests](../tests/exp_policy_refinement/README.md) for target, value and codegen
coverage.
