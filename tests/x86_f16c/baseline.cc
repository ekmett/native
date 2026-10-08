// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <hint.h>
import native;
extern "C" hint_noinline unsigned native_f16c_baseline(unsigned x, unsigned y) noexcept {
  return (x * 17u) ^ y;
}
