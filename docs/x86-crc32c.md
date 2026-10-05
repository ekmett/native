# x86 CRC32C: Castagnoli checksum updates

[x86 instruction sets](x86.md)

## Why use it

CRC32C detects accidental corruption in buffers and records. The instruction
updates a running remainder with up to eight bytes at once, avoiding a lookup
table for a short checksum loop. It is a checksum primitive, with no
cryptographic authentication property.

## Operations

`import native.x86.crc32c;` provides
`native::crc32c<Arch>(accumulator, value)`. It belongs to `native::minimal`
and is re-exported by `native.x86` and `native`.

The accumulator and result are `std::uint32_t`. The value can be
`std::uint8_t`, `std::uint16_t`, `std::uint32_t` or, on x86-64,
`std::uint64_t`. An update consumes the numeric operand from its least
significant byte to its most significant byte. Thus `uint16_t{0x3231}`
consumes `0x31`, then `0x32`.

```cpp
std::uint32_t crc = 0xffffffffu;
for (auto byte : bytes)
  crc = native::crc32c<arch>(crc, byte);
crc ^= 0xffffffffu;
```

The conventional initial seed and final complement are explicit here.
The operation itself applies neither. All 32 seed bits participate.

## Caveats

CRC32C uses the Castagnoli polynomial `0x1edc6f41`, reflected as `0x82f63b78`.
It differs from the CRC-32 polynomial used by ZIP and Ethernet. Wider updates
follow numeric byte order, so the caller chooses buffer loads and byte-order
conversion.

Runtime calls require `x86_feature::crc32`, a `"crc32"` compiler target and
CPU admission. This scalar instruction needs no SIMD or vector OS state.
The broader `sse42` compiler bundle includes it. Calls are `noexcept`.
The module default `Arch` is the provider's `NATIVE_BASELINE`; caller target
attributes do not change it. With the feature, overloads are `constexpr` and
use native runtime instructions. Without it, only `consteval` calls exist.

See Intel's [CRC32 instruction reference](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf)
and Clang's [CRC intrinsics](https://clang.llvm.org/doxygen/crc32intrin_8h.html).

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
