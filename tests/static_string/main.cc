// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <algorithm>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <utility>
import simd.static_string;
using namespace simd;
static_string peer_string();
static_wstring peer_wstring();
static_u8string peer_u8string();
static_u16string peer_u16string();
static_u32string peer_u32string();
static_c_string peer_c_string();
static_string peer_empty();
bool import_only_check();
void require(bool ok) { if (!ok) { std::fputs("static_string check failed\n", stderr); std::abort(); } }
template<class S> void check(S local, S peer) {
  using C = typename S::value_type;
  static_assert(std::ranges::contiguous_range<S>);
  static_assert(std::ranges::borrowed_range<S>);
  static_assert(std::ranges::view<S>);
  static_assert(std::is_trivially_copyable_v<S>);
  require(local == peer && local.data() == peer.data());
  require(local.size() == 8 && local.data()[8] == C());
  require(local.front() == C('i') && local.back() == C('d') && local.at(1) == C('n'));
  require(std::hash<S>{}(local) == std::hash<S>{}(peer));
  std::unordered_set<S> values{local, peer};
  require(values.size() == 1);
  std::basic_string<C> owned = local;
  std::basic_string_view<C> view = local;
  require(owned.size() == 8 && view == owned);
  require(std::ranges::equal(local, view));
  require(*local.rbegin() == C('d') && local.crend() - local.crbegin() == 8);
  S empty;
  require(empty.data() != nullptr && empty.data()[0] == C());
  require(empty.empty() && empty.begin() == empty.end() && empty.rbegin() == empty.rend());
  auto moved = std::move(local);
  require(moved == peer && local == peer);
  swap(moved, empty);
  require(moved.empty() && empty == peer);
}
int main() {
  require(import_only_check());
  check("interned"_ss, peer_string());
  check(L"interned"_ss, peer_wstring());
  check(u8"interned"_ss, peer_u8string());
  check(u"interned"_ss, peer_u16string());
  check(U"interned"_ss, peer_u32string());
  constexpr static_string sequence(std::integer_sequence<char, 'i','n','t','e','r','n','e','d'>{});
  static_assert(sequence == "interned"_ss);
  require(sequence.data() == peer_string().data());
  require(peer_empty() == static_string{});
  static_assert(std::ranges::contiguous_range<static_c_string>);
  static_assert(std::ranges::borrowed_range<static_c_string>);
  static_assert(std::ranges::view<static_c_string>);
  auto c = "interned"_scs;
  require(c == peer_c_string() && c.data() == peer_string().data());
  require(std::hash<static_c_string>{}(c) == std::hash<static_c_string>{}(peer_c_string()));
  static_string restored = c;
  require(restored == "interned"_ss && restored.size() == 8);
  static_c_string empty;
  require(empty.data() != nullptr && empty.data()[0] == 0 && empty.begin() == empty.end());
  require(empty.rbegin() == empty.rend());
  std::ostringstream os; os << "output"_ss;
  require(os.str() == "output");
  std::wostringstream ws; ws << L"output"_ss;
  require(ws.str() == L"output");
  // The compatibility constructor remains available, with its interned-storage precondition.
  static_string escape(std::string_view("interned"_ss.data(), 8));
  require(escape == peer_string());
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  bool caught = false;
  try { (void)restored.at(restored.size()); } catch (std::out_of_range const &) { caught = true; }
  require(caught);
#endif
  std::puts("PASS: static-string module, five widths, cross-TU storage, ranges, hash and empty defaults");
}
