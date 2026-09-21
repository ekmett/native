// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "contract.h"
#if NATIVE_TEST_NUMERICS
import native.numerics;
#else
import native.types;
#endif

namespace fixture {
  void const * name_storage() { return native::type<sample>.data(); }
  bool peer_checks() {
    sample object;
    auto saved = native::type_of(object);
    return saved.compare(native::type<sample>) == 0 && saved.data() == name_storage() &&
      native::type<int const>.compare(native::type<int>) == 0 &&
      native::type<int &>.compare(native::type<int>) == 0;
  }
}
