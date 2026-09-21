// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
import native;
using B=native::vec<native::bf16,8,native::neon_bf16>;
using F=native::vec<float,4,native::neon_bf16>;
static_assert(sizeof(B)==16);
static_assert(std::same_as<decltype(native::dot2(B{},B{},F{})),F>);
