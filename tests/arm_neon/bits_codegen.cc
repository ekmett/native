// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/neon.h>
import native.arm.neon;
constexpr auto architecture = native::neon & native::arm_feature::rdm;
template<class T, std::size_t N> using vector = native::simd<T, N, architecture>;

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_clz_s8_8(int8x8_t a) { return vclz_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_clz_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_clz_s8_16(int8x16_t a) { return vclzq_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_clz_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t raw_clz_s16_4(int16x4_t a) { return vclz_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t native_clz_s16_4(int16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 4>>(a);
  return native::detail::arm_neon::to_register<int16x4_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t raw_clz_s16_8(int16x8_t a) { return vclzq_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t native_clz_s16_8(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<int16x8_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t raw_clz_s32_2(int32x2_t a) { return vclz_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t native_clz_s32_2(int32x2_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 2>>(a);
  return native::detail::arm_neon::to_register<int32x2_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_clz_s32_4(int32x4_t a) { return vclzq_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_clz_s32_4(int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 4>>(a);
  return native::detail::arm_neon::to_register<int32x4_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t raw_clz_u8_8(uint8x8_t a) { return vclz_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t native_clz_u8_8(uint8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t raw_clz_u8_16(uint8x16_t a) { return vclzq_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t native_clz_u8_16(uint8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 16>>(a);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t raw_clz_u16_4(uint16x4_t a) { return vclz_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t native_clz_u16_4(uint16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t raw_clz_u16_8(uint16x8_t a) { return vclzq_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t native_clz_u16_8(uint16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x2_t raw_clz_u32_2(uint32x2_t a) { return vclz_u32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint32x2_t native_clz_u32_2(uint32x2_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint32_t, 2>>(a);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t raw_clz_u32_4(uint32x4_t a) { return vclzq_u32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t native_clz_u32_4(uint32x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint32_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::clz(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_cls_s8_8(int8x8_t a) { return vcls_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_cls_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_cls_s8_16(int8x16_t a) { return vclsq_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_cls_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t raw_cls_s16_4(int16x4_t a) { return vcls_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t native_cls_s16_4(int16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 4>>(a);
  return native::detail::arm_neon::to_register<int16x4_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t raw_cls_s16_8(int16x8_t a) { return vclsq_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t native_cls_s16_8(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<int16x8_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t raw_cls_s32_2(int32x2_t a) { return vcls_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t native_cls_s32_2(int32x2_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 2>>(a);
  return native::detail::arm_neon::to_register<int32x2_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_cls_s32_4(int32x4_t a) { return vclsq_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_cls_s32_4(int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 4>>(a);
  return native::detail::arm_neon::to_register<int32x4_t>(native::cls(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_rbit_s8_8(int8x8_t a) { return vrbit_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_rbit_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::rbit(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_rbit_s8_16(int8x16_t a) { return vrbitq_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_rbit_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::rbit(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t raw_rbit_u8_8(uint8x8_t a) { return vrbit_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t native_rbit_u8_8(uint8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::rbit(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t raw_rbit_u8_16(uint8x16_t a) { return vrbitq_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t native_rbit_u8_16(uint8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 16>>(a);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::rbit(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_rev16_s8_8(int8x8_t a) { return vrev16_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_rev16_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::rev16(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_rev16_s8_16(int8x16_t a) { return vrev16q_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_rev16_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::rev16(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t raw_rev16_u8_8(uint8x8_t a) { return vrev16_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t native_rev16_u8_8(uint8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::rev16(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t raw_rev16_u8_16(uint8x16_t a) { return vrev16q_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t native_rev16_u8_16(uint8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 16>>(a);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::rev16(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_rev32_s8_8(int8x8_t a) { return vrev32_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_rev32_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_rev32_s8_16(int8x16_t a) { return vrev32q_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_rev32_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t raw_rev32_s16_4(int16x4_t a) { return vrev32_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t native_rev32_s16_4(int16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 4>>(a);
  return native::detail::arm_neon::to_register<int16x4_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t raw_rev32_s16_8(int16x8_t a) { return vrev32q_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t native_rev32_s16_8(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<int16x8_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t raw_rev32_u8_8(uint8x8_t a) { return vrev32_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t native_rev32_u8_8(uint8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t raw_rev32_u8_16(uint8x16_t a) { return vrev32q_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t native_rev32_u8_16(uint8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 16>>(a);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t raw_rev32_u16_4(uint16x4_t a) { return vrev32_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t native_rev32_u16_4(uint16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t raw_rev32_u16_8(uint16x8_t a) { return vrev32q_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t native_rev32_u16_8(uint16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::rev32(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t raw_rev64_s8_8(int8x8_t a) { return vrev64_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x8_t native_rev64_s8_8(int8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 8>>(a);
  return native::detail::arm_neon::to_register<int8x8_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t raw_rev64_s8_16(int8x16_t a) { return vrev64q_s8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int8x16_t native_rev64_s8_16(int8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int8_t, 16>>(a);
  return native::detail::arm_neon::to_register<int8x16_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t raw_rev64_s16_4(int16x4_t a) { return vrev64_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x4_t native_rev64_s16_4(int16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 4>>(a);
  return native::detail::arm_neon::to_register<int16x4_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t raw_rev64_s16_8(int16x8_t a) { return vrev64q_s16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int16x8_t native_rev64_s16_8(int16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int16_t, 8>>(a);
  return native::detail::arm_neon::to_register<int16x8_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t raw_rev64_s32_2(int32x2_t a) { return vrev64_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x2_t native_rev64_s32_2(int32x2_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 2>>(a);
  return native::detail::arm_neon::to_register<int32x2_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t raw_rev64_s32_4(int32x4_t a) { return vrev64q_s32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
int32x4_t native_rev64_s32_4(int32x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::int32_t, 4>>(a);
  return native::detail::arm_neon::to_register<int32x4_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t raw_rev64_u8_8(uint8x8_t a) { return vrev64_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x8_t native_rev64_u8_8(uint8x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint8x8_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t raw_rev64_u8_16(uint8x16_t a) { return vrev64q_u8(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint8x16_t native_rev64_u8_16(uint8x16_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint8_t, 16>>(a);
  return native::detail::arm_neon::to_register<uint8x16_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t raw_rev64_u16_4(uint16x4_t a) { return vrev64_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x4_t native_rev64_u16_4(uint16x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint16x4_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t raw_rev64_u16_8(uint16x8_t a) { return vrev64q_u16(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint16x8_t native_rev64_u16_8(uint16x8_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint16_t, 8>>(a);
  return native::detail::arm_neon::to_register<uint16x8_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x2_t raw_rev64_u32_2(uint32x2_t a) { return vrev64_u32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint32x2_t native_rev64_u32_2(uint32x2_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint32_t, 2>>(a);
  return native::detail::arm_neon::to_register<uint32x2_t>(native::rev64(x));
}

extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t raw_rev64_u32_4(uint32x4_t a) { return vrev64q_u32(a); }
extern "C" __attribute__((noinline, used, target("neon")))
uint32x4_t native_rev64_u32_4(uint32x4_t a) {
  auto x = native::detail::arm_neon::from_register<vector<std::uint32_t, 4>>(a);
  return native::detail::arm_neon::to_register<uint32x4_t>(native::rev64(x));
}
