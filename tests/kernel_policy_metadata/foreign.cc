// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if NATIVE_TEST_IMPORT
import native;
#else
#include "native/value_traits.h"
#endif

struct foreign_value {
  static constexpr auto architecture=[] {
#if NATIVE_TEST_ENUM
    if constexpr(native::target_arch==native::wasm) return native::arm_feature::neon;
    else return native::wasm_feature::simd128;
#else
    if constexpr(native::target_arch==native::wasm) return native::isa<native::arm>{};
    else return native::isa<native::wasm>{};
#endif
  }();
  int number{};
  friend constexpr foreign_value operator+(foreign_value a,foreign_value b) noexcept {
    return {a.number+b.number};
  }
};
#if NATIVE_TEST_IMPORT
auto rejected(native::wide<foreign_value,1> value) { return value+value; }
#else
constexpr auto rejected=native::detail::value_architecture_v<foreign_value>;
#endif
