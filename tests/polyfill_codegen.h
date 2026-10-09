// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace native_test {
  template<auto Family>
  constexpr native::isa<Family> codegen_arch(native::isa<Family> arch) noexcept {
#ifdef NATIVE_TEST_CODEGEN_POLYFILL
    return arch | native::polyfill;
#else
    return arch;
#endif
  }
}
