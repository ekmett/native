// SPDX-FileCopyrightText: 2024-2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Scalar numerics contracts through the public module.
#include <cstdint>
#include <utility>
import native.numerics;
using namespace native;

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

// Call through pointers so these constexpr constants also get runtime coverage.
// Check stored NaN encodings without quieting them through float conversion.
template<class T>
bool half_limits(std::uint16_t const (&expected)[9]) noexcept {
  using limits = std::numeric_limits<T>;
  T (*volatile values[])() noexcept = {
    limits::min, limits::max, limits::lowest, limits::epsilon,
    limits::round_error, limits::infinity, limits::quiet_NaN,
    limits::signaling_NaN, limits::denorm_min
  };
  for (unsigned i = 0; i != 9; ++i)
    if (values[i]().to_bits() != expected[i]) return false;
  return true;
}

// Four possible relations: less, equal, greater and unordered. The mask for
// each immediate is its truth table, independent of the implementation branches.
template<class T, std::size_t... I>
bool floating_predicates(T a, T b, unsigned relation, std::index_sequence<I...>) {
  constexpr unsigned masks[]{2,1,3,8,13,14,12,7,10,9,11,0,5,6,4,15};
  return ((cmp<static_cast<CMP>(I)>(a,b) == bool(masks[I%16] & relation)) && ...);
}
template<class T, std::size_t... I>
bool integer_predicates(T a, T b, unsigned relation, std::index_sequence<I...>) {
  constexpr unsigned masks[]{2,1,3,0,5,6,4,7};
  return ((cmpint<static_cast<CMPINT>(I)>(a,b) == bool(masks[I] & relation)) && ...);
}
template<class T> bool floating_truth_tables() {
  volatile T inputs[]{T(-1),T(1),T(0),-T(0),std::numeric_limits<T>::infinity(),
    std::numeric_limits<T>::quiet_NaN()};
  struct pair { unsigned a,b,relation; };
  for(auto p : {pair{0,1,1},pair{1,0,4},pair{2,3,2},pair{4,4,2},
      pair{1,4,1},pair{5,1,8},pair{1,5,8},pair{5,5,8}})
    if(!floating_predicates(T(inputs[p.a]),T(inputs[p.b]),p.relation,std::make_index_sequence<32>{})) return false;
  return true;
}

int main() {
  if (!half_api<fp16>() || !half_api<bf16>()) return 1;
  if (!cmp_unord(std::numeric_limits<fp16>::quiet_NaN(), 1.0_fp16)) return 2;
  if (!cmp_unord(1.0_bf16, std::numeric_limits<bf16>::quiet_NaN())) return 3;
  if (!cmp_ord(0.5_fp16, 1.5_fp16) || !cmp_ord(0.5_bf16, 1.5_bf16)) return 4;
  volatile float runtime = 1.00390625f;
  // Preserve the historical explicit fast helper's runtime truncation.
  if (fast_to_bf16(runtime).to_bits() != 0x3f80) return 5;
  if (!floating_truth_tables<float>() || !floating_truth_tables<double>()) return 6;
  volatile std::int32_t low = std::numeric_limits<std::int32_t>::min();
  volatile std::int32_t high = std::numeric_limits<std::int32_t>::max();
  if (!integer_predicates(std::int32_t(low),std::int32_t(high),1,std::make_index_sequence<8>{}) ||
      !integer_predicates(std::int32_t(high),std::int32_t(low),4,std::make_index_sequence<8>{}) ||
      !integer_predicates(std::int32_t(low),std::int32_t(low),2,std::make_index_sequence<8>{})) return 7;
  volatile std::uint32_t upper = std::numeric_limits<std::uint32_t>::max();
  if (!integer_predicates(std::uint32_t(upper),std::uint32_t(0),4,std::make_index_sequence<8>{})) return 8;
  if (!half_limits<fp16>({0x0400, 0x7bff, 0xfbff, 0x1400, 0x3800,
      0x7c00, 0x7fff, 0x7dff, 0x0001})) return 9;
  if (!half_limits<bf16>({0x0080, 0x7f7f, 0xff7f, 0x3c00, 0x3f00,
      0x7f80, 0x7fc0, 0x7f81, 0x0001})) return 10;
  // Explicit literal-operator calls prevent constant evaluation hiding the body.
  volatile long double literal = 0.25L;
  if (operator""_fp16(literal).to_bits() != 0x3400 ||
      operator""_bf16(literal).to_bits() != 0x3e80) return 11;
  return 0;
}
