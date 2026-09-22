// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include "native/attributes.h"
#include <cstdio>
import native.x86.wait;
import native.x86.features;
static_assert(__is_same(decltype(&native::umwait::mwait),
  std::uint8_t (*)(std::uint64_t) noexcept native_blocking));
static_assert(native::waiter<native::umwait>);
extern "C" [[gnu::target("waitpkg"), gnu::noinline]]
unsigned char pause_deadline(std::uint64_t deadline) {
  return native::tpause<native::x86_feature::waitpkg>(deadline);
}
int main() {
  if (!native::classify_isa(native::observe_x86_capabilities(),
      native::x86_feature::waitpkg).admitted()) return 77;
  // An expired deadline bounds this smoke independently of OS wait limits.
  auto result = pause_deadline(0);
  std::printf("TPAUSE expired deadline: status=%u\n", unsigned(result));
  return result > 1;
}
