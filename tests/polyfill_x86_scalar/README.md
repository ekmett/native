# Scalar x86 polyfills

The baseline module consumer exercises runtime and constant evaluation for
BMI1, BMI2, POPCNT, LZCNT, CRC32C and ADX with `isa<x86>(polyfill)`.
PEXT/PDEP round trips, independent bit-serial multiplication and CRC references,
zero counts, masked shift controls and noncanonical carry bytes cover the shared
semantic implementations. No optional instruction feature is enabled on this
consumer.

Eight disassembly pairs compare hardware tags against the same hardware tags
with `polyfill`; native instruction selection must remain identical. Existing
scalar-default and ADX/CRC rejection fixtures retain missing-permission and
immediate-range checks.
