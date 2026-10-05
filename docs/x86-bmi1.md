# x86 BMI1: bit extraction and lowest-bit operations

[x86 instruction sets](x86.md)

## Why use it

A bitset often needs one set bit at a time. `blsi` isolates it, `blsr` removes
it, and `tzcnt` gives its position. BMI1 also extracts a field without a
separate shift and mask. These are useful building blocks for sparse masks,
packed records and bitset iteration.

## Operations

`import native.x86.bmi1;` provides the following operations in `native`.
`native.x86` and `native` re-export the module. Operands and results are
`std::uint32_t` or `std::uint64_t`; `tzcnt` also accepts `std::uint16_t`.

| Operation | Result |
| --- | --- |
| `andn<Arch>(a, b)` | `(~a) & b` |
| `bextr<Arch>(value, control)` | Extract a field; start is control bits 7:0, length is bits 15:8 |
| `bextr<Arch>(value, start, length)` | Extract using the low eight bits of each unsigned control |
| `blsi<Arch>(value)` | Isolate the lowest set bit; zero stays zero |
| `blsmsk<Arch>(value)` | Set bits through the lowest set bit, inclusive; zero gives all ones |
| `blsr<Arch>(value)` | Clear the lowest set bit; zero stays zero |
| `tzcnt<Arch>(value)` | Count trailing zeros; zero gives the operand width |

`bextr` clips the field at the operand width and clears the remaining result
bits. Zero length or a start at or beyond the width gives zero. Control bits
above bit 15 are ignored. All functions are `noexcept`.

## Caveats

Use `target_features<native::x86>("bmi")`, a matching compiler target and
`classify_isa` before entry. BMI1 is independent of BMI2 and AVX and needs no
vector OS state. The feature check matters even for `tzcnt`: its encoding can
execute as BSF on older CPUs, with different zero-input behavior.

`Arch` is an `isa<x86>`. The module's scalar default is the provider's
`NATIVE_BASELINE`; caller target attributes do not change it. Feature-bearing
calls are `constexpr` with native runtime paths. Tags without BMI1 accept
constant evaluation through `consteval` overloads, with no runtime fallback.
The API exposes values, not arithmetic flags.

See Intel's [BMI1 instruction entries](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf)
and [TZCNT entry](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf).
