# SHA-1 and SHA-256 instruction primitives

`import native.x86.sha;` provides the seven Intel SHA instruction primitives.
Link `native::native`; the `native.x86` and `native` hubs also export them.
Every argument and result is `simd<std::uint32_t,4,Arch>`. The four dwords
contain parts of one hash state or message schedule, rather than four hashes.

| Operation | Meaning |
| --- | --- |
| `sha1rnds4<Arch,Selector>(state, message)` | Four SHA-1 rounds; selector 0–3 chooses the function and round constant |
| `sha1nexte<Arch>(state, message)` | Add the derived E value to the high message dword |
| `sha1msg1<Arch>(a, b)` | First step in computing four SHA-1 schedule words |
| `sha1msg2<Arch>(a, b)` | Final step in computing four SHA-1 schedule words |
| `sha256rnds2<Arch>(cdgh, abef, message)` | Two SHA-256 rounds, returning updated ABEF |
| `sha256msg1<Arch>(a, b)` | First step in computing four SHA-256 schedule words |
| `sha256msg2<Arch>(a, b)` | Final step in computing four SHA-256 schedule words |

SHA-1 state is `[D,C,B,A]` in increasing dword order. The message operand of
`sha1rnds4` is `[W3,W2,W1,W0+E]`. The instruction includes its phase constant;
selectors 0, 1, 2 and 3 correspond to rounds 0–19, 20–39, 40–59 and 60–79.
`sha1nexte` preserves message dwords 0–2 and adds `rotr(state[3],2)` to dword 3.
SHA-1 schedule inputs and outputs put their earliest word in the high dword.

SHA-256 uses `cdgh=[H,G,D,C]` and `abef=[F,E,B,A]`. The message operand contains
`[W0+K0,W1+K1,unused,unused]`; its high two dwords do not participate.
The result is the updated `[F,E,B,A]`. The previous ABEF becomes CDGH after
those two rounds. SHA-256 schedule operands place the earliest word in the low
dword. Message primitives are partial schedule steps: SHA-1 also needs an XOR
with the intervening words, and SHA-256 needs an addition of the intervening
words between its two primitives. The fixture composes both schedules and
round primitives into SHA-1 and SHA-256 `"abc"` digest checks.

Use `target_features<native::x86>("sha")`. Runtime calls require SHA, complete
SSE2 register storage, and a caller compiled for `"sha"`. SHA is independently
decoded from CPUID leaf 7, subleaf 0, EBX bit 29. It does not require AVX or OS
AVX state, and is not added to the general `avx2` or `avx512` profiles. Admit
the target with `classify_isa` before entering an instruction-bearing function.
The instructions have no writemask forms.

Instruction-bearing tags use `constexpr` overloads with native runtime paths.
Tags lacking SHA use `consteval` overloads, while still requiring complete
128-bit storage. All arithmetic wraps modulo 2³². `Selector` is an unsigned
compile-time value in 0–3; other selectors, raw registers, mixed architecture
tags and other vector shapes are rejected. These operations do not add a
runtime software hash implementation or perform padding, endian conversion,
feed-forward, or complete hashing on behalf of the caller.

The [fixture](../tests/x86_sha/README.md) describes independent scalar checks,
known digests, runtime admission, rejection controls and raw/public codegen.
Packing and instruction semantics follow Intel's
[SHA extensions description](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sha-extensions.html)
and [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).
Algorithm equations follow [FIPS 180-4](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf),
sections 4.1, 4.2, 6.1 and 6.2; intrinsic signatures follow Clang's
[SHA header](https://clang.llvm.org/doxygen/shaintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
