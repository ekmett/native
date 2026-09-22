// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.x86.wait;
#if NATIVE_WAIT_NEGATIVE != 2
[[gnu::target("waitpkg")]]
#endif
unsigned char probe(std::uint64_t deadline) {
#if NATIVE_WAIT_NEGATIVE == 1
  return native::tpause(deadline);
#elif NATIVE_WAIT_NEGATIVE == 3
  return native::tpause<native::x86_feature::waitpkg, 2>(deadline);
#else
  return native::tpause<native::x86_feature::waitpkg>(deadline);
#endif
}
