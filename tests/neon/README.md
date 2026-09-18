# Apple ARM64 NEON checks

`SIMD_TEST_ISA=NEON` selects the primary test profile. The AArch64 hub includes
NEON and native half families. Architecture tags distinguish their types.
No runtime SIMD dispatch or x86 flags are needed.

Run `run.py` through the host's CPU resource gate, using a fresh output directory:

```sh
python3 tests/neon/run.py --source /path/to/simd --source-commit <revision> --tools /path/to/tool-root --out /path/to/new-output --lease-env HOST_CPU_LEASE
```

`--lease-env` is optional and checks that the named environment variable is
nonempty; it does not acquire a lock or prove exclusive access. The caller is
responsible for the real resource gate whether or not this check is requested.
The tool root contains the Clang 23.1.1 and CMake 4.4.3 bundles named in the
runner; Ninja is read from `/opt/homebrew/bin/ninja`. Builds use at most two jobs.

The runner checks that CMake consumed its NEON, PCH and IPO options. The profile fixture independently
checks elementary arithmetic and compares header/import packets with eight
columns of 96 words, including raw wide exp, integer transport and guarded
memory tails. Other host tests cover masks, scaling and half conversions in
explicitly selected floating-point environments. Caller controls are restored.

The runner relocates the installed package to a path with spaces and builds
separate consumers, requiring identical build-tree and installed packets. Standalone memory tests exercise ownership and POSIX
mappings, ordinarily and with ASan. Exact source/tool hashes, commands, streams
and JUnit stay in the output directory. These are Apple CPU/compiler checks;
they imply no Metal, GPU, universal ARM or performance result.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
