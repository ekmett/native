# Integer x86 polyfills

This fixture compiles the maintained public instruction disassembly corpus with
hardware tags and with the same hardware tags plus `polyfill`. The comparison
covers every integer operation family and checks that permission does not alter
native instruction selection.

Runtime variants in the original family directories reuse their independent
oracles with scalar and SSE2 split storage. They cover all integer instruction
families, masks, instruction block boundaries, whole-vector permutation and
compaction, and packed memory access suppression. VBMI2 additionally exercises
byte-unaligned word loads/stores and null pointers under empty masks. Focused
constexpr anchors supplement the existing exhaustive native constexpr corpus.
