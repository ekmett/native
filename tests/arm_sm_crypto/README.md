# SM3/SM4 qualification

Build the targets `native_test_arm_sm_crypto_main`, `_constexpr`, `_metadata`,
`_participation`, `_codegen`, `_minimum_0` and `_minimum_1` in an AArch64 provider
configured with `NATIVE_BUILD_TESTS=ON`. Run
`ctest --test-dir <build> -R '^native.arm.sm_crypto' --output-on-failure`.

The suite contains 42 tests. The runtime test admits Clang's complete `sm4`
target through the OS observer before executing any optional instruction. It
returns 77 when admission is unavailable. The remaining checks do not require
SM3/SM4 hardware. The constant-only executable establishes the compile-time
corpus independently of the runtime test's skip status.

The corpus covers nine operations and all four immediate lanes of each TT
operation, for 21 forms. Each form has 96 input triples: 32 walking-bit patterns,
zero and all-one boundaries, and 62 seeded random samples. The independent
reference uses truth-table Boolean functions and a separately represented SM4
substitution permutation. Both strong and feature-absent public calls must match
it during constant evaluation; admitted hardware must match the same corpus.
Complete SM3("abc") hashing and SM4 key expansion, encryption and decryption
also check published known answers through the public operations.

The assembly test compares each public leaf with its ACLE intrinsic at the
same vector ABI and target. All 21 pairs must match exactly. Negative compile
tests cover runtime use of every weak overload, every target boundary, every
wrong shape, and both invalid ends of all four TT immediate ranges. Metadata
checks exercise independent present/observed bits, the coupled compiler target,
property accessors, stable enum indices, and translation units compiled with and
without `+sm4`. Those last translation units contain only constant assertions
and an empty main function; they execute no optional instruction.

For an installed or relocated package, configure this directory as the project
with `CMAKE_PREFIX_PATH` pointing at the package and the same C++26 compiler.
It builds the direct-module, ARM-hub and host-hub consumers plus the constant-only
corpus. No raw intrinsic header is part of the consumer API.

The local macOS qualification reports unknown SM3/SM4 sysctl keys and therefore
skips hardware execution. That result is retained as an execution gap; native
hardware behavior must be qualified on an admitted AArch64 host.
