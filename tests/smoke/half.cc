// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <bit>
#include <cmath>
#include <compare>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <utility>
import native.numerics;

#define CHECK(condition) do { if (!(condition)) { \
  std::printf("half storage check failed at line %d: %s\n", __LINE__, #condition); return 1; \
} } while (false)

int check_fp16() noexcept {
  using native::fp16;
  using limits = std::numeric_limits<fp16>;
  // Exact results around zero, subnormal ties, normal ties and overflow.
  struct encoding { std::uint32_t input; std::uint16_t expected; };
  constexpr encoding encodings[]{
    {0x00000000, 0x0000}, {0x80000000, 0x8000},
    {0x32ffffff, 0x0000}, {0x33000000, 0x0000}, {0x33000001, 0x0001},
    {0x33800000, 0x0001}, {0x33c00000, 0x0002}, {0x33c00001, 0x0002},
    {0x387fc000, 0x03ff}, {0x38800000, 0x0400},
    {0x3f800000, 0x3c00}, {0x3f801000, 0x3c00}, {0x3f803000, 0x3c02},
    {0xbf800000, 0xbc00}, {0x477fefff, 0x7bff}, {0x477ff000, 0x7c00},
    {0x47800000, 0x7c00}, {0xc7800000, 0xfc00},
    {0x7f800000, 0x7c00}, {0xff800000, 0xfc00},
    {0x7f800001, 0x7e00}, {0x7fcaa000, 0x7e55}
  };
  for (auto const & c : encodings) {
    volatile auto input = c.input;
    CHECK(fp16(std::bit_cast<float>(std::uint32_t(input))).to_bits() == c.expected);
  }
  struct decoding { std::uint16_t input; std::uint32_t expected; };
  constexpr decoding decodings[]{
    {0x0000, 0x00000000}, {0x8000, 0x80000000}, {0x0001, 0x33800000},
    {0x03ff, 0x387fc000}, {0x0400, 0x38800000}, {0x3c00, 0x3f800000},
    {0xbc00, 0xbf800000}, {0x7bff, 0x477fe000}, {0x7c00, 0x7f800000},
    {0xfc00, 0xff800000}, {0x7c01, 0x7fc02000}, {0xfe55, 0xffcaa000}
  };
  for (auto const & c : decodings) {
    volatile auto input = c.input;
    auto value = fp16::from_bits(input);
    CHECK(std::bit_cast<std::uint32_t>(static_cast<float>(value)) == c.expected);
    // Native-half projections transport bits, including signaling NaNs.
    if constexpr (!std::is_integral_v<fp16::underlying_type>) {
      auto storage = static_cast<fp16::underlying_type>(value);
      CHECK(fp16(storage).to_bits() == c.input);
    }
  }

  volatile std::uint16_t words[]{0x3c00, 0x4000, 0x8000, 0x7c01};
  auto one = fp16::from_bits(words[0]), two = fp16::from_bits(words[1]);
  auto negative_zero = fp16::from_bits(words[2]), nan = fp16::from_bits(words[3]);
  CHECK(one == one && !(one == two) && one != two && !(one != one));
  CHECK(one < two && !(two < one) && one <= one && !(two <= one));
  CHECK(two > one && !(one > two) && two >= two && !(one >= two));
  CHECK((one <=> two) == std::partial_ordering::less);
  CHECK((two <=> one) == std::partial_ordering::greater);
  CHECK((one <=> one) == std::partial_ordering::equivalent);
  CHECK((nan <=> one) == std::partial_ordering::unordered);
  CHECK(!(nan == one) && nan != one && !(nan < one) && !(nan <= one));
  CHECK(!(nan > one) && !(nan >= one) && std::isnan(nan) && !std::isnan(one));
  fp16 zero{};
  CHECK(zero.to_bits() == 0 && zero == negative_zero);
  auto copy = one;
  auto moved = std::move(copy);
  copy = two;
  moved = std::move(copy);
  CHECK(moved.to_bits() == 0x4000 && copy.to_bits() == 0x4000);
  swap(one, two);
  CHECK(one.to_bits() == 0x4000 && two.to_bits() == 0x3c00);
  using native::operator""_fp16;
  CHECK((1.5_fp16).to_bits() == 0x3e00);
  CHECK(limits::min().to_bits() == 0x0400);
  CHECK(limits::max().to_bits() == 0x7bff);
  CHECK(limits::lowest().to_bits() == 0xfbff);
  CHECK(limits::epsilon().to_bits() == 0x1400);
  CHECK(limits::round_error().to_bits() == 0x3800);
  CHECK(limits::infinity().to_bits() == 0x7c00);
  CHECK(limits::quiet_NaN().to_bits() == 0x7fff);
  CHECK(limits::signaling_NaN().to_bits() == 0x7dff);
  CHECK(limits::denorm_min().to_bits() == 1);
  return 0;
}

