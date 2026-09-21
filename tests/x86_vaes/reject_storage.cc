// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.x86.vaes;

#if NATIVE_TEST_WIDTH == 512
constexpr auto storage = native::target_features<native::x86>("avx512f");
constexpr auto weaker = native::target_features<native::x86>("avx");
#elif NATIVE_TEST_WIDTH == 256
constexpr auto storage = native::target_features<native::x86>("avx");
constexpr auto weaker = native::target_features<native::x86>("sse2");
#else
constexpr auto storage = native::target_features<native::x86>("sse2");
constexpr native::isa<native::x86> weaker{};
#endif
#if NATIVE_TEST_STORAGE
constexpr auto arch = weaker;
#else
constexpr auto arch = storage;
#endif
using vector = native::simd<std::uint8_t, NATIVE_TEST_WIDTH / 8, arch>;
static_assert(sizeof(vector) == NATIVE_TEST_WIDTH / 8);
