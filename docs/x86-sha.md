# x86 SHA: SHA-1 and SHA-256 rounds

[x86 instruction sets](x86.md)

## Why use it

SHA compression repeatedly applies the same round and message-schedule
functions. The SHA extension groups that work into instructions for one hash
state, reducing the rotations, Boolean operations and additions needed by a
compression implementation.

## Operations

`import native.x86.sha;` supplies seven primitives through `native::native`.
`native.x86` and `native` also export them. All arguments and results are
`simd<std::uint32_t,4,Arch>`.

| Operation | Meaning |
| --- | --- |
| `sha1rnds4<Arch, Selector>(state, message)` | Four SHA-1 rounds; selector 0–3 chooses the function and constant |
| `sha1nexte<Arch>(state, message)` | Add the derived E value to the high message word |
| `sha1msg1<Arch>(a, b)` | First step for four SHA-1 schedule words |
| `sha1msg2<Arch>(a, b)` | Final step for four SHA-1 schedule words |
| `sha256rnds2<Arch>(cdgh, abef, message)` | Two SHA-256 rounds, returning updated ABEF |
| `sha256msg1<Arch>(a, b)` | First step for four SHA-256 schedule words |
| `sha256msg2<Arch>(a, b)` | Final step for four SHA-256 schedule words |

SHA-1 state is `[D,C,B,A]` in increasing lane order. `sha1rnds4` takes
`[W3,W2,W1,W0+E]`; selectors 0–3 correspond to rounds 0–19, 20–39, 40–59 and
60–79. `sha1nexte` keeps message lanes 0–2 and adds `rotr(state[3],2)` to
lane 3. SHA-1 schedules place the earliest word in the high lane.

SHA-256 uses `cdgh=[H,G,D,C]` and `abef=[F,E,B,A]`. The message holds
`[W0+K0,W1+K1,unused,unused]`. The returned ABEF is `[F,E,B,A]`; the old ABEF
becomes CDGH after two rounds. SHA-256 schedules place the earliest word low.

## Caveats

The four lanes are parts of one hash, not four independent hashes. Message
primitives are partial schedule steps: SHA-1 also needs XOR with intervening
words, and SHA-256 needs an addition between its two primitives. The caller
supplies padding, byte-order conversion, feed-forward and the complete hash.
SHA-1's instruction support does not make it suitable for collision-resistant
applications.

Runtime calls require SHA, SSE2 storage and a `"sha"` caller target. Use
`target_features<native::x86>("sha")` and admit it before entry. AVX and OS AVX
state are unnecessary. The general AVX2 and AVX-512 profiles do not imply SHA;
there are no writemasks.

Feature-bearing overloads are `constexpr` with native runtime paths. Without
SHA, complete 128-bit storage permits `consteval` calls only. Arithmetic wraps
modulo 2³²; `Selector` must be a compile-time unsigned value in 0–3. Inputs
must have the same tag and shape.

See Intel's [SHA extensions description](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sha-extensions.html),
[FIPS 180-4](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
and Clang's [SHA header](https://clang.llvm.org/doxygen/shaintrin_8h_source.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
