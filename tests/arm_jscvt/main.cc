// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#if NATIVE_TEST_INTERFACE == 1
import native.arm;
#elif NATIVE_TEST_INTERFACE == 2
import native;
#else
import native.arm.jscvt;
#endif

constexpr auto arch = native::target_features("jsconv");
static_assert(arch.has(native::arm_feature::jsconv));
constexpr native::isa<native::arm> weak{};
constexpr std::array<double, 18> inputs{
  0., -0., 1.9, -1.9, 2147483647., 2147483648., -2147483649.,
  4294967295., 4294967296., 4294967297., -4294967297.,
  0x1.0000000000001p52, 0x1.0000000000001p83, 0x1.0000000000001p84,
  std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(),
  std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::denorm_min()};
constexpr std::array<std::int32_t, inputs.size()> expected{
  0, 0, 1, -1, INT32_MAX, INT32_MIN, INT32_MAX, -1, 0, 1, -1,
  1, INT32_MIN, 0, 0, 0, 0, 0};
static_assert([] {
  for (std::size_t i = 0; i < inputs.size(); ++i)
    if (native::jcvt<arch>(inputs[i]) != expected[i] ||
        native::jcvt<weak>(inputs[i]) != expected[i]) return false;
  return true;
}());
static_assert(native::jcvt(1.9) == 1);
static_assert(noexcept(native::jcvt<arch>(0.)));

struct sample { double input; std::int32_t output; };
constexpr auto constant_bank = [] consteval {
  std::array<sample, 2048> result{};
  std::uint64_t state = 0x289a75b6ea12c317;
  for (auto & item : result) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    item.input = std::bit_cast<double>(state);
    item.output = native::jcvt<weak>(item.input);
    if (item.output != native::jcvt<arch>(item.input)) __builtin_abort();
  }
  return result;
}();

// A separate arithmetic reference avoids sharing the polyfill's bit decomposition.
std::int32_t reference(double x) {
  if (!std::isfinite(x)) return 0;
  auto word = std::fmod(std::trunc(x), 4294967296.);
  if (word < 0) word += 4294967296.;
  return std::bit_cast<std::int32_t>(std::uint32_t(word));
}

__attribute__((target("jsconv"), noinline))
std::int32_t hardware(double x) { return native::jcvt<arch>(x); }

int main() {
  if (!native::classify_isa(native::observe_arm_capabilities(), arch).admitted()) return 77;
  for (std::size_t i = 0; i < inputs.size(); ++i)
    if (hardware(inputs[i]) != expected[i]) return 1;
  for (auto const & item : constant_bank)
    if (hardware(item.input) != item.output || reference(item.input) != item.output) return 2;
  std::uint64_t state = 0x98fa62cb170ba459;
  for (unsigned i = 0; i < 100000; ++i) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    double value = std::bit_cast<double>(state);
    auto actual = hardware(value), want = reference(value);
    if (actual != want) {
      std::printf("jscvt mismatch input=%016llx actual=%d expected=%d\n",
        static_cast<unsigned long long>(state), actual, want);
      return 1;
    }
  }
  std::puts("JSCVT: constant corners and 100000 seeded binary64 inputs passed");
}
