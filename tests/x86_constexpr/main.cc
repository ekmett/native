// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
#if NATIVE_CONSTEXPR_INTERFACE == 0
#include <native/x86/bmi1.h>
#include <native/x86/bmi2.h>
#include <native/x86/popcnt.h>
#include <native/x86/lzcnt.h>
#include <native/x86/crc32c.h>
#elif NATIVE_CONSTEXPR_INTERFACE == 1
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
#else
import native;
#endif
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

namespace scalar_constexpr_fixture {
  template<auto A> consteval bool rejects_foreign_family() {
    return !requires(std::uint32_t x) { native::andn<A>(x,x); } &&
      !requires(std::uint32_t x) { native::pdep<A>(x,x); } &&
      !requires(std::uint32_t x) { native::popcnt<A>(x); } &&
      !requires(std::uint32_t x) { native::lzcnt<A>(x); } &&
      !requires(std::uint32_t x) { native::crc32c<A>(x,x); };
  }
  static_assert(rejects_foreign_family<native::isa<native::arm>{}>());
  static_assert(rejects_foreign_family<native::isa<native::wasm>{}>());
  static_assert(!rejects_foreign_family<native::isa<native::x86>{}>());

  constexpr native::isa<native::x86> strong = native::isa<native::x86>{native::x86_feature::bmi1} & native::x86_feature::bmi2 &
    native::x86_feature::popcnt & native::x86_feature::lzcnt & native::x86_feature::crc32;

  template<class U> constexpr unsigned width = sizeof(U) * 8;
  template<class U> constexpr U all = std::numeric_limits<U>::max();

