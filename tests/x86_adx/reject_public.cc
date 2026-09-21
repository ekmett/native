// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <native/attributes.h>
#if NATIVE_TEST_PRIVATE
#include <native/x86/adx.h>
#else
import native.x86.adx;
#endif

#if NATIVE_TEST_WEAK || NATIVE_TEST_FEATURE
constexpr native::isa<native::x86> arch{};
#else
constexpr auto arch = native::target_features<native::x86>("adx");
#endif
#if NATIVE_TEST_TARGET
#define NATIVE_TEST_CALLER_TARGET "no-adx"
#else
#define NATIVE_TEST_CALLER_TARGET "adx"
#endif

#if NATIVE_TEST_WIDTH == 32
using value = std::uint32_t;
#else
using value = std::uint64_t;
#endif
#if NATIVE_TEST_SHAPE == 1
using output = std::int64_t;
#elif NATIVE_TEST_SHAPE == 2
using output = value const;
#else
using output = value;
#endif

native_noinline native_target(NATIVE_TEST_CALLER_TARGET)
auto reject_addcarryx(std::uint8_t carry, value a, value b, output * result) noexcept {
#if NATIVE_TEST_PRIVATE
  return native::detail::x86_adx::addcarryx<arch>(carry, a, b, result);
#elif NATIVE_TEST_DEFAULT
  return native::addcarryx(carry, a, b, result);
#else
  return native::addcarryx<arch>(carry, a, b, result);
#endif
}
