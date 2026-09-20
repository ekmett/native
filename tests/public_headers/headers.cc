#include <native/config.h>
#include <native/attributes.h>

#if __has_include(<native/vec.h>) || __has_include(<native/simd/config.h>)
#error Module implementation headers must not leak through native::headers
#endif

int main() { return 0; }

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