  constexpr std::uint64_t random(std::uint64_t& state) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
  }

  template<class U> constexpr unsigned count(U x) {
    unsigned result = 0;
    for (unsigned i = 0; i != width<U>; ++i) result += unsigned((x >> i) & 1u);
    return result;
  }

  template<class U> constexpr unsigned trailing(U x) {
    for (unsigned i = 0; i != width<U>; ++i) if ((x >> i) & 1u) return i;
    return width<U>;
  }

  template<class U> constexpr unsigned leading(U x) {
    for (unsigned i = 0; i != width<U>; ++i) if ((x >> (width<U> - 1 - i)) & 1u) return i;
    return width<U>;
  }

  template<class U> constexpr U extract(U x, unsigned start, unsigned length) {
    U result = 0;
    start %= 256;
    length %= 256;
    for (unsigned i = 0; i < length && start + i < width<U>; ++i)
      result |= U(((x >> (start + i)) & 1u) << i);
    return result;
  }

  template<class U> constexpr U deposit(U x, U mask) {
    U result = 0;
    unsigned source = 0;
    for (unsigned destination = 0; destination != width<U>; ++destination)
      if ((mask >> destination) & 1u) {
        result |= U(((x >> source) & 1u) << destination);
        ++source;
      }
    return result;
  }

  template<class U> constexpr U gather(U x, U mask) {
    U result = 0;
    unsigned destination = 0;
    for (unsigned source = 0; source != width<U>; ++source)
      if ((mask >> source) & 1u) {
        result |= U(((x >> source) & 1u) << destination);
        ++destination;
      }
    return result;
  }

  template<class U> constexpr std::array<U, 2> multiply(U a, U b) {
    U low = 0, high = 0;
    for (unsigned i = 0; i != width<U>; ++i) if ((b >> i) & 1u) {
      auto previous = low;
      low = U(low + U(a << i));
      high = U(high + U(i ? a >> (width<U> - i) : 0) + U(low < previous));
    }
    return {low, high};
  }

  template<class U> constexpr U rotate(U x, unsigned amount) {
    for (unsigned i = 0; i < amount % width<U>; ++i)
      x = U((x / 2) | U((x & 1u) << (width<U> - 1)));
    return x;
  }

  template<native::isa<native::x86> A, class U> consteval bool counts() {
    std::uint64_t state = 0x9e3779b97f4a7c15ull;
    for (unsigned i = 0; i != 96; ++i) {
      U x = i < width<U> ? U(U{1} << i) : U(random(state));
      if (native::popcnt<A>(x) != count(x) || native::tzcnt<A>(x) != trailing(x) ||
          native::lzcnt<A>(x) != leading(x)) return false;
    }
    return native::popcnt<A>(U{0}) == 0 && native::popcnt<A>(all<U>) == width<U> &&
      native::tzcnt<A>(U{0}) == width<U> && native::lzcnt<A>(U{0}) == width<U>;
  }

  template<native::isa<native::x86> A, class U> consteval bool bmi1_values() {
    std::uint64_t state = 0xc6a4a7935bd1e995ull;
    for (unsigned i = 0; i != 96; ++i) {
      U a = U(random(state)), b = U(random(state));
      if (i < 2) a = i ? all<U> : U{0};
      U lowest = a ? U(U{1} << trailing(a)) : U{0};
      U through = a ? U(all<U> >> (width<U> - 1 - trailing(a))) : all<U>;
      if (native::andn<A>(a, b) != U((all<U> ^ a) & b) ||
          native::blsi<A>(a) != lowest || native::blsmsk<A>(a) != through ||
          native::blsr<A>(a) != U(a ^ lowest)) return false;
    }
    return true;
  }

  template<native::isa<native::x86> A, class U> consteval bool extraction() {
    constexpr std::array<unsigned, 10> controls{0, 1, 15, 31, 32, 63, 64, 255, 256, ~0u};
    constexpr std::array<U, 5> values{0, 1, all<U>, U(U{1} << (width<U> - 1)), U(0xc7315a7d659ac2e3ull)};
    for (auto x : values) for (auto start : controls) for (auto length : controls) {
      auto expected = extract(x, start, length);
      auto packed = std::uint32_t{0xbeef0000u | (start & 255u) | ((length & 255u) << 8)};
      if (native::bextr<A>(x, start, length) != expected ||
          native::bextr<A>(x, packed) != expected) return false;
    }
    return true;
  }

  template<native::isa<native::x86> A, class U> consteval bool deposit_extract() {
    std::uint64_t state = 0xd6e8feb86659fd93ull;
    for (unsigned i = 0; i != 96; ++i) {
      U x = U(random(state)), mask = U(random(state));
      if (i < 2) mask = i ? all<U> : U{0};
      if (native::pdep<A>(x, mask) != deposit(x, mask) ||
          native::pext<A>(x, mask) != gather(x, mask)) return false;
    }
    return true;
  }

  template<native::isa<native::x86> A, class U> consteval bool products() {
    std::uint64_t state = 0x94d049bb133111ebull;
    constexpr std::array<U, 7> values{0, 1, 2, all<U>, U(all<U> - 1),
      U(U{1} << (width<U> / 2)), U(U{1} << (width<U> - 1))};
    for (auto a : values) for (auto b : values) {
      auto expected = multiply(a, b);
      U high = all<U>;
      auto low = native::mulx<A>(a, b, &high);
      if (low != expected[0] || high != expected[1]) return false;
      // An output may alias the caller's input storage; arguments are values.
      auto aliased = a;
      low = native::mulx<A>(aliased, b, &aliased);
      if (low != expected[0] || aliased != expected[1]) return false;
    }
    for (unsigned i = 0; i != 96; ++i) {
      U a = U(random(state)), b = U(random(state)), high = 0;
      auto expected = multiply(a, b);
      auto low = native::mulx<A>(a, b, &high);
      if (low != expected[0] || high != expected[1]) return false;
    }
    return true;
  }

  template<native::isa<native::x86> A, class U> consteval bool shifts() {
    using S = std::make_signed_t<U>;
    constexpr std::array<unsigned, 12> controls{0, 1, 15, 31, 32, 63, 64, 127, 255, 256, 257, ~0u};
    constexpr std::array<U, 5> values{0, 1, all<U>, U(U{1} << (width<U> - 1)), U(0xc7315a7d659ac2e3ull)};
    for (auto x : values) for (auto c : controls) {
      auto n = c % width<U>;
      U arithmetic = U(x >> n);
      if (n && (x >> (width<U> - 1))) arithmetic |= U(all<U> << (width<U> - n));
      auto zero_high = (c % 256) >= width<U> ? x : extract(x, 0, c);
      if (native::bzhi<A>(x, c) != zero_high || native::shlx<A>(x, c) != U(x << n) ||
          native::shrx<A>(x, c) != U(x >> n) ||
          std::bit_cast<U>(native::sarx<A>(std::bit_cast<S>(x), c)) != arithmetic) return false;
    }
    return true;
  }

  template<native::isa<native::x86> A, class U, unsigned I> consteval bool rotation() {
    constexpr std::array<U, 5> values{0, 1, all<U>, U(U{1} << (width<U> - 1)), U(0xc7315a7d659ac2e3ull)};
    for (auto x : values) if (native::rorx<A, I>(x) != rotate(x, I)) return false;
    return true;
  }

  constexpr std::uint32_t reverse(std::uint32_t x) {
    std::uint32_t result = 0;
    for (unsigned i = 0; i != 32; ++i) result = (result << 1) | ((x >> i) & 1u);
    return result;
  }

  // MSB-first polynomial division independently checks the reflected update.
  template<class U> constexpr std::uint32_t crc_oracle(std::uint32_t crc, U input) {
    crc = reverse(crc);
    for (unsigned bit = 0; bit != width<U>; ++bit) {
      bool feedback = ((crc >> 31) ^ ((input >> bit) & 1u)) != 0;
      crc = (crc << 1) ^ (feedback ? 0x1edc6f41u : 0u);
    }
    return reverse(crc);
  }

  template<native::isa<native::x86> A, class U> consteval bool crc_values() {
    std::uint64_t state = 0x853c49e6748fea9bull;
    for (unsigned i = 0; i != 96; ++i) {
      auto accumulator = static_cast<std::uint32_t>(random(state));
      U value = U(random(state));
      if (native::crc32c<A>(accumulator, value) != crc_oracle(accumulator, value)) return false;
    }
    return native::crc32c<A>(0, U{0}) == 0;
  }

  template<native::isa<native::x86> A> consteval bool crc_sequence() {
    std::uint32_t crc = ~0u;
    for (auto c : std::array<std::uint8_t, 9>{'1','2','3','4','5','6','7','8','9'})
      crc = native::crc32c<A>(crc, c);
    if (~crc != 0xe3069283u) return false;
    auto low = native::crc32c<A>(~0u, std::uint32_t{0x34333231});
    return native::crc32c<A>(low, std::uint32_t{0x38373635}) ==
      native::crc32c<A>(~0u, std::uint64_t{0x3837363534333231ull});
  }
  static_assert(counts<native::isa<native::x86>{}, std::uint16_t>());
  static_assert(counts<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(counts<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(bmi1_values<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(extraction<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(deposit_extract<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(products<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(shifts<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 0>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 1>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 31>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 32>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 63>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 64>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 127>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint32_t, 255>());
  static_assert(bmi1_values<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(extraction<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(deposit_extract<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(products<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(shifts<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 0>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 1>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 31>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 32>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 63>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 64>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 127>());
  static_assert(rotation<native::isa<native::x86>{}, std::uint64_t, 255>());
  static_assert(crc_values<native::isa<native::x86>{}, std::uint8_t>());
  static_assert(crc_values<native::isa<native::x86>{}, std::uint16_t>());
  static_assert(crc_values<native::isa<native::x86>{}, std::uint32_t>());
  static_assert(crc_values<native::isa<native::x86>{}, std::uint64_t>());
  static_assert(crc_sequence<native::isa<native::x86>{}>());
  static_assert(counts<strong, std::uint16_t>());
  static_assert(counts<strong, std::uint32_t>());
  static_assert(counts<strong, std::uint64_t>());
  static_assert(bmi1_values<strong, std::uint32_t>());
  static_assert(extraction<strong, std::uint32_t>());
  static_assert(deposit_extract<strong, std::uint32_t>());
  static_assert(products<strong, std::uint32_t>());
  static_assert(shifts<strong, std::uint32_t>());
  static_assert(rotation<strong, std::uint32_t, 0>());
  static_assert(rotation<strong, std::uint32_t, 1>());
  static_assert(rotation<strong, std::uint32_t, 31>());
  static_assert(rotation<strong, std::uint32_t, 32>());
  static_assert(rotation<strong, std::uint32_t, 63>());
  static_assert(rotation<strong, std::uint32_t, 64>());
  static_assert(rotation<strong, std::uint32_t, 127>());
  static_assert(rotation<strong, std::uint32_t, 255>());
  static_assert(bmi1_values<strong, std::uint64_t>());
  static_assert(extraction<strong, std::uint64_t>());
  static_assert(deposit_extract<strong, std::uint64_t>());
  static_assert(products<strong, std::uint64_t>());
  static_assert(shifts<strong, std::uint64_t>());
  static_assert(rotation<strong, std::uint64_t, 0>());
  static_assert(rotation<strong, std::uint64_t, 1>());
  static_assert(rotation<strong, std::uint64_t, 31>());
  static_assert(rotation<strong, std::uint64_t, 32>());
  static_assert(rotation<strong, std::uint64_t, 63>());
  static_assert(rotation<strong, std::uint64_t, 64>());
  static_assert(rotation<strong, std::uint64_t, 127>());
  static_assert(rotation<strong, std::uint64_t, 255>());
  static_assert(crc_values<strong, std::uint8_t>());
  static_assert(crc_values<strong, std::uint16_t>());
  static_assert(crc_values<strong, std::uint32_t>());
  static_assert(crc_values<strong, std::uint64_t>());
  static_assert(crc_sequence<strong>());
}
int main() { return 0; }
