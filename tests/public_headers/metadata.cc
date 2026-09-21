// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#include <native/mask_traits.h>
#include <native/targets.h>
#include <native/wasm/features.h>

static_assert(std::is_same_v<native::mask<float const&>, bool>);
static_assert(std::is_same_v<native::mask<std::array<float, 3>>, std::array<bool, 3>>);

constexpr auto wasm=native::decode_wasm_capabilities({true,true,true,false});
static_assert(native::classify_isa(wasm,native::wasm_feature::simd128).admitted());
static_assert(!native::classify_isa(wasm,native::wasm_feature::relaxed_simd).admitted());
static_assert(native::wasm_feature_probe(native::wasm_feature::simd128).size()==43);

#if defined(__x86_64__) || defined(_M_X64)
static_assert(NATIVE_TARGET_ISA(avx2) == native::avx2);
#elif defined(__aarch64__) || defined(_M_ARM64)
static_assert(NATIVE_TARGET_ISA(neon) == native::neon);
#endif

template<native::isa<> A> requires(A == NATIVE_TARGET_ISA(scalar))
constexpr int repeated();
template<native::isa<> A> requires(A == NATIVE_TARGET_ISA(scalar))
constexpr int repeated() { return 7; }
static_assert(repeated<native::scalar>() == 7);

#define METADATA_TARGETS(X, ...) X(scalar, __VA_ARGS__)
#define METADATA_BODY(name, arch) \
  template<native::isa<> A> requires(A == arch) \
  int name(int value) { return value + 1; }
NATIVE_TARGET_VARIANTS(increment, METADATA_TARGETS, METADATA_BODY)

int main() { return increment<native::scalar>(41) == 42 ? 0 : 1; }
