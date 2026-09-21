// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>
import native;
extern "C" native_noinline unsigned native_pclmul_baseline(unsigned x, unsigned y) noexcept {
  return (x * 17u) ^ y;
}
