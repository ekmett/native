# Conversion permission checks

F16C reuses the independent integer decoding/rounding oracle. Runtime inputs
exercise all MXCSR rounding and DAZ/FTZ combinations, fixed and current-rounding
immediates, every binary16 widening representation, and scalar or split storage.
Compile-time anchors retain gradual inputs and default nearest-even rounding.
Fallback checks compare values and controls; floating exception flags are
unspecified for permitted emulation and traps remain masked.

AVXNECONVERT reuses the maintained independent conversion corpus with scalar and
split storage. Its fixed conversion policy must ignore MXCSR controls and leave
status unchanged. The naturally aligned memory operands include vector-unaligned
addresses and full documented source extents.

Three disassembly comparisons retain the native conversion bodies after adding
permission, including discarded-result F16C status effects. A separate F16C probe
uses an instruction-only ISA plus permission, so missing native storage still
selects native conversions. Load/store differences between storage layouts are
allowed in that probe; native operation counts and absence of helper calls are
checked. Existing feature-absent runtime negative tests retain admission checks.
