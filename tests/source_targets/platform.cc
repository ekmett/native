// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>

static_assert(native::is_arm == bool(EXPECT_ARM));
static_assert(native::is_x86 == bool(EXPECT_X86));
static_assert(native::is_wasm == bool(EXPECT_WASM));
static_assert(int(native::is_arm) + int(native::is_x86) + int(native::is_wasm) <= 1);

template<class T> constexpr int select_family(T) {
  if constexpr (native::is_arm) return 1;
  else if constexpr (native::is_x86) return 2;
  else if constexpr (native::is_wasm) return 3;
  else return 0;
}
static_assert(select_family(0) == (EXPECT_ARM ? 1 : EXPECT_X86 ? 2 : EXPECT_WASM ? 3 : 0));
