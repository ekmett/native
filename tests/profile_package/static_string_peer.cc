// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.static_string;
using namespace simd;
void const * static_string_peer(unsigned width) {
  switch (width) {
    case 0: return "installed"_ss.data();
    case 1: return L"installed"_ss.data();
    case 2: return u8"installed"_ss.data();
    case 3: return u"installed"_ss.data();
    default: return U"installed"_ss.data();
  }
}
static_string static_string_peer_value() { return "installed"_ss; }
