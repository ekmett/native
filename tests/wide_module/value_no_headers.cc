// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.wide;
bool value_without_headers() {
  native::wide input{1.0f,-0.0f};
  auto finite=native::isfinite(input);auto inf=native::isinf(input);auto nan=native::isnan(input);auto sign=native::signbit(input);
  auto copied=native::copysign(input,native::wide{-1.0f,1.0f});
  return finite.registers[0] && finite.registers[1] && !inf.registers[0] && !nan.registers[0] &&
    !sign.registers[0] && sign.registers[1] && copied.registers[0]==-1.0f && !native::signbit(copied).registers[1];
}
