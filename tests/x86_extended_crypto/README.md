# X86 extended crypto fixture

Configure this directory against an installed package, or run the root
`native.x86.extended_crypto` tests:

```sh
cmake -S tests/x86_extended_crypto -B build-crypto-consumer -G Ninja \
  -DCMAKE_PREFIX_PATH=/path/to/relocated/native -DCMAKE_CXX_COMPILER=clang++
cmake --build build-crypto-consumer
ctest --test-dir build-crypto-consumer --output-on-failure
```

To build the provider directly from this tree, configure with
`-DNATIVE_TEST_SOURCE_PROVIDER=ON`. When cross-compiling on Apple hosts, set an
explicit `CMAKE_CXX_COMPILER_TARGET` as well as the architecture: synthesized
installed BMIs need the same target as their consumers.

The fixture covers:

- SHA-512(`abc`) and SM3(`abc`), with every expanded message word and compression
  round produced through the public instructions.
- The standard SM4 key-schedule/encryption example in both vector widths.
- Strong and feature-absent constant evaluation for those complete computations.
- Ten seeded one-block messages, including empty and padding-boundary inputs,
  checked against independent Python hashlib/OpenSSL SHA-512 and SM3 digests.
  `generate_seeded.py` regenerates the checked-in expected values.
- Independent CPUID bits, subleaf availability, XCR0 admission, compiler target
  prerequisites and provider-baseline macro snapshots, including AVX-NE-CONVERT.
- Feature, caller-target, raw-register, shape and immediate rejection controls.
- Thirteen raw-intrinsic/public codegen pairs, including SM3 immediate 255.

LLVM 23.1.1 x86_64/macOS qualification passes all sixteen constant-evaluation,
metadata, codegen and negative controls in both the source-provider build and a
relocated installed-package consumer. The three native-instruction runtime tests
skip under Rosetta. No SHA512/SM3/SM4-capable execution host has been qualified.
The runtime executable retains the actual instruction leaves and admits each
family independently. These results establish no timing claim.

The source-only SM4 substitution permutation is shared with the ARM SM4 module;
it does not make one architecture's instruction module depend on the other.
