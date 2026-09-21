// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#if NATIVE_TEST_INTERFACE == 1
import native.x86.sha;
import native.features;
#else
import native;
#endif
#include "checks.h"

extern "C" native_noinline unsigned native_sha_baseline_import(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}

int main() {
  return sha_fixture::run();
}
