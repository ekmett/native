# Explicit operation polyfills

Issue: https://github.com/ekmett/native/issues/70

The shared `polyfill` flag permits runtime semantic fallbacks without adding
hardware features to an ISA value. Native instruction overloads keep priority.
Feature-absent calls without permission remain immediate-only or rejected.

## First checkpoint plan

- [x] Import the ISA permission checkpoint from the storage worker; use
  `Arch.has(polyfill)` and retain family-typed instruction interfaces.
- [x] Exercise ARM SM3/SM4 with `neon | polyfill` and scalar `polyfill` storage
  through the maintained instruction corpus and full known-answer algorithms.
  Enable the existing semantic bodies only where the instruction is absent.
- [x] Add scalar ARM CRC32/CRC32C fallbacks for all four operand widths, preserving
  exact operand admission, byte order, native priority and immediate-only calls.
- [x] Run focused CMake/CTest checks, native code equivalence and representative
  no-permission rejections; commit the operation checkpoint.

## Remaining operation inventory

This inventory is deliberately not a blanket permission change. Integer bodies
are candidates only after their width, lane dependencies and native route are
checked. Storage decomposition is implemented separately.

| Families | Existing semantics / next review |
| --- | --- |
| ARM AES, PMULL, SHA-1/256/512, SHA-3 | Integer semantic helpers; fixed instruction shapes and dependent schedules must stay logical-lane correct |
| ARM DOTPROD, I8MM, RDM and NEON integer operations | Integer helpers; widened groups, saturation and immediate constraints need review |
| x86 AES/VAES, PCLMUL/VPCLMUL, GFNI, SHA, SHA-512, SM3/SM4 | Integer helpers; 128-bit subblocks and cross-lane algorithms differ by family |
| x86 BMI1/BMI2, POPCNT, LZCNT, CRC32C, ADX | Scalar semantics; distinguish defined zero and carry behavior from native-only environmental effects |
| x86 IFMA/VNNI, bit algorithms, VBMI/VBMI2 and conflict operations | Integer helpers; preserve group widths, mask rules and logical compaction across register splits |
| Floating arithmetic, FP16/BF16, FCMA, FP16FML, F16C/JSCVT | Existing constant evaluation is not automatically a runtime FP contract; rounding, signed zero, NaNs, denormals and fused operations need review |
| Memory, waits and environmental instructions | No general numerical polyfill promise; fault, ordering, waiting and environment contracts need explicit decisions |
| WebAssembly relaxed operations | Existing deterministic constant semantics differ from allowed relaxed runtime behavior; review separately |

Permission does not establish constant-time cipher behavior. Substitution-table
fallbacks preserve values but do not promise data-independent timing.

## Verification checkpoints

Clang 23.1.1/CMake 4.4, macOS ARM64, Release:

- Baseline runtime SM3/SM4 and CRC polyfill tests fail with immediate-function
  diagnostics before adding the permission overloads.
- Final focused run after storage checkpoint `830fad2`: 20 passed, one native
  SM3/SM4 runtime check skipped for unavailable host admission. Both NEON-storage
  and scalar-storage SM3/SM4 polyfills execute the complete instruction corpus
  and known answers, plus runtime-seeded samples with normal inlining. Scalar
  CRC executes all operand widths, standard check values and byte-order laws.
- Native SM3/SM4 with permission matches all 21 intrinsic leaves. Existing
  representative no-permission and target rejections pass. Permission retains
  exact scalar types, vector shapes and immediate bounds.

The focused CTest command selected
`native.arm.sm_crypto.(main|metadata|participation|constexpr|polyfill|scalar_polyfill|polyfill_codegen|codegen|reject_0_0|reject_1_0|reject_6_0|reject_7_0|reject_8_0)`
and `native.arm.crc.(polyfill|module|header|codegen|reject_[0-3])` with exact-name
anchors, serial execution and `--output-on-failure`.
