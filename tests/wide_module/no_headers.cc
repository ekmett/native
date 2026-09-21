// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.wide;

constexpr bool import_only() {
  native::wide one{7};
  auto [a] = one;
  native::wide values{1,2,3};
  auto [x,y,z] = values;
  auto const sum = values + values;
  auto [p,q,r] = sum;
  return a == 7 && x+y+z == 6 && p+q+r == 12;
}
static_assert(import_only());
