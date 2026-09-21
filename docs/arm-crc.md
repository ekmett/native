# ARM CRC32 and CRC32C

`import native.arm.crc;` provides raw checksum updates for unsigned 8-, 16-,
32- and 64-bit operands. `native.arm` and `native` re-export them. Use `crc32`
for the IEEE polynomial and `crc32c` for the Castagnoli polynomial. Both return
an unsigned 32-bit accumulator and require `arm_feature::crc`.

Each update consumes the operand's bits from least to most significant. The
reflected polynomials are `0xedb88320` and `0x82f63b78` respectively. The functions
apply neither an initial nor a final complement. To obtain the usual checksum
of a byte sequence, initialize the accumulator to `0xffffffff`, update it with
each byte, then complement the result. For `123456789`, that gives CRC32
`0xcbf43926` and CRC32C `0xe3069283`.

```cpp
#include <cstdint>
#include <native/targets.h>
import native.arm.crc;
using namespace native;

#define NATIVE_TARGET_checksum "crc"
NATIVE_TARGET_PUSH(checksum)
std::uint32_t update(std::uint32_t crc, std::uint64_t word) {
  return crc32c<NATIVE_TARGET_ISA(checksum)>(crc, word);
}
NATIVE_TARGET_POP()
```

Before calling `update`, check `NATIVE_TARGET_ISA(checksum)` and any inherited
compiler minimum with `observe_arm_capabilities()` and `classify_isa`. Importing
the module makes the declarations available; the target scope enables their
instructions. A 64-bit operand has the same effect as eight byte updates, lowest
byte first. Loading that operand from memory remains the caller's responsibility,
including byte order and bounds.

The operand width is part of overload selection. Pass one of the unsigned fixed
width types explicitly; floating inputs and mismatched accumulator types are
rejected rather than silently narrowed. CRC itself requires no FP or Advanced SIMD register state. These integer operations do not alter
FPCR or FPSR. The implementation header is `native/arm/crc.h`; granular module
consumers link `native::minimal`, and hub consumers link `native::native`.

Tests compare every width against independent bit-at-a-time polynomial division,
check equivalence to byte updates and standard checksum values, and inspect all
eight instruction forms from a baseline translation unit. Rejection tests cover
missing features, missing compiler targets, and unintended conversions. Runtime
execution is admitted before testing; unavailable hardware is reported as a skip.

Instruction definitions and intrinsic signatures are in the
[Arm C Language Extensions](https://arm-software.github.io/acle/main/acle.html#crc32-intrinsics).
