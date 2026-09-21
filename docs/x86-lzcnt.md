# x86 LZCNT

Import `native.x86.lzcnt`, `native.x86`, or `native` for
`native::lzcnt<Arch>(value)`. Source-tree header consumers can include
`<native/x86/lzcnt.h>`; the installed public API uses the named modules.
The operation accepts `std::uint16_t`,
`std::uint32_t`, or `std::uint64_t` and returns the same unsigned type.

LZCNT counts zero bits before the most significant set bit. A zero input returns
its width: 16, 32 or 64. An input whose most significant bit is set returns zero.
Intel defines all three operand widths. The required feature is LZCNT,
reported by extended CPUID leaf 0x80000001 ECX bit 5; it requires no BMI or
vector OS state. On a CPU without LZCNT, the same instruction encoding executes
BSR with different semantics. See the LZCNT entry in
[Intel's instruction reference, Volume 2A](https://cdrdv2-public.intel.com/922480/253666-092-sdm-vol-2a.pdf#page=696)
and the [current Intel manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).

Each overload is `noexcept`, always inline, targeted to `"lzcnt"`, and constrained
by `Arch.has(native::x86_feature::lzcnt)`. The caller supplies both a matching
target scope and runtime admission:

```cpp
#include <cstdint>
#include <native/targets.h>
import native.x86.lzcnt;

#define NATIVE_TARGET_leading_zeroes "lzcnt"
constexpr auto leading_isa = NATIVE_TARGET_ISA(leading_zeroes);

NATIVE_TARGET_PUSH(leading_zeroes)
std::uint16_t leading_zeroes(std::uint16_t value) noexcept {
  return native::lzcnt<leading_isa>(value);
}
NATIVE_TARGET_POP()

int main() {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, leading_isa, NATIVE_TARGET_MINIMUM).admitted())
    return 0;
  return leading_zeroes(0) == 16 ? 0 : 1;
}
```

The wrappers preserve the zero-input behavior at every width and expose the
count, not the instruction's flags. Constant folding and instruction selection
remain compiler decisions; the API does not promise an exact encoding.

`tests/x86_lzcnt` checks the header, direct module and main import, feature
participation and admission, zero/all-one/single-bit cases, and randomized
results against `std::countl_zero`. Its codegen test checks all three widths
with optional ISA features disabled outside the target scopes.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
