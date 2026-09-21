// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.arm.crc;
import native.arm.sha;
import native.arm.pmull;
#if NATIVE_ARM_CONSTEXPR_REJECT < 8
std::uint32_t reject(std::uint32_t c,std::uint64_t word) {
#if NATIVE_ARM_CONSTEXPR_REJECT == 0
  return native::crc32<native::isa<native::arm>{}>(c,static_cast<std::uint8_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 1
  return native::crc32<native::isa<native::arm>{}>(c,static_cast<std::uint16_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 2
  return native::crc32<native::isa<native::arm>{}>(c,static_cast<std::uint32_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 3
  return native::crc32<native::isa<native::arm>{}>(c,word);
#elif NATIVE_ARM_CONSTEXPR_REJECT == 4
  return native::crc32c<native::isa<native::arm>{}>(c,static_cast<std::uint8_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 5
  return native::crc32c<native::isa<native::arm>{}>(c,static_cast<std::uint16_t>(word));
#elif NATIVE_ARM_CONSTEXPR_REJECT == 6
  return native::crc32c<native::isa<native::arm>{}>(c,static_cast<std::uint32_t>(word));
#else
  return native::crc32c<native::isa<native::arm>{}>(c,word);
#endif
}
#elif NATIVE_ARM_CONSTEXPR_REJECT == 8
std::uint32_t reject(std::uint32_t x) { return native::sha1h<native::isa<native::arm>{}>(x); }
#elif NATIVE_ARM_CONSTEXPR_REJECT == 9
std::uint32_t reject(std::uint32_t c,std::uint64_t word) {
  return native::crc32<native::isa<native::arm>(native::arm_feature::crc)>(c,word);
}
#elif NATIVE_ARM_CONSTEXPR_REJECT == 10
std::uint32_t reject(std::uint32_t x) { return native::sha1h<native::isa<native::arm>(native::arm_feature::sha1)>(x); }
#endif

#if NATIVE_ARM_CONSTEXPR_REJECT == 11
auto reject(std::uint64_t a,std::uint64_t b) { return native::pmull<native::neon>(a,b); }
#elif NATIVE_ARM_CONSTEXPR_REJECT == 12
auto reject(std::uint64_t a,std::uint64_t b) {
  return native::pmull<native::feature_closure(native::arm_feature::pmull)>(a,b);
}
#endif

// A foreign family must not become an empty ARM requirement, even for constants.
#if NATIVE_ARM_CONSTEXPR_REJECT >= 13
#if NATIVE_ARM_CONSTEXPR_REJECT < 16
constexpr native::isa<native::x86> foreign{};
#else
constexpr native::isa<native::wasm> foreign{};
#endif
#if NATIVE_ARM_CONSTEXPR_REJECT == 13 || NATIVE_ARM_CONSTEXPR_REJECT == 16
constexpr auto rejected=native::crc32<foreign>(std::uint32_t{},std::uint8_t{1});
#elif NATIVE_ARM_CONSTEXPR_REJECT == 14 || NATIVE_ARM_CONSTEXPR_REJECT == 17
constexpr auto rejected=native::sha1h<foreign>(std::uint32_t{1});
#else
constexpr auto rejected=native::pmull<foreign>(std::uint64_t{1},std::uint64_t{1});
#endif
#endif
