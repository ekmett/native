// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Moved verbatim from the pre-module header; retained Catch test reference only.
#if defined(NATIVE_TESTING) || defined(NATIVE_TESTING_BF16)
TEST_CASE("bf16","[bf16]") {
  using namespace native;

  SECTION("bf16 default constructors and conversion") {
    CHECK(bf16(0.5f).content == Catch::Approx(0.5f));
    CHECK(0.25_bf16 .content == Catch::Approx(0.25f));
    CHECK(static_cast<float>(1.0_bf16) == Catch::Approx(1.0f));
  }

  SECTION("bf16 comparison operators") {
    CHECK(0.5_bf16 == 0.5_bf16);
    CHECK(0.1_bf16 < 0.2_bf16);
    CHECK(0.3_bf16 > 0.2_bf16);
    CHECK(0.2_bf16 <= 0.2_bf16);
    CHECK(0.2_bf16 >= 0.2_bf16);

    // Negative checks for inequality
    CHECK_FALSE(0.5_bf16 < 0.5_bf16);
    CHECK_FALSE(0.3_bf16 < 0.2_bf16);
    CHECK_FALSE(0.1_bf16 > 0.2_bf16);
    CHECK_FALSE(0.3_bf16 <= 0.2_bf16);
    CHECK_FALSE(0.1_bf16 >= 0.2_bf16);
  }

  SECTION("bf16 swap function") {
    bf16 x(1.0_bf16), y(2.0_bf16);
    swap(x, y);
    CHECK(x == 2.0_bf16);
    CHECK(y == 1.0_bf16);
  }

  SECTION("bf16 bit operations") {
    bf16 val = bf16::from_bits(0x3F00);
    CHECK(val.to_bits() == 0x3F00);
    CHECK(bf16::from_bits(0x7F80) == std::numeric_limits<bf16>::infinity());
  }

  SECTION("bf16 isnan function") {
    CHECK(std::isnan(bf16::from_bits(0x7FC0)));
    CHECK_FALSE(std::isnan(1.0_bf16));
  }

  SECTION("bf16 numeric_limits") {
    using nl = std::numeric_limits<bf16>;

    CHECK(nl::is_specialized);
    CHECK(nl::is_signed);
    CHECK(nl::digits == 8);
    CHECK(nl::epsilon() == bf16::from_bits(0x3C00));
    CHECK(nl::round_error() == bf16::from_bits(0x3F00));
    CHECK(nl::min() == bf16::from_bits(0x0080));
    CHECK(nl::max() == bf16::from_bits(0x7F7F));
    CHECK(nl::lowest() == bf16::from_bits(0xFF7F));
    CHECK(nl::infinity() == bf16::from_bits(0x7F80));
    CHECK(std::isnan(nl::quiet_NaN()));
    CHECK(std::isnan(nl::signaling_NaN()));
    CHECK(nl::denorm_min() == bf16::from_bits(1));
  }
}
#endif
