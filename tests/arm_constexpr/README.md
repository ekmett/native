# ARM scalar constant evaluation

CRC32 and CRC32C are checked at all four widths against normal-polynomial long
division, with a basis for every accumulator and operand bit, dense inputs and
the standard `123456789` checksums. SHA1H is checked against `std::rotr` over every
single-bit word and a known multi-bit word. Each check covers an explicit ISA
with and without the instruction. Module and hub consumers additionally check
omitted-ISA constant calls from a provider built without CRC/SHA. The ordinary
CRC-header fixture is compiled as C++20 and keeps explicit ISA arguments.

Nine actual failed compilations pass runtime parameters to the immediate-only
weak overloads. Two more retain proof that an explicit feature tag cannot replace
the compiler target requirement for runtime code. Existing exact-type guards
remain tested by the family and scalar-default suites.

The scalar-default code-generation suite compares all nine runtime-capable
operations against explicit-ISA and native-helper leaves instruction for
instruction. CRC and crypto family runtime references remain unchanged.

The standalone installed-package suite uses the same granular-module and hub
constant assertions after relocation. It needs no optional hardware feature.
