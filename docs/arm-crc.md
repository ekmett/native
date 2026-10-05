# ARM CRC: CRC32 and CRC32C updates

[ARM instruction sets](arm.md)

## Why use it

A checksum loop can consume a whole integer word per update instead of looking
up each byte in a polynomial table. These instructions provide the raw CRC
recurrence for the IEEE and Castagnoli polynomials, so you can choose the input
chunk size without changing the checksum.

## Operations

Import `native.arm.crc`, or use the `native.arm` or `native` hub.
`crc32<Arch>(crc, value)` uses the IEEE polynomial;
`crc32c<Arch>(crc, value)` uses Castagnoli. Both take and return `std::uint32_t`
accumulators. The input must be exactly `std::uint8_t`, `std::uint16_t`,
`std::uint32_t` or `std::uint64_t`.

Each update consumes the operand's bits from least to most significant. The
reflected polynomials are `0xedb88320` and `0x82f63b78`. A 64-bit update is
equivalent to eight byte updates, lowest byte first.

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.crc;

#define NATIVE_TARGET_checksum "crc"
NATIVE_TARGET_PUSH(checksum)
std::uint32_t update(std::uint32_t crc, std::uint64_t word) {
  return native::crc32c<NATIVE_TARGET_ISA(checksum)>(crc, word);
}
NATIVE_TARGET_POP()

static_assert(native::crc32<native::isa<native::arm>{}>(
  std::uint32_t{0}, std::uint8_t{1}) == 0x77073096);
```

## Caveats

The functions apply neither an initial nor a final complement. For the usual
checksum of a byte sequence, start at `0xffffffff`, consume the bytes, then
complement the result. The bytes of `123456789` give CRC32 `0xcbf43926` and
CRC32C `0xe3069283` with that convention. Word loads remain your responsibility,
including bounds and byte order.

Runtime calls require an ARM `Arch` containing `arm_feature::crc` and a
`"crc"` caller target. Before entering the target function, check
`NATIVE_TARGET_ISA(checksum)` and `NATIVE_TARGET_MINIMUM` with
`observe_arm_capabilities()` and `classify_isa`. CRC needs no Advanced SIMD
register state and does not change FPCR or FPSR.

Imported scalar calls may omit `Arch`. That default is captured from
`NATIVE_BASELINE` when the module is compiled; a target scope in the importer
does not change it. Use an explicit ISA for an optional target function.
Standalone `native/arm/crc.h` calls require an explicit ISA.

All widths support constant evaluation. A CRC-capable tag provides `constexpr`
calls; a tag without CRC provides only `consteval` calls and no runtime
fallback. Exact unsigned operand types are required in either case.

See the [Arm C Language Extensions](https://arm-software.github.io/acle/main/acle.html#crc32-intrinsics).
