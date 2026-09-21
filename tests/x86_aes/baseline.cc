// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>

import native;

extern "C" native_noinline unsigned native_aes_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
