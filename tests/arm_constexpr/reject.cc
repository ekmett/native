// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.crc;
import native.arm.sha;
#if NATIVE_ARM_CONSTEXPR_REJECT < 8
std::uint32_t reject(std::uint32_t c,std::uint64_t word) {
#if NATIVE_ARM_CONSTEXPR_REJECT == 0
  return native::crc32<native::isa{}>(c,static_cast<std::uint8_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 1
  return native::crc32<native::isa{}>(c,static_cast<std::uint16_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 2
  return native::crc32<native::isa{}>(c,static_cast<std::uint32_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 3
  return native::crc32<native::isa{}>(c,word);
#elif NATIVE_ARM_CONSTEXPR_REJECT == 4
  return native::crc32c<native::isa{}>(c,static_cast<std::uint8_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 5
  return native::crc32c<native::isa{}>(c,static_cast<std::uint16_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 6
  return native::crc32c<native::isa{}>(c,static_cast<std::uint32_t>(word));
#else
  return native::crc32c<native::isa{}>(c,word);
#endif
}
#elif NATIVE_ARM_CONSTEXPR_REJECT == 8
std::uint32_t reject(std::uint32_t x) { return native::sha1h<native::isa{}>(x); }
#elif NATIVE_ARM_CONSTEXPR_REJECT == 9
std::uint32_t reject(std::uint32_t c,std::uint64_t word) {
  return native::crc32<native::isa(native::arm_feature::crc)>(c,word);
}
#else
std::uint32_t reject(std::uint32_t x) { return native::sha1h<native::isa(native::arm_feature::sha1)>(x); }
#endif
