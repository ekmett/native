# Type utilities module tests

`native.types` exports `type<T>`, `type_of`, `one_of_t` and `not_one_of_t` from the
baseline common provider. `native.numerics` re-exports it. Type names are backed by
an owning static string shared across importers. Windows uses the MSVC-ABI
`typeid` name; platforms with `cxxabi.h` try demangling and fall back on failure.
Exact spelling is implementation-defined. Allocating initialization has no
nonthrowing or purity promise; escaping static initialization follows the
language's termination rules.

The fixture imports both routes from two translation units, including a peer
without standard includes. It checks empty/cv/reference/array concept sets,
constrained calls, distinct nonempty names, cvref-erasing `type_of`, and shared
storage. It does not assume identical name spelling across ABIs.

```sh
cmake -S tests/types_module -B build/types -G Ninja -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release
cmake --build build/types --parallel 2
ctest --test-dir build/types --output-on-failure
```

Use the initialized host toolchain and resource gate, with separate builds for
exceptions OFF/ON and ASan. No GPU or performance test is included.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
