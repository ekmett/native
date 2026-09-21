# x86 POPCNT

`native::popcnt<Arch>(value)` counts the set bits of an unsigned integer.
Zero returns zero; an all-one input returns its width. The overloads accept
`std::uint16_t`, `std::uint32_t`, or `std::uint64_t` and return the same type.

Import `native.x86.popcnt`, `native.x86`, or `native` to use them. Source-tree
header consumers can include `<native/x86/popcnt.h>`; the installed public API
uses the named modules.

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

Intel defines 16-, 32- and 64-bit forms, admitted by CPUID leaf 1 ECX bit 23.
SSE, BMI and vector OS state are not prerequisites.
See the POPCNT entry in [Intel's instruction reference, Volume 2B](https://cdrdv2-public.intel.com/782151/253667-sdm-vol-2b.pdf#page=401)
and the [current Intel manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).

Each runtime overload is `noexcept` and always inline, with target `"popcnt"` and the
constraint `Arch.has(native::x86_feature::popcnt)`. Supplying the feature in
`Arch` does not change the caller's compiler target or check the CPU. The
caller still needs a matching target scope and a runtime capability check:

```cpp
#include <cstdint>
#include <native/targets.h>
import native.x86.popcnt;

#define NATIVE_TARGET_count_bits "popcnt"
constexpr auto count_isa = NATIVE_TARGET_ISA(count_bits);

NATIVE_TARGET_PUSH(count_bits)
std::uint64_t count_bits(std::uint64_t value) noexcept {
  return native::popcnt<count_isa>(value);
}
NATIVE_TARGET_POP()

int main() {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, count_isa, NATIVE_TARGET_MINIMUM).admitted())
    return 0;
  return count_bits(0xf0f0) == 8 ? 0 : 1;
}
```

The 16-bit overload counts a zero-extended operand through Clang's 32-bit
intrinsic. Its value contract is 16-bit; optimization may choose a different
instruction width. None of these wrappers exposes the instruction's flags.
The existing generic scalar/vector `popcount` operations keep their own API;
`popcnt` is the explicit x86 feature-gated spelling.

`tests/x86_popcnt` checks the header, direct module and main import, along with
feature constraints and runtime admission. It compares zero, all-one,
single-bit and random inputs with `std::popcount`. Assembly checks cover all
three widths with optional ISA features disabled outside the target scopes.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