int check_bf16() noexcept {
  using native::bf16;
  using limits = std::numeric_limits<bf16>;
  // Exact results around zero, subnormal ties, normal ties and overflow.
  struct encoding { std::uint32_t input; std::uint16_t expected; };
  constexpr encoding encodings[]{
    {0x00000000, 0x0000}, {0x80000000, 0x8000},
    {0x00007fff, 0x0000}, {0x00008000, 0x0000}, {0x00008001, 0x0001},
    {0x00010000, 0x0001}, {0x00018000, 0x0002},
    {0x007f0000, 0x007f}, {0x007f8000, 0x0080}, {0x00800000, 0x0080},
    {0x3f800000, 0x3f80}, {0x3f808000, 0x3f80}, {0x3f818000, 0x3f82},
    {0xbf800000, 0xbf80}, {0x7f7f7fff, 0x7f7f}, {0x7f7f8000, 0x7f80},
    {0xff7f8000, 0xff80}, {0x7f800000, 0x7f80}, {0xff800000, 0xff80},
    {0x7f800001, 0x7fc0}, {0x7faa0000, 0x7fea}
  };
  for (auto const & c : encodings) {
    volatile auto input = c.input;
    CHECK(bf16(std::bit_cast<float>(std::uint32_t(input))).to_bits() == c.expected);
  }
  struct decoding { std::uint16_t input; std::uint32_t expected; };
  constexpr decoding decodings[]{
    {0x0000, 0x00000000}, {0x8000, 0x80000000}, {0x0001, 0x00010000},
    {0x007f, 0x007f0000}, {0x0080, 0x00800000}, {0x3f80, 0x3f800000},
    {0xbf80, 0xbf800000}, {0x7f7f, 0x7f7f0000}, {0x7f80, 0x7f800000},
    {0xff80, 0xff800000}, {0x7f81, 0x7fc10000}, {0xffea, 0xffea0000}
  };
  for (auto const & c : decodings) {
    volatile auto input = c.input;
    auto value = bf16::from_bits(input);
    CHECK(std::bit_cast<std::uint32_t>(static_cast<float>(value)) == c.expected);
    // Native-half projections transport bits, including signaling NaNs.
    if constexpr (!std::is_integral_v<bf16::underlying_type>) {
      auto storage = static_cast<bf16::underlying_type>(value);
      CHECK(bf16(storage).to_bits() == c.input);
    }
  }

  volatile std::uint16_t words[]{0x3f80, 0x4000, 0x8000, 0x7f81};
  auto one = bf16::from_bits(words[0]), two = bf16::from_bits(words[1]);
  auto negative_zero = bf16::from_bits(words[2]), nan = bf16::from_bits(words[3]);
  CHECK(one == one && !(one == two) && one != two && !(one != one));
  CHECK(one < two && !(two < one) && one <= one && !(two <= one));
  CHECK(two > one && !(one > two) && two >= two && !(one >= two));
  CHECK((one <=> two) == std::partial_ordering::less);
  CHECK((two <=> one) == std::partial_ordering::greater);
  CHECK((one <=> one) == std::partial_ordering::equivalent);
  CHECK((nan <=> one) == std::partial_ordering::unordered);
  CHECK(!(nan == one) && nan != one && !(nan < one) && !(nan <= one));
  CHECK(!(nan > one) && !(nan >= one) && std::isnan(nan) && !std::isnan(one));
  bf16 zero{};
  CHECK(zero.to_bits() == 0 && zero == negative_zero);
  auto copy = one;
  auto moved = std::move(copy);
  copy = two;
  moved = std::move(copy);
  CHECK(moved.to_bits() == 0x4000 && copy.to_bits() == 0x4000);
  swap(one, two);
  CHECK(one.to_bits() == 0x4000 && two.to_bits() == 0x3f80);
  using native::operator""_bf16;
  CHECK((1.5_bf16).to_bits() == 0x3fc0);
  CHECK(limits::min().to_bits() == 0x0080);
  CHECK(limits::max().to_bits() == 0x7f7f);
  CHECK(limits::lowest().to_bits() == 0xff7f);
  CHECK(limits::epsilon().to_bits() == 0x3c00);
  CHECK(limits::round_error().to_bits() == 0x3f00);
  CHECK(limits::infinity().to_bits() == 0x7f80);
  CHECK(limits::quiet_NaN().to_bits() == 0x7fc0);
  CHECK(limits::signaling_NaN().to_bits() == 0x7f81);
  CHECK(limits::denorm_min().to_bits() == 1);
  // The fast runtime overload truncates; constant evaluation rounds.
  volatile float halfway = std::bit_cast<float>(0x3f818000u);
  CHECK(native::fast_to_bf16(halfway).to_bits() == 0x3f81);
  CHECK(native::fast_to_bf16(one).to_bits() == one.to_bits());
  static_assert(native::fast_to_bf16(std::bit_cast<float>(0x3f818000u)).to_bits() == 0x3f82);
  return 0;
}

int main() noexcept {
  if (auto result = check_fp16()) return result;
  return check_bf16();
}
