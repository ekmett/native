// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.crc;
import native.arm.rdm;
import native.arm.pmull;
import native.arm.sha;
void reject() {
#if NATIVE_ARM_DEFAULT_REJECT == 0
  (void)native::crc32(std::uint32_t{},std::uint64_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 1
  (void)native::sqrdmlah(std::int16_t{},std::int16_t{},std::int16_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 2
  (void)native::sha1h(std::uint32_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 3
  (void)native::pmull(std::uint64_t{},std::uint64_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 4
  (void)native::crc32c(std::uint32_t{},std::int32_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 5
  (void)native::sqrdmlsh(std::int16_t{},std::int32_t{},std::int16_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 6
  (void)native::sha1h(std::uint64_t{});
#elif NATIVE_ARM_DEFAULT_REJECT == 7
  (void)native::pmull(std::uint32_t{},std::uint64_t{});
#endif
}
