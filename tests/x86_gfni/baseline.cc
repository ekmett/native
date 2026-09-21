// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/attributes.h>
import native;

// The omnibus import exposes GFNI without enabling optional instructions here.
extern "C" native_noinline unsigned native_gfni_baseline(unsigned x, unsigned y) noexcept {
  return (x * 17u) ^ y;
}
