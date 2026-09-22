// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#if NATIVE_WAIT_RAW
#include <immintrin.h>
#else
import native.x86.wait;
#endif
extern "C" [[gnu::target("waitpkg"), gnu::noinline]]
unsigned char native_tpause(std::uint64_t deadline) {
#if NATIVE_WAIT_RAW
  return _tpause(1, deadline);
#else
  return native::tpause<native::x86_feature::waitpkg>(deadline);
#endif
}
