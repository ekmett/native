// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
import native;
import native.math;

static_assert(std::same_as<decltype(math::exp(0.f)), float>);
static_assert(std::same_as<decltype(math::exp(std::array<float, 2>{})), std::array<float, 2>>);

int main() {
  auto exponentials = math::exp(std::array{0.f, 0.f});
  if (exponentials != std::array{1.f, 1.f}) return 1;
  auto [sine, cosine] = math::sincos(0.f);
  return sine == 0.f && cosine == 1.f ? 0 : 2;
}
