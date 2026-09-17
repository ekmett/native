// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.static_string;
using namespace simd;
static_assert("literal"_ss.size() == 7);
static_assert("literal"_ss == "literal"_ss);
static_assert("literal"_ss != "other"_ss);
static_assert(static_string{}.data() == ""_ss.data());
static_assert(static_c_string{}.data() == ""_scs.data());
static_assert(static_string(static_c_string{}) == ""_ss);
static_assert(static_string{}.begin() == static_string{}.end());
static_assert(static_c_string{}.begin() == static_c_string{}.end());
static_assert(!static_string{});
static_assert("x"_ss);
static_assert("a\0b"_ss.size() == 3);
bool import_only_check() {
  constexpr auto text = "literal"_ss;
  unsigned count = 0;
  for (char c : text) count += c != 0;
  return count == 7 && std::hash<static_string>{}(text) ==
    std::hash<static_string>{}("literal"_ss);
}
