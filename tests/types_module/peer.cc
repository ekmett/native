// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#if SIMD_TEST_NUMERICS
import simd.numerics;
#else
import simd.types;
#endif

namespace fixture {
  void const * name_storage() { return simd::type<sample>.data(); }
  bool peer_checks() {
    sample object;
    auto saved = simd::type_of(object);
    return saved.compare(simd::type<sample>) == 0 && saved.data() == name_storage() &&
      simd::type<int const>.compare(simd::type<int>) == 0 &&
      simd::type<int &>.compare(simd::type<int>) == 0;
  }
}
