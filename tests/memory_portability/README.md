# Memory module checks

`import native.memory;` provides malloc/free ownership and independent C-string
copying. On POSIX builds the same module also exports `mmap_ptr` and its deleter;
`sys/mman.h` is included only in its global module fragment on those platforms.
No production header is required. These tests import the actual module.

The host checks move, reset, release, array ownership and independent empty and
nonempty string copies. The POSIX check creates, writes, moves and unmaps a real
mapping. The attribute checks remain compile-time probes. Configure
`-DNATIVE_MEMORY_ASAN=ON` to instrument both the module and its consumers.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
