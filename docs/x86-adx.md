# x86 ADX: unsigned addition with carry

[x86 instruction sets](x86.md)

## Why use it

Multiword addition needs two results from each limb: the low sum and a carry
for the next limb. `addcarryx` keeps that pair explicit, so an integer too wide
for one register can still use the processor's carry arithmetic.

## Operations

`import native.x86.adx;` provides `native::addcarryx<Arch>(carry, a, b, out)`
for `std::uint32_t` and `std::uint64_t`. The module belongs to
`native::minimal`; `native.x86` and `native` also export it.

```cpp
std::uint64_t sum;
std::uint8_t next = native::addcarryx<arch>(carry, a, b, &sum);
```

The operands and output object have the same unsigned type. The function
writes the sum modulo 2³² or 2⁶⁴ and returns a `std::uint8_t` containing zero
or one. Any nonzero input carry contributes one. Addends are passed by value,
so the output may overwrite an object from which an addend was read.

## Caveats

The output pointer must identify a writable object. During constant evaluation,
that object must also be modifiable under the usual constant-expression rules.

Runtime calls need `target_features<native::x86>("adx")`, a matching caller
target and CPU admission through `classify_isa`. ADX needs no vector OS state.
The scalar default tag is the module provider's `NATIVE_BASELINE`; a target
attribute on the importer does not change it. An explicit tag avoids that
ambiguity. Feature-bearing overloads are `constexpr`; tags without ADX have
`consteval` overloads only.

Clang's intrinsics can lower to ADD/ADC in an ordinary addition chain. This
API does not expose independent ADCX and ADOX flag chains.

See Clang's [ADX header](https://clang.llvm.org/doxygen/adxintrin_8h_source.html)
and Intel's [Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
