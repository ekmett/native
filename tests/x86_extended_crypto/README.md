# X86 extended crypto fixture

Configure this directory standalone, or run the root
`native.x86.extended_crypto` tests. It covers:

- SHA-512(`abc`) and SM3(`abc`), with every expanded message word and compression
  round produced through the public instructions.
- The standard SM4 key-schedule/encryption example in both vector widths.
- Strong and feature-absent constant evaluation for those complete computations.
- Independent CPUID bits, subleaf availability, XCR0 admission, compiler target
  prerequisites and provider-baseline macro snapshots, including AVX-NE-CONVERT.
- Feature, caller-target, raw-register, shape and immediate rejection controls.
- Thirteen raw-intrinsic/public codegen pairs, including SM3 immediate 255.

LLVM 23.1.1 x86_64/macOS qualification passes constant evaluation, metadata,
codegen and the negative controls. The three native-instruction runtime tests
skip under Rosetta. No SHA512/SM3/SM4-capable execution host has been qualified.
The runtime executable retains the actual instruction leaves and admits each
family independently. These results establish no timing claim.

The source-only SM4 substitution permutation is shared with the ARM SM4 module;
it does not make one architecture's instruction module depend on the other.
