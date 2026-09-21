# Instruction operand storage

These module consumers check register shapes used by instruction extensions: narrow NEON operands, half representations, double pairs and x86 operands below the full arithmetic profiles. They exercise representation copies, alignment, partial loads and stores, empty tails and the associated Boolean mask operations. Importing a raw implementation register preserves its physical bits; logical transfers access only the declared lanes.

The ARM and SSE2 cases execute at the host baseline. Wider x86 storage is also instantiated by its instruction-family tests under the appropriate target. The family code-generation tests separately compare public SIMD calls against direct instruction implementations.
