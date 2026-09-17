// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <cfenv>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <limits>
#include <type_traits>
import simd.numerics;
std::uint32_t omnibus_half_bits();

// This bank exercises the target's actual nearest-even, gradual environment.
// It neither emulates nor claims unsupported rounding/FTZ control modes.
bool environment() {
  volatile float tiny = std::bit_cast<float>(std::uint32_t(1));
  volatile float one = 1.0f, half_ulp = 0x1p-24f;
  return std::fegetround() == FE_TONEAREST &&
    std::bit_cast<std::uint32_t>(float(tiny + tiny)) == 2 &&
    std::bit_cast<std::uint32_t>(float(one + half_ulp)) == 0x3f800000;
}
bool nan32(std::uint32_t word) { return (word & 0x7fffffffu) > 0x7f800000u; }
template<unsigned Fraction, int Bias>
std::uint32_t reference_decode(unsigned word) {
  auto sign = (word & 0x8000u) << 16;
  auto exponent = (word & 0x7fffu) >> Fraction;
  auto fraction = word & ((1u << Fraction) - 1u);
  auto top = 0x7fffu >> Fraction;
  if (exponent == top) return sign | (fraction ? 0x7fc00000u : 0x7f800000u);
  double significand = exponent ? (1u << Fraction) + fraction : fraction;
  int power = (exponent ? int(exponent) - Bias : 1 - Bias) - int(Fraction);
  return sign | std::bit_cast<std::uint32_t>(float(std::ldexp(significand, power)));
}
template<class T> concept integer_projection = requires(T v) { v.operator std::uint16_t(); };
template<class T> constexpr bool traits() {
  static_assert(sizeof(T) == 2 && alignof(T) == 2);
  static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>);
  static_assert(!integer_projection<T>);
  // Integer construction retains the existing conversion through float, never raw bits.
  if constexpr (std::integral<typename T::underlying_type>)
    static_assert(T(std::uint16_t(1)).to_bits() == T(1.0f).to_bits());
  return true;
}
static_assert(traits<simd::fp16>() && traits<simd::bf16>());
static_assert(simd::fp16(0x1.002p0f).to_bits() == 0x3c00);
static_assert(simd::fp16(0x1.006p0f).to_bits() == 0x3c02);
static_assert(simd::bf16(0x1.01p0f).to_bits() == 0x3f80);
static_assert(simd::bf16(0x1.03p0f).to_bits() == 0x3f82);
static_assert(std::numeric_limits<simd::fp16>::min().to_bits() == 0x0400);
static_assert(std::numeric_limits<simd::bf16>::min().to_bits() == 0x0080);
#if defined(__wasm__)
static_assert(std::same_as<simd::fp16::underlying_type, std::uint16_t>);
static_assert(std::same_as<simd::bf16::underlying_type, std::uint16_t>);
#elif defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
static_assert(std::same_as<simd::fp16::underlying_type, _Float16>);
static_assert(std::same_as<simd::bf16::underlying_type, __bf16>);
#endif

template<class T, unsigned Fraction, int Bias> bool check() {
  unsigned infinity = (0x7fffu >> Fraction) << Fraction;
  for (unsigned word = 0; word < 65536; ++word) {
    volatile std::uint16_t input = std::uint16_t(word);
    auto value = T::from_bits(input);
    if (value.to_bits() != word) return false;
    if constexpr (!std::integral<typename T::underlying_type>) {
      auto native = static_cast<typename T::underlying_type>(value);
      if (T(native).to_bits() != word) return false;
    }
    auto actual = std::bit_cast<std::uint32_t>(static_cast<float>(value));
    auto expected = reference_decode<Fraction,Bias>(word);
    if (actual != expected && !(nan32(actual) && nan32(expected))) return false;
    if (!nan32(actual) && T(std::bit_cast<float>(actual)).to_bits() != word) return false;
    bool unordered = nan32(actual);
    if ((value == value) == unordered || (value != value) != unordered ||
        (value <= value) == unordered || (value >= value) == unordered ||
        (value < value) || (value > value)) return false;
    auto order = value <=> value;
    if (unordered ? order != std::partial_ordering::unordered : order != 0) return false;
  }
  unsigned boundaries = 0;
  auto encode = [](std::uint32_t word, unsigned expected) {
    volatile std::uint32_t supplied = word;
    return T(std::bit_cast<float>(std::uint32_t(supplied))).to_bits() == expected;
  };
  for (unsigned word = 0; word + 1 < infinity; ++word) {
    double a = std::bit_cast<float>(reference_decode<Fraction,Bias>(word));
    double b = std::bit_cast<float>(reference_decode<Fraction,Bias>(word+1));
    auto midpoint = std::bit_cast<std::uint32_t>(float((a+b)*0.5));
    for (unsigned sign : {0u, 0x8000u}) {
      if (!encode((sign<<16)|(midpoint-1), sign|word) ||
          !encode((sign<<16)|midpoint, sign|(word+(word&1u))) ||
          !encode((sign<<16)|(midpoint+1), sign|(word+1))) return false;
      boundaries += 3;
    }
  }
  auto overflow = Fraction == 10 ? 0x477ff000u : 0x7f7f8000u;
  for (unsigned sign : {0u, 0x8000u}) {
    if (!encode((sign<<16)|(overflow-1), sign|(infinity-1)) ||
        !encode((sign<<16)|overflow, sign|infinity) ||
        !encode((sign<<16)|(overflow+1), sign|infinity)) return false;
    boundaries += 3;
  }
  for (auto word : {0x7f800001u, 0x7fc00000u, 0xffa12345u}) {
    volatile std::uint32_t supplied = word;
    auto value = T(std::bit_cast<float>(std::uint32_t(supplied)));
    if (!nan32(std::bit_cast<std::uint32_t>(static_cast<float>(value)))) return false;
  }
  std::printf("fraction=%u decode/storage=65536 encode_boundaries=%u nearest-even gradual native=%d\n",
    Fraction, boundaries, !std::integral<typename T::underlying_type>);
  return true;
}
int main() {
  if (!environment()) { std::fputs("requires nearest-even gradual binary32\n", stderr); return 1; }
  if (!check<simd::fp16,10,15>()) return 2;
  if (!check<simd::bf16,7,127>()) return 3;
  return omnibus_half_bits() == 0x3c003f80u ? 0 : 4;
}
