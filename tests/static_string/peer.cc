// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.static_string;
using namespace simd;
static_string peer_string() { return "interned"_ss; }
static_wstring peer_wstring() { return L"interned"_ss; }
static_u8string peer_u8string() { return u8"interned"_ss; }
static_u16string peer_u16string() { return u"interned"_ss; }
static_u32string peer_u32string() { return U"interned"_ss; }
static_c_string peer_c_string() { return "interned"_scs; }
static_string peer_empty() { return ""_ss; }
