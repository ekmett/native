# x86 BMI2: bit packing and wide products

[x86 instruction sets](x86.md)

## Why use it

`pext` gathers a sparse set of bits into a dense index; `pdep` scatters those
bits back into a chosen layout. That lets a mask describe the packing instead
of a chain of shifts. `mulx` supplies both halves of a product for multiword
arithmetic, while the shift operations give defined masked-count behavior.

## Operations

`import native.x86.bmi2;` exports the operations below through `native`.
The `native.x86` and `native` hubs also export them. Here `U` is
`std::uint32_t` or `std::uint64_t`.

| Operation | Result |
| --- | --- |
| `bzhi<Arch>(U value, unsigned index)` | Clear bits at and above `index & 255`; retain all bits when that index reaches the width |
| `mulx<Arch>(U a, U b, U* high)` | Return the low product and write its high half |
| `pdep<Arch>(U value, U mask)` | Deposit consecutive low bits into positions selected by `mask` |
| `pext<Arch>(U value, U mask)` | Pack bits selected by `mask` into consecutive low positions |
| `shlx<Arch>(U value, unsigned count)` | Logical left shift, count modulo the width |
| `shrx<Arch>(U value, unsigned count)` | Logical right shift, count modulo the width |
| `sarx<Arch>(value, unsigned count)` | Signed right shift on `int32_t` or `int64_t`, count modulo the width |
| `rorx<Arch, Imm8>(U value)` | Right rotation by a compile-time byte, count modulo the width |

`rorx<Imm8>(value)` uses the module default tag; `rorx<Imm8, Arch>(value)`
also accepts an explicit tag. `Imm8` is in 0–255. All functions are `noexcept`.

## Caveats

The `mulx` output must point to a writable object; constant evaluation also
requires that object to be modifiable in the constant expression. These
wrappers specify results and the output write, with no C++ flag-preservation
or exact-instruction-encoding promise.

Runtime calls need BMI2, a `"bmi2"` caller target and CPU admission. BMI1 and
vector OS state are unnecessary. `Arch` is an `isa<x86>`; its scalar default
is the provider's `NATIVE_BASELINE`, unaffected by importer target attributes.
Feature-bearing overloads are `constexpr`; tags without BMI2 are
`consteval`-only.

`pdep` and `pext` costs vary substantially by CPU. A compact expression is
not a throughput guarantee; measure them in the loop that needs the packing.

See Intel's [BMI2 instruction reference](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).
