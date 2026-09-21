// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/crc.h>
constexpr auto requirements=native::target_features<native::arm>("crc");
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32_8(std::uint32_t a,std::uint8_t b) {return native::crc32<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32_16(std::uint32_t a,std::uint16_t b) {return native::crc32<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32_32(std::uint32_t a,std::uint32_t b) {return native::crc32<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32_64(std::uint32_t a,std::uint64_t b) {return native::crc32<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32c_8(std::uint32_t a,std::uint8_t b) {return native::crc32c<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32c_16(std::uint32_t a,std::uint16_t b) {return native::crc32c<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32c_32(std::uint32_t a,std::uint32_t b) {return native::crc32c<requirements>(a,b);}
extern "C" native_noinline native_target("crc")
std::uint32_t native_crc32c_64(std::uint32_t a,std::uint64_t b) {return native::crc32c<requirements>(a,b);}
extern "C" native_noinline unsigned native_crc_baseline(unsigned a) {return a+1;}
