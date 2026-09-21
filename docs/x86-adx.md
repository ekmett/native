# Unsigned addition with carry

`import native.x86.adx;` exports `addcarryx` for 32-bit and 64-bit unsigned
integers. It belongs to `native::minimal` and is also available through the
`native.x86` and `native` hubs when linking `native::native`.

```cpp
std::uint64_t sum;
std::uint8_t next = native::addcarryx<arch>(carry, a, b, &sum);
```

`a`, `b` and the pointed-to result share `std::uint32_t` or `std::uint64_t`.
`carry` is a `std::uint8_t`; any nonzero value contributes one. The function
writes the sum modulo 2³² or 2⁶⁴ and returns overflow as a `std::uint8_t`
containing zero or one. The output pointer must identify a valid writable
object. Addends are passed by value, so the output may overwrite storage
from which an addend was read. No SIMD register or mask appears in the API.

Use `target_features<native::x86>("adx")` for `arch` and enable `"adx"` on the
caller. Admit the target before entry. ADX is independently observed at CPUID
leaf 7, subleaf 0, EBX bit 19 and requires no SIMD storage or OS vector state.
The module's default `Arch` is the provider's `NATIVE_BASELINE`, matching the
other scalar instruction families; importer macros do not change that default.
An explicit tag remains available for separately targeted functions.

Strong tags use `constexpr` overloads with native intrinsic paths. Tags missing
ADX use `consteval` overloads, including default calls when the provider's
baseline lacks ADX. Constant evaluation implements exact unsigned arithmetic,
including normalization of all 255 nonzero carry-byte values. An output object
modified during constant evaluation must satisfy the usual C++ constant
expression rules. There is no weak-tag runtime fallback.

Clang 23 lowers its `_addcarryx_u32` and `_addcarryx_u64` intrinsics to ADD/ADC
in the fixture's isolated additions and four-limb chains. The public operations
produce exactly the same instruction streams as those intrinsics. This API
does not expose independently scheduled ADCX and ADOX flag chains, and the
current qualification does not claim those dedicated opcodes. That remains a
compiler/API limitation; adding assembly or extra flag setup would change the
raw-intrinsic contract.

The [fixture](../tests/x86_adx/README.md) documents full-adder references,
carry chains, default architecture behavior and assembly comparisons. Carry
semantics follow Clang's [ADX header](https://clang.llvm.org/doxygen/adxintrin_8h_source.html)
and Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
