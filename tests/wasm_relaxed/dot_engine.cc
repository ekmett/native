// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <wasm_simd128.h>

// This engine conformance probe deliberately has no library/module dependency.
// Exact source: WebAssembly/spec ba9fd9f5c23e569201265d5bda6fb8dde18ad8c0,
// document/core/exec/numerics.rst, op-vextbinop, op-vextternop, relaxed-ops.
extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t raw_dot(v128_t a, v128_t b) {
  return wasm_i16x8_relaxed_dot_i8x16_i7x16(a, b);
}

extern "C" __attribute__((target("relaxed-simd"), noinline))
v128_t raw_dot_add(v128_t a, v128_t b, v128_t c) {
  return wasm_i32x4_relaxed_dot_i8x16_i7x16_add(a, b, c);
}

volatile std::int8_t first[2]{-128, 1};
volatile std::uint8_t second[2]{128, 255};

__attribute__((target("relaxed-simd")))
bool engine_check(bool deterministic) {
  // R_idot is shared by both operations and fixed for the entire program.
  // The deterministic profile requires R_idot = 0: signed, saturated pairs.
  unsigned modes = deterministic ? 1 : 3;
  for (unsigned trial = 0; trial < 2; ++trial) {
    auto a = first[trial];
    auto b = second[trial];
    std::array<std::int16_t, 8> dot{};
    std::array<std::int32_t, 4> added{};
    auto av = wasm_i8x16_splat(a);
    auto bv = wasm_i8x16_splat(static_cast<std::int8_t>(b));
    wasm_v128_store(dot.data(), raw_dot(av, bv));
    wasm_v128_store(added.data(), raw_dot_add(av, bv, wasm_i32x4_splat(0)));
    int signed_sum = 2 * int(a) * int(static_cast<std::int8_t>(b));
    int unsigned_sum = 2 * int(a) * int(b);
    auto saturate = [](int x) { return x > 32767 ? 32767 : x < -32768 ? -32768 : x; };
    int signed_pair = saturate(signed_sum);
    int unsigned_pair = saturate(unsigned_sum);
    unsigned possible = 3;
    for (auto value : dot) {
      if (value != signed_pair) {
        possible &= ~1u;
      }
      if (value != unsigned_pair) {
        possible &= ~2u;
      }
    }
    for (auto value : added) {
      if (value != 2 * signed_pair) {
        possible &= ~1u;
      }
      if (value != 2 * unsigned_pair) {
        possible &= ~2u;
      }
    }
    std::printf("a=%d b=%u: raw dot=%d, raw dot_add=%d; signed=(%d,%d), unsigned=(%d,%d), modes=%u\n",
      int(a), unsigned(b), int(dot[0]), int(added[0]), signed_pair, 2 * signed_pair,
      unsigned_pair, 2 * unsigned_pair, possible);
    modes &= possible;
  }
  if (modes == 0) {
    std::puts(deterministic
      ? "Raw engine result violates the pinned core deterministic signed, saturated dot interpretation."
      : "Raw engine result violates the pinned core specification's fixed saturated dot interpretation.");
  }
  return modes != 0;
}

int main(int argc, char ** argv) {
  bool deterministic = argc == 2 && std::strcmp(argv[1], "--deterministic") == 0;
  if (argc != 1 && !deterministic) {
    std::puts("Usage: dot_engine [--deterministic]");
    return 2;
  }
  return engine_check(deterministic) ? 0 : 1;
}
