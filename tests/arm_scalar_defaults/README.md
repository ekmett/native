# ARM scalar module defaults

The fixtures exercise all fourteen scalar overloads whose ISA cannot be deduced
from a vector: eight CRC updates, four RDM operations, SHA1H and scalar-input
PMULL. Both module baselines check constraints, exact scalar types, explicit ISA
arguments, vector deduction and unchanged lane ordering. A present-baseline
runtime check admits the compiler's feature set before comparing CRC, SHA1H and
PMULL results with references and checking RDM saturation and FPSR.QC.

The positive provider uses `armv8.1-a+nolse+crc+aes+sha2+rdm`: Clang 23 does not
report `__ARM_FEATURE_QRDMX` for `armv8-a+rdm` alone. The negative provider disables
all four instruction families. Standalone CRC headers keep explicit ISA
arguments in both configurations. The installed package runs the same public
module assertions and runtime fixture.

Every default call has an explicit-ISA and a native-helper assembly counterpart.
All fourteen instruction sequences must match exactly, with no memory accesses,
stack use or calls. Eight failed-compilation checks cover missing baseline
features and forbidden scalar conversions.

The provider check reuses the exact BMIs and response files produced by CMake.
A changed importer must either retain the provider's default ISA or fail with the
compiler's BMI configuration diagnostic. It never disables Clang's compatibility
checks. Target-attributed scopes in the ordinary consumer also assert that they
do not change the imported defaults.

CRC and SHA1H now have immediate-only overloads for a baseline without the
instruction. Their availability expressions intentionally see those overloads;
negative fixtures pass runtime parameters to prove there is no runtime fallback.
RDM and PMULL still require their feature even for constant arguments.
