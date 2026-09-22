// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include "native/attributes.h"
#include <cstdio>
#include <x86intrin.h>
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
  // A deadline one tick in the past exercises both halves without waiting.
  auto deadline = std::uint64_t(__rdtsc()) - 1;
  auto result = pause_deadline(deadline);
  volatile std::uint32_t monitored = 0;
  native::umwait::monitor(&monitored);
  auto waited = native::umwait::mwait(deadline);
  std::printf("WAITPKG expired deadline=%llu: TPAUSE=%u UMWAIT=%u\n",
    static_cast<unsigned long long>(deadline), unsigned(result), unsigned(waited));
  return result > 1 || waited > 1;
}
