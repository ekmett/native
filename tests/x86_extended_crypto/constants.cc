// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
import native.x86.sha512;
import native.x86.sm3;
import native.x86.sm4;
#include "checks.h"
using namespace native;
static_assert(checks::sha512_abc<target_features<x86>("sha512")>());
static_assert(checks::sha512_abc<target_features<x86>("avx2")>());
static_assert(checks::sm3_abc<target_features<x86>("sm3")>());
static_assert(checks::sm3_abc<target_features<x86>("sse2")>());
static_assert(checks::sm4_example<target_features<x86>("sm4"),4>());
static_assert(checks::sm4_example<target_features<x86>("sm4"),8>());
static_assert(checks::sm4_example<target_features<x86>("sse2"),4>());
static_assert(checks::sm4_example<target_features<x86>("avx2"),8>());
int main() {}
