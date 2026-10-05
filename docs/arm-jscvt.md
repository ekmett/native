# ARM JSCVT: JavaScript integer conversion

[ARM instruction sets](arm.md)

## Why use it

JavaScript's conversion to a signed word is defined even when the input is a
NaN, infinity or far outside the integer range. JSCVT implements that conversion
in one instruction. It is useful whenever the desired result is the low 32 bits
of a truncated binary64 value.

## Operations

Import `native.arm.jscvt`, or use the `native.arm` or `native` hub.
`native::jcvt<Arch>(double)` returns `std::int32_t`: truncate toward zero,
reduce modulo 2³², then interpret the word as signed. NaNs, infinities and
signed zeros produce zero.

```cpp
#include <cstdint>
import native.arm.jscvt;

constexpr auto requirement = native::target_features<native::arm>("jsconv");

__attribute__((target("jsconv")))
std::int32_t integer_word(double x) {
  return native::jcvt<requirement>(x);
}

static_assert(native::jcvt<native::isa<native::arm>{}>(4294967297.0) == 1);
```

## Caveats

An out-of-range C++ floating-to-integer cast does not have this contract.
Constant evaluation decodes binary64 bits rather than making such a cast.
Runtime calls execute FJCVTZS with its floating-point environment behavior;
constant evaluation computes the integer result without changing FP status.

Runtime calls require `arm_feature::jsconv` and a `"jsconv"` caller target.
The module uses the extension's JSCVT spelling; the feature and target use
Clang's spelling. Check runtime admission before entering the target function.
Importing a module does not check the CPU or enable instructions.

Omitting `Arch` uses the baseline captured when the module was compiled. An
importer's target attribute does not change that default. A tag without JSCVT
permits only `consteval` calls; it supplies no runtime fallback.

See the [Arm ACLE conversion contract](https://arm-software.github.io/acle/main/acle.html#floating-point-data-processing-intrinsics).
