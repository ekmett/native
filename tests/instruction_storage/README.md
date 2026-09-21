# Instruction operand storage

These module consumers check register shapes used by instruction extensions: narrow NEON operands, half representations, double pairs and x86 operands below the full arithmetic profiles. They exercise representation copies, alignment, partial loads and stores, empty tails and the associated Boolean mask operations. Importing a raw implementation register preserves its physical bits; logical transfers access only the declared lanes.

The ARM and SSE2 cases execute at the host baseline. Wider x86 storage is also instantiated by its instruction-family tests under the appropriate target. The family code-generation tests separately compare public SIMD calls against direct instruction implementations.

The constexpr consumers check the same lane construction and typed transfers during constant evaluation, including representation-preserving half copies, zero-length tails, scalar integer wrapping, scalar float arithmetic and Boolean masks. The native register representation stays unchanged. Paired assembly checks compare the affected runtime paths with direct register operations under the same target.

Constant evaluation covers signed and unsigned vector arithmetic, modular overflow,
shifts, mask logic, every valid partial count, short-vector padding, binary32
representation bridges, and full-register FP16/BF16 representation-preserving
loads and stores. These checks run through both modules and the private header
where the corresponding interface is available. Runtime bridge assembly is
compared with direct intrinsic code.
