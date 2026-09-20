// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/mask_traits.h>
#include <native/targets.h>

static_assert(std::is_same_v<native::mask<float const&>, bool>);
static_assert(std::is_same_v<native::mask<std::array<float, 3>>, std::array<bool, 3>>);

static_assert(NATIVE_TARGET_ISA(avx2) == native::avx2);
static_assert(NATIVE_TARGET_ISA(neon) == native::neon);

template<native::isa A> requires(A == NATIVE_TARGET_ISA(avx2))
constexpr int repeated();
template<native::isa A> requires(A == NATIVE_TARGET_ISA(avx2))
constexpr int repeated() { return 7; }
static_assert(repeated<native::avx2>() == 7);

#define METADATA_TARGETS(X, ...) X(scalar, __VA_ARGS__)
#define METADATA_BODY(name, arch) \
  template<native::isa A> requires(A == arch) \
  int name(int value) { return value + 1; }
NATIVE_TARGET_VARIANTS(increment, METADATA_TARGETS, METADATA_BODY)

int main() { return increment<native::scalar>(41) == 42 ? 0 : 1; }
