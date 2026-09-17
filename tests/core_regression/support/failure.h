#pragma once
#include <cstdio>
#include <cstdlib>

namespace simd::test {
  template<class E> [[noreturn]] inline void fail(E const & error) noexcept {
    std::fprintf(stderr, "%s\n", error.what());
    std::abort();
  }
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
