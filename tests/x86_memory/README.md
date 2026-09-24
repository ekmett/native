# Indexed memory fixture

This fixture imports the public `native.x86.memory` module and the `native`
hub. Runtime checks admit AVX2, AVX512F and AVX512VL independently. A machine
without any applicable profile returns CTest's skip code; a machine lacking
AVX512 still executes the AVX2 bank. Protected-page allocation is required for
the inactive-address checks.

The checks cover the exact mixed index/data register shapes, all four scales,
signed and unsigned integer data, negative indices, AVX2 sign-bit masks,
AVX512 predicates, empty masks with null bases, ignored indices and zeroed
excess result lanes. Duplicate scatter destinations include a masked highest
lane. Runtime-only byte accesses use independent `memcpy` oracles for unaligned
loads and partially overlapping stores. Constant evaluation uses independent
array indexing and forward stores over the same typed public operations.

The code-generation fixture compares all 480 public instruction bodies with
direct intrinsic controls, and requires one gather/scatter instruction per
body with no helper calls or branches, under both minimal and broad caller feature sets. Eighteen rejection builds cover invalid
scales, index/result shapes, mask types, target mismatches, runtime weak-tag
calls and invalid constant memory accesses. The successful boundary control
also checks constant evaluation with SSE4.2 storage and no gather/scatter ISA.

Build the fixture through the repository CMake configuration and run
`ctest --test-dir <build> -R '^native\.x86\.memory\.' --output-on-failure`.
It can also be configured independently against an installed package using
`native_DIR`. The rejection tests serialize their build commands so they do
not run concurrent Ninja processes against one build tree.

Clang 23.1.1 qualification passes all 22 checks from source and all 22 again
after package relocation on Linux x86-64 and Windows x64. The Linux host runs
the AVX2 bank; the Windows host runs AVX2, AVX512F and AVX512F+VL. Both hosts
pass the complete minimal/broad code-generation comparisons. These checks
establish instruction equivalence and memory semantics, not gather/scatter
throughput on other CPUs.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
