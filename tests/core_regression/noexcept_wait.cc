// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.wait;
#include <utility>

// Test callback dispatch without executing a platform-specific wait instruction.
namespace {
  struct ready {
    bool operator()(volatile int *) const noexcept { return true; }
  };
  struct throwing_predicate {
    bool operator()(volatile int *) const noexcept(false);
  };
  struct waiter {
    static constexpr bool supported = true;
    static void monitor(volatile void *) noexcept {}
    static void mwait(unsigned) noexcept {}
  };
  struct throwing_waiter : waiter {
    static void monitor(volatile void *) noexcept(false);
  };
  struct callback {
    template<simd::waiter W> int operator()() const noexcept { return 7; }
  };
  struct throwing_callback {
    template<simd::waiter W> int operator()() const noexcept(false);
  };
  static_assert(noexcept(simd::wait_until<waiter>(static_cast<volatile int *>(nullptr), ready{})));
  static_assert(!noexcept(simd::wait_until<waiter>(static_cast<volatile int *>(nullptr), throwing_predicate{})));
  static_assert(!noexcept(simd::wait_until<throwing_waiter>(static_cast<volatile int *>(nullptr), ready{})));
  static_assert(noexcept(simd::with_waiter(callback{})));
  static_assert(!noexcept(simd::with_waiter(throwing_callback{})));
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  bool throwing_predicate::operator()(volatile int *) const { throw 42; }
  template<simd::waiter W> int throwing_callback::operator()() const { throw 43; }
#else
  bool throwing_predicate::operator()(volatile int *) const { return true; }
  template<simd::waiter W> int throwing_callback::operator()() const { return 43; }
#endif
}
int main() {
  int value = 0;
  simd::wait_until<waiter>(&value, ready{});
  if (simd::with_waiter(callback{}) != 7) return 1;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  int caught = 0;
  try { simd::wait_until<waiter>(&value, throwing_predicate{}); }
  catch (int x) { if (x != 42) return 2; ++caught; }
  try { (void)simd::with_waiter(throwing_callback{}); }
  catch (int x) { if (x != 43) return 3; ++caught; }
  if (caught != 2) return 4;
#endif
}
