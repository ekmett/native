# JavaScript integer conversion

`import native.arm.jscvt;` exposes `native::jcvt<Arch>(double)` on AArch64.
The result is a `std::int32_t`: truncate toward zero, then reduce the integer
modulo 2³² and interpret that word as signed. NaNs, infinities and signed zeros
produce zero. This differs from a C++ floating-to-integer cast for out-of-range
inputs.

The operation requires `arm_feature::jsconv` and Clang's `"jsconv"` function
target. The module name follows the architecture's JSCVT extension spelling;
the feature and target use Clang's spelling. Import through `native.arm` or
`native` for the complete architecture hub, or link `native::minimal` when using
the scalar module alone.

```cpp
#include <cstdint>
import native.arm.jscvt;

constexpr auto requirement = native::target_features("jsconv");

__attribute__((target("jsconv")))
std::int32_t integer_word(double x) {
  return native::jcvt<requirement>(x);
}

static_assert(native::jcvt<native::isa<native::arm>{}>(4294967297.0) == 1);
```

As with other scalar instruction APIs, omitted `Arch` uses the baseline captured
when the module was compiled. A weaker tag accepts constant inputs through a
`consteval` overload and rejects runtime inputs. Runtime calls require admission
before entering the target function; importing the module does not check the CPU.

Constant evaluation decodes binary64 bits and never makes an out-of-range C++
cast. Runtime calls lower directly to FJCVTZS and retain the instruction's
floating-point environment behavior. Constant evaluation computes the numerical
result without changing FP status. See the [Arm ACLE conversion
contract](https://arm-software.github.io/acle/main/acle.html#floating-point-data-processing-intrinsics).

[The qualification fixture](../tests/arm_jscvt/README.md) checks special values,
modular wrap boundaries, 2,048 constant results against hardware and an independent
arithmetic reference, and 100,000 seeded binary64 inputs. It also checks exact
raw/public instructions, caller-target rejection and weak-tag runtime rejection.
