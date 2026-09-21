# x86 CRC32C

Import `native.x86.crc32c`, `native.x86`, or `native` for
`native::crc32c<Arch>(accumulator, value)`. Source-tree header consumers can
include `<native/x86/crc32c.h>`; the installed public API uses named modules.
The granular module belongs to `native::minimal` and includes the implementation
header in its global module fragment. Vendor intrinsic headers remain outside
the `native` namespace. Importing the API leaves the caller's target unchanged.

The accumulator and return type are always `std::uint32_t`:

| Operand type | Bits consumed | Execution mode |
| --- | --- | --- |
| `std::uint8_t` | 8 | x86 |
| `std::uint16_t` | 16 | x86 |
| `std::uint32_t` | 32 | x86 |
| `std::uint64_t` | 64 | x86-64 only |

The package currently supports x86-64 builds. The 64-bit operand overload also
has an explicit x86-64 declaration guard.

Each call performs one raw CRC32C update using the Castagnoli polynomial
`0x1edc6f41`, whose reflected representation is `0x82f63b78`. It consumes the
numeric operand from least significant bit to most significant bit. Consequently,
a wider update equals successive byte updates from least significant byte to
most significant byte: `std::uint16_t{0x3231}` consumes `0x31`, then `0x32`.
The API takes values and defines no buffer loading or byte-order conversion.
Callers choose how bytes become numeric operands and which memory to access.

No initial or final complement is implicit. All 32 seed bits participate.
The 64-bit instruction zeroes the upper half of its destination; its wrapper
accepts a 32-bit seed and returns the 32-bit remainder. These semantics and the
CPUID requirement follow the CRC32 entry in
[Intel's instruction reference](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf).

Every overload is side-effect-free, `noexcept`, always inline, constrained by
`Arch.has(native::x86_feature::crc32)`, and targeted to `"crc32"`.
The feature maps to CPUID leaf 1 ECX bit 20, with no POPCNT, SIMD or OS vector
state prerequisite. The existing `sse42` feature represents a broader compiler
bundle; its closure includes `crc32`. The independent feature matches
[Clang's CRC intrinsic target](https://clang.llvm.org/doxygen/crc32intrin_8h.html)
and [LLVM's separate CRC32 feature](https://reviews.llvm.org/D105462).

Use a matching target scope and admit execution before calling. This example
explicitly supplies the conventional initial seed and final complement:

```cpp
#include <cstdint>
#include <native/targets.h>
import native.x86.crc32c;

#define NATIVE_TARGET_checksum "crc32"
constexpr native::isa checksum_isa{native::x86_feature::crc32};

NATIVE_TARGET_PUSH(checksum)
std::uint32_t checksum_example() noexcept {
  constexpr std::uint8_t bytes[]{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::uint32_t crc = 0xffffffffu;
  for (auto byte : bytes)
    crc = native::crc32c<checksum_isa>(crc, byte);
  return crc ^ 0xffffffffu;
}
NATIVE_TARGET_POP()

int main() {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, checksum_isa, NATIVE_TARGET_MINIMUM).admitted())
    return 0;
  return checksum_example() == 0xe3069283u ? 0 : 1;
}
```

`tests/x86_crc32c` covers the known `"123456789"` vector, an independent bitwise
polynomial oracle, directed seeds and operands, randomized updates at every
width, and agreement between wider operands and ordered byte updates.
Consumption checks cover the header, granular module, x86 module and main
module, including a relocated installed package. Negative compilation checks
exercise missing feature requirements and missing caller targets. Codegen
checks compile baseline callers with optional features disabled, check each
CRC operand width, and check that ordinary baseline functions remain free of
CRC instructions. Assembly checks establish instruction selection only.
Runtime checks admit optional instructions before execution and report skips
when unavailable; compilation alone does not establish execution coverage.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
