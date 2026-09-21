// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <immintrin.h>
#include <native/attributes.h>

extern "C" native_noinline native_target("adx")
std::uint8_t native_addcarryx_32(std::uint8_t carry, std::uint32_t a, std::uint32_t b, std::uint32_t * result) noexcept {
  unsigned int sum;
  auto next = _addcarryx_u32(carry, a, b, &sum);
  *result = sum;
  return next;
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_chain_32(std::uint32_t const * a, std::uint32_t const * b, std::uint32_t * result) noexcept {
  std::uint8_t carry = 0;
  unsigned int sum0;
  carry = _addcarryx_u32(carry, a[0], b[0], &sum0);
  result[0] = sum0;
  unsigned int sum1;
  carry = _addcarryx_u32(carry, a[1], b[1], &sum1);
  result[1] = sum1;
  unsigned int sum2;
  carry = _addcarryx_u32(carry, a[2], b[2], &sum2);
  result[2] = sum2;
  unsigned int sum3;
  carry = _addcarryx_u32(carry, a[3], b[3], &sum3);
  result[3] = sum3;
  return carry;
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_addcarryx_64(std::uint8_t carry, std::uint64_t a, std::uint64_t b, std::uint64_t * result) noexcept {
  unsigned long long sum;
  auto next = _addcarryx_u64(carry, a, b, &sum);
  *result = sum;
  return next;
}

extern "C" native_noinline native_target("adx")
std::uint8_t native_chain_64(std::uint64_t const * a, std::uint64_t const * b, std::uint64_t * result) noexcept {
  std::uint8_t carry = 0;
  unsigned long long sum0;
  carry = _addcarryx_u64(carry, a[0], b[0], &sum0);
  result[0] = sum0;
  unsigned long long sum1;
  carry = _addcarryx_u64(carry, a[1], b[1], &sum1);
  result[1] = sum1;
  unsigned long long sum2;
  carry = _addcarryx_u64(carry, a[2], b[2], &sum2);
  result[2] = sum2;
  unsigned long long sum3;
  carry = _addcarryx_u64(carry, a[3], b[3], &sum3);
  result[3] = sum3;
  return carry;
}

extern "C" native_noinline unsigned native_adx_baseline(unsigned a, unsigned b) noexcept {
  return (a ^ b) + a;
}
