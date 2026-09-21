// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <wasm_simd128.h>

// This executable deliberately has no dependency on native. It checks the
// engine's raw instruction against the pinned core numerics definition.
// WebAssembly/spec ba9fd9f5c23e569201265d5bda6fb8dde18ad8c0,
// document/core/exec/numerics.rst, relaxed-ops and op-irelaxed-laneselect.
std::uint64_t state = 0x9e3779b97f4a7c15;
unsigned failures = 0;
// One R_laneselect applies across all widths, lanes and calls in the program.
unsigned modes = 3;

std::uint64_t random_bits() {
  state ^= state >> 12;
  state ^= state << 25;
  state ^= state >> 27;
  return state * 0x2545f4914f6cdd1d;
}

template<class U>
__attribute__((target("relaxed-simd")))
void check_selection() {
  constexpr auto width = 8 * sizeof(U);
  constexpr auto count = 16 / sizeof(U);
  std::array<U, count> left{}, right{}, mask{}, result{};
  for (unsigned trial = 0; trial < 256; ++trial) {
    for (unsigned i = 0; i < count; ++i) {
      left[i] = U(random_bits());
      right[i] = U(random_bits());
      mask[i] = trial == 0 ? U(0) : trial == 1 ? U(~U(0)) : U(random_bits());
      if constexpr (sizeof(U) == 2) {
        if (trial == 2) {
          left[i] = 0x1234;
          right[i] = 0xabcd;
          mask[i] = 0x8000;
        }
      }
    }
    auto a = wasm_v128_load(left.data());
    auto b = wasm_v128_load(right.data());
    auto m = wasm_v128_load(mask.data());
    v128_t output;
    if constexpr (sizeof(U) == 1)
      output = wasm_i8x16_relaxed_laneselect(a, b, m);
    else if constexpr (sizeof(U) == 2)
      output = wasm_i16x8_relaxed_laneselect(a, b, m);
    else if constexpr (sizeof(U) == 4)
      output = wasm_i32x4_relaxed_laneselect(a, b, m);
    else
      output = wasm_i64x2_relaxed_laneselect(a, b, m);
    wasm_v128_store(result.data(), output);
    for (unsigned i = 0; i < count; ++i) {
      auto bits = U((left[i] & mask[i]) | (right[i] & U(~mask[i])));
      auto lane = mask[i] >> (width - 1) ? left[i] : right[i];
      unsigned possible = 0;
      if (result[i] == bits) {
        possible |= 1;
      }
      if (result[i] == lane) {
        possible |= 2;
      }
      if (possible == 0) {
        if (failures < 12) {
          std::printf("i%zu laneselect case %u lane %u: a=%llx b=%llx mask=%llx "
                      "raw=%llx; bitselect=%llx whole-lane=%llx\n",
            width, trial, i, static_cast<unsigned long long>(left[i]),
            static_cast<unsigned long long>(right[i]), static_cast<unsigned long long>(mask[i]),
            static_cast<unsigned long long>(result[i]), static_cast<unsigned long long>(bits),
            static_cast<unsigned long long>(lane));
        }
        ++failures;
      }
      modes &= possible;
    }
  }
}

int main(int argc, char ** argv) {
  bool deterministic = argc == 2 && std::strcmp(argv[1], "--deterministic") == 0;
  if (argc != 1 && !deterministic) {
    std::puts("Usage: laneselect_engine [--deterministic]");
    return 2;
  }
  // The deterministic profile fixes R_laneselect = 0: bit selection.
  modes = deterministic ? 1 : 3;
  check_selection<std::uint8_t>();
  check_selection<std::uint16_t>();
  check_selection<std::uint32_t>();
  check_selection<std::uint64_t>();
  if (failures) {
    std::printf("%u raw engine lane-selection results violate the pinned core specification.\n",
      failures);
    return 1;
  }
  if (modes == 0) {
    std::puts(deterministic
      ? "Raw engine lane selection violates the pinned core deterministic bit-selection interpretation."
      : "Raw engine lane selection has no fixed interpretation across widths, lanes and calls.");
    return 1;
  }
  std::puts(deterministic
    ? "Raw engine lane selection agrees with the pinned core deterministic bit-selection interpretation."
    : "Raw engine lane selection agrees with one fixed pinned core interpretation.");
}
