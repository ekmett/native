# Compiled API examples

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

The Doxygen examples are snippets of `api.cc`, compiled and executed against an
installed package. They check API use and the displayed results; they are not an
additional numerical accuracy or performance qualification. The local `check`
helper aborts on a failed example assertion, including in Release builds.

```sh
cmake -S tests/api -B build/api -G Ninja -DCMAKE_CXX_COMPILER=clang-cl \
  -DCMAKE_BUILD_TYPE=Release -Dsimd_DIR=/prefix/lib/cmake/simd \
  '-DAPI_PROFILES=AVX2;AVX512'
cmake --build build/api --parallel 2
ctest --test-dir build/api --output-on-failure
```

Choose only architectures admitted for execution. The default is AVX2 on x86,
NEON on ARM. Use clang++ on Apple. The caller supplies its resource gate.
Doxygen's example path should include this directory.

`audit_docs.py XML_DIRECTORY` checks the public function/friend IDs referenced by
Doxygen's current `index.xml`, including definitions emitted only in groups.
It de-duplicates IDs across namespace/class/struct/group pages, excludes friend
class declarations, ignores stale XML and private/detail entities,
and fails for missing descriptions or an empty result. A documented overload-set
alias is counted separately when Doxygen emits a distinct ID. For a focused
native-header review, use `--source-prefix src/cxx/simd/`; omit that option for the
whole public source surface. Run the check for each documentation profile used to
expose conditional declarations; a clean warning log alone is not coverage.

Run `python tests/api/test_audit_docs.py` for the group-only definition, duplicate
ID, hidden helper, stale XML and empty-index regression checks.
