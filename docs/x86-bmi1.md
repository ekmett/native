# BMI1 operations

`import native.x86.bmi1;` exposes `native::andn`, `bextr`, `blsi`, `blsmsk`,
`blsr`, and `tzcnt`. They are also re-exported by `native.x86` and `native` on
x86-64. Source-tree header consumers can include `<native/x86/bmi1.h>`; the
installed public API uses the named modules.

The template argument `Arch` has type `native::isa<native::x86>`. Runtime
overloads require only `Arch.has(native::x86_feature::bmi1)`. Calls are `noexcept`,
side-effect-free, and always inline, with compiler target `"bmi"`. Call from a
matching target scope after CPU admission:

```cpp
constexpr native::isa<native::x86> bits{native::x86_feature::bmi1};

__attribute__((target("bmi")))
std::uint32_t clear_lowest(std::uint32_t value) {
  return native::blsr<bits>(value);
}

// In baseline code:
auto cpu = native::observe_x86_capabilities();
if (native::classify_isa(cpu, bits).admitted()) {
  auto result = clear_lowest(0x18u); // 0x10
}
```

Imported scalar operations default `Arch` to `NATIVE_BASELINE` as captured when
their owning module is compiled. A function target attribute on the caller
does not change that captured value.
Explicit `Arch` arguments are supported, and standalone headers require them.

`Arch` has type `native::isa<native::x86>`; ARM and Wasm tags are rejected.
All operand widths support constant evaluation. If `Arch` lacks the feature,
the selected overload is `consteval`: a constant call is accepted, while a call
with runtime inputs is ill-formed. With the feature present, the overload is
`constexpr` and uses the instruction implementation at runtime. Runtime calls
still require a matching compiler target and admitted CPU support; there is
no runtime software fallback.

The five bit operations have `std::uint32_t` and `std::uint64_t` overloads.
`tzcnt` additionally supports `std::uint16_t`. Each returns the operand type.

| Operation | Result |
| --- | --- |
| `andn<Arch>(a, b)` | `(~a) & b` |
| `bextr<Arch>(value, control)` | Extract bits using start in bits 7:0 and length in bits 15:8 of `std::uint32_t control`. |
| `bextr<Arch>(value, unsigned start, unsigned length)` | Extract using the low eight bits of each control. |
| `blsi<Arch>(value)` | Only the lowest set bit; zero for a zero input. |
| `blsmsk<Arch>(value)` | Ones through the lowest set bit, inclusive; all ones for zero. |
| `blsr<Arch>(value)` | Clear the lowest set bit; zero remains zero. |
| `tzcnt<Arch>(value)` | Count trailing zero bits; zero returns 16, 32, or 64. |

`bextr` clips extraction at the operand width and clears the remaining result
bits. Zero length or a start at or beyond the width returns zero. Control bits
above bit 15 have no effect. The wrappers expose values, not arithmetic flags.
Semantics follow Intel's instruction entries in [SDM Volume 2A, ANDN through
BLSR](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf)
and [SDM Volume 2B, TZCNT](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf).

BMI1 is CPUID leaf 7, subleaf 0, EBX bit 3. Intel lists it independently of
BMI2 and AVX in [SDM Volume 1, section 5.1.16.1](https://cdrdv2-public.intel.com/819711/253665-sdm-vol-1.pdf).
These general-purpose instructions require no AVX register state admission.
The BMI1 feature check is retained for `tzcnt`: on older CPUs its encoding can
execute as BSF, whose zero-input result is undefined.
