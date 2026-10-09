// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "../property_check.h"
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
import native.x86.adx;

namespace {
  constexpr native::isa<native::x86> emulated = native::polyfill;
  static_assert(!emulated.has(native::x86_feature::bmi1));
  static_assert(!emulated.has(native::x86_feature::bmi2));

  template<class U> constexpr U low_bits(unsigned count) noexcept {
    constexpr auto width = sizeof(U) * 8;
    return count >= width ? ~U{} : U((U{1} << count) - 1);
  }
  template<class U> constexpr std::array<U, 2> product(U a, U b) noexcept {
    constexpr auto width = sizeof(U) * 8;
    U low{}, high{};
    for (unsigned bit = 0; bit < width; ++bit) {
      if (((b >> bit) & 1) == 0) continue;
      auto add = U(a << bit);
      auto old = low;
      low = U(low + add);
      high = U(high + (bit ? a >> (width - bit) : 0) + (low < old));
    }
    return {low, high};
  }
  template<class U> constexpr bool values(U x, U mask) noexcept {
    constexpr auto width = unsigned(sizeof(U) * 8);
    auto extracted = native::pext<emulated>(x, mask);
    auto deposited = native::pdep<emulated>(x, mask);
    if (native::pext<emulated>(deposited, mask) != (x & low_bits<U>(std::popcount(mask))) ||
        native::pdep<emulated>(extracted, mask) != (x & mask)) return false;
    if (native::pdep<emulated>(U(5), U(22)) != U(18) ||
        native::pext<emulated>(U(5), U(22)) != U(2)) return false;
    if (native::andn<emulated>(x, mask) != U(~x & mask) ||
        native::blsi<emulated>(x) != U(x & U(0 - x)) ||
        native::blsmsk<emulated>(x) != U(x ^ U(x - 1)) ||
        native::blsr<emulated>(x) != U(x & U(x - 1))) return false;
    if (native::popcnt<emulated>(x) != U(std::popcount(x)) ||
        native::lzcnt<emulated>(x) != U(std::countl_zero(x)) ||
        native::tzcnt<emulated>(x) != U(std::countr_zero(x))) return false;
    for (auto count : std::array<unsigned, 7>{0, 1, width - 1, width, width + 1, 255, 256}) {
      auto index = count & 255u;
      if (native::bzhi<emulated>(x, count) != U(x & low_bits<U>(index))) return false;
      auto n = count % width;
      if (native::shlx<emulated>(x, count) != U(x << n) ||
          native::shrx<emulated>(x, count) != U(x >> n)) return false;
      using S = std::make_signed_t<U>;
      if (native::sarx<emulated>(std::bit_cast<S>(x), count) != (std::bit_cast<S>(x) >> n)) return false;
      auto begin = count & 255u;
      U expected{};
      for (unsigned bit = 0; bit < width; ++bit)
        if (bit < index && begin < width && bit < width - begin)
          expected |= U(((x >> (begin + bit)) & 1) << bit);
      if (native::bextr<emulated>(x, count, count) != expected ||
          native::bextr<emulated>(x, std::uint32_t(begin | (index << 8))) != expected) return false;
    }
    if (native::rorx<emulated, 255>(x) != std::rotr(x, 255) ||
        native::rorx<255, emulated>(x) != std::rotr(x, 255)) return false;
    auto accumulator = std::uint32_t(mask);
    auto crc = accumulator;
    for (unsigned bit = 0; bit < width; ++bit) {
      auto low = (crc ^ std::uint32_t(x >> bit)) & 1;
      crc = (crc >> 1) ^ (low ? 0x82f63b78u : 0u);
    }
    if (native::crc32c<emulated>(accumulator, x) != crc) return false;
    auto bytes = accumulator;
    for (unsigned byte = 0; byte < sizeof(U); ++byte)
      bytes = native::crc32c<emulated>(bytes, std::uint8_t(x >> (byte * 8)));
    if (bytes != crc) return false;
    auto words = accumulator;
    for (unsigned word = 0; word < sizeof(U) / 2; ++word)
      words = native::crc32c<emulated>(words, std::uint16_t(x >> (word * 16)));
    if (words != crc) return false;
    auto expected = product(x, mask);
    U high{};
    if (native::mulx<emulated>(x, mask, &high) != expected[0] || high != expected[1]) return false;
    for (auto carry : std::array<std::uint8_t, 3>{0, 1, 255}) {
      auto first = U(x + mask);
      auto sum = U(first + (carry != 0));
      U result{};
      auto overflow = native::addcarryx<emulated>(carry, x, mask, &result);
      if (result != sum || overflow != std::uint8_t(first < x || sum < first)) return false;
    }
    return true;
  }
  constexpr bool counts16() noexcept {
    return native::tzcnt<emulated>(std::uint16_t{}) == 16 &&
      native::lzcnt<emulated>(std::uint16_t{}) == 16 &&
      native::popcnt<emulated>(std::uint16_t(0x8001)) == 2;
  }
  constexpr bool crc() noexcept {
    auto accumulator = ~std::uint32_t{};
    for (auto value : "123456789")
      if (value) accumulator = native::crc32c<emulated>(accumulator, std::uint8_t(value));
    return ~accumulator == 0xe3069283u;
  }
  static_assert(values(std::uint32_t{}, ~std::uint32_t{}));
  static_assert(values(~std::uint64_t{}, std::uint64_t(0x8000000000000001ull)));
  static_assert(counts16() && crc());
}
int main() {
  auto settings = native_test::property_config(64);
  native_test::property_rng random{settings.seed};
  for (std::size_t sample = 0; sample < settings.cases; ++sample) {
    auto x = random.next(), mask = random.next();
    if (!native_test::property_check("scalar polyfills", settings.seed, sample,
        values(std::uint32_t(x), std::uint32_t(mask)) && values(x, mask), x, mask)) return 1;
  }
  return counts16() && crc() ? 0 : 1;
}
