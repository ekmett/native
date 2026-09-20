// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.static_string;
using namespace native;
// No textual includes: the installed module owns definitions and literal storage.
void const * static_string_peer(unsigned width);
static_string static_string_peer_value();
static_assert("installed"_ss.size() == 9);
static_assert(static_string{}.data() == ""_ss.data());
static_assert(static_c_string{}.data() == ""_scs.data());
extern "C" int static_string_check() {
  if ("installed"_ss.data() != static_string_peer(0)) return 1;
  if (L"installed"_ss.data() != static_string_peer(1)) return 2;
  if (u8"installed"_ss.data() != static_string_peer(2)) return 3;
  if (u"installed"_ss.data() != static_string_peer(3)) return 4;
  if (U"installed"_ss.data() != static_string_peer(4)) return 5;
  auto local = "installed"_ss;
  auto peer = static_string_peer_value();
  if (local != peer || std::hash<static_string>{}(local) != std::hash<static_string>{}(peer)) return 6;
  unsigned count = 0;
  for (auto c : peer) count += c != 0;
  if (count != 9 || static_string("installed"_scs) != peer) return 7;
  return 0;
}
