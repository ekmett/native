// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.wide;
bool value_without_headers() {
  simd::wide input{1.0f,-0.0f};
  auto finite=simd::isfinite(input);auto inf=simd::isinf(input);auto nan=simd::isnan(input);auto sign=simd::signbit(input);
  auto copied=simd::copysign(input,simd::wide{-1.0f,1.0f});
  return finite.registers[0] && finite.registers[1] && !inf.registers[0] && !nan.registers[0] &&
    !sign.registers[0] && sign.registers[1] && copied.registers[0]==-1.0f && !simd::signbit(copied).registers[1];
}
