// SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Module-only counterparts of the former embedded half/numerics checks.
import simd.numerics;
using namespace simd;

static_assert(one_of<4, 2, 4, 8> && not_one_of<3, 2, 4, 8>);
static_assert(one_of_t<int, int, double> && not_one_of_t<char, int, double>);
static_assert(sizeof(int_t<float>) == sizeof(float));
static_assert(sizeof(uint_t<double>) == sizeof(double));
static_assert(imm<4>.value == 4);
static_assert(static_cast<float>(0.25_fp16) == 0.25f);
static_assert(static_cast<float>(0.25_bf16) == 0.25f);
static_assert(0.5_fp16 == fp16(0.5f) && 0.5_bf16 == bf16(0.5f));
static_assert(fp16::from_bits(0x3c00).to_bits() == 0x3c00);
static_assert(bf16::from_bits(0x3f80).to_bits() == 0x3f80);
static_assert(std::numeric_limits<fp16>::min().to_bits() == 0x0400);
static_assert(std::numeric_limits<bf16>::min().to_bits() == 0x0080);
static_assert(std::isnan(fp16::from_bits(0x7fff)));
static_assert(std::isnan(bf16::from_bits(0x7fc0)));
static_assert(!std::isnan(fp16::from_bits(0x7c00)));
static_assert(!std::isnan(bf16::from_bits(0x7f80)));
static_assert(scalef(2.0f, 3.0f) == 16.0f);
static_assert(scalef(4.0, -2.0) == 1.0);
static_assert(cmpint<CMPINT::EQ>(5, 5) && cmpint<CMPINT::LT>(-1, 0));
static_assert(cmp<CMP::EQ_OQ>(1.0f, 1.0f));

template<class T> bool half_api() {
  T zero{};
  if (zero.to_bits() != 0) return false;
  T a(0.5f), b(1.5f);
  auto copied = a;
  auto assigned = b;
  assigned = copied;
  if (assigned != a) return false;
  swap(a, b);
  if (static_cast<float>(a) != 1.5f || static_cast<float>(b) != 0.5f) return false;
  return a > b && b < a && a >= b && b <= a && a != b && !(a == b);
}

int main() {
  if (!half_api<fp16>() || !half_api<bf16>()) return 1;
  if (!cmp_unord(std::numeric_limits<fp16>::quiet_NaN(), 1.0_fp16)) return 2;
  if (!cmp_unord(1.0_bf16, std::numeric_limits<bf16>::quiet_NaN())) return 3;
  if (!cmp_ord(0.5_fp16, 1.5_fp16) || !cmp_ord(0.5_bf16, 1.5_bf16)) return 4;
  volatile float runtime = 1.00390625f;
  // Preserve the historical explicit fast helper's runtime truncation.
  if (fast_to_bf16(runtime).to_bits() != 0x3f80) return 5;
  return 0;
}
