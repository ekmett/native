# Compile-time implementation policies

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

`abi_lookup<Arch, Policies>` selects the first entry in an `isa_list` whose
required features are contained in `Arch::features`. It is available through
`<simd/isa.h>` in C++20 and through `import simd;`. It performs no CPU query.
Use `with_isa` separately when admitting code for execution on a particular CPU.

```cpp
using base = simd::isa<simd::avx2::features | simd::feature::avx512f |
                       simd::feature::avx512dq>;
using with_vl = simd::isa<base::features | simd::feature::avx512vl>;
using policies = simd::isa_list<with_vl, base, simd::avx2>;
using choice = simd::abi_lookup<simd::avx512, policies>;
static_assert(choice::matched && choice::index == 0);
static_assert(std::same_as<choice::type, with_vl>);
```

`index` is a zero-based position in that particular list. A match exposes
`type` (the original entry), `architecture` (its requested tag), `minimum`
(its inherited compiler requirements), and `required_features` (the prerequisite
closure of the requested and inherited features). Ordinary tag entries have a
zero minimum. For example, `target_entry<avx2, avx512::features>` requires all
AVX-512 preset features to match; an `avx2` caller alone is insufficient. The
result still retains that exact `target_entry` and its requested `avx2` tag.

For an empty list or no match, `matched` is false, `index` is `abi_npos`, both
type aliases are `void`, and both feature fields are zero. Check `matched`
before treating the feature fields as a policy. No scalar fallback is added.
An explicit `scalar` entry has zero requirements and matches every architecture,
so put it last when that is the desired fallback.

Order also resolves overlapping and incomparable requirements. A tag containing
both BW and VL can match either a BW entry or a VL entry; the earlier one wins.
Put a combined entry before both if that intersection needs its own body.
Feature prerequisites are normalized, but entries are neither sorted nor
deduplicated.

`requires_abi<Arch, Policies, I>` accepts precisely the tags selecting position
`I`. It is false for no-match, even when `I == abi_npos`, so overload constraints
for different positions are disjoint:

```cpp
template<simd::architecture Arch, std::size_t L, std::size_t N>
  requires simd::requires_abi<Arch, policies, 0>
__attribute__((target("avx2,fma,bmi2,avx512f,avx512dq,avx512vl")))
simd::wide<simd::vec<float,L,Arch>,N>
operation(simd::wide<simd::vec<float,L,Arch>,N> const & input);
```

This declaration illustrates one policy overload; other positions need their
own definitions and target attributes. Keep the original `Arch` in arguments,
intermediates and results. Selecting a sufficient policy does not retag values,
and different caller tags can still create different template instantiations.
`requires_abi` selects an overload; it does not target its body. Every called
helper must also be legal under that body's target requirements.

Choose policies per operation and account for element type, register width and
numerical policy separately. For a composed operation, refine the choices of
all dependencies: the tuple of their selected positions identifies a combined
case. Concatenating their lists does not compute that refinement. This lookup
does not change existing `exp` arithmetic or the current `wide` target families.
