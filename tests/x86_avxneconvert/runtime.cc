// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
import native.x86.avxneconvert;
#include "checks.h"

template<std::size_t N>
[[gnu::target("avxneconvert"), gnu::noinline]]
bool run_case(unsigned index, std::uint32_t & state, bool exhaustive) {
  checks::packet<N> p;
  // Exactly 2-byte aligned operands, with the full 2*N memory footprint.
  alignas(64) std::array<native::bf16, 2 * N + 1> b{};
  alignas(64) std::array<native::fp16, 2 * N + 1> h{};
  std::array<float, N> f{};
  for (unsigned i = 0; i < 2 * N; ++i) {
    auto word = exhaustive   ? std::uint16_t(index + i)
                : index < 16 ? checks::half_corners[(index + i) % 16]
                             : std::uint16_t(reference::random(state));
    p.source[i] = word;
    b[i + 1] = native::bf16::from_bits(word);
    h[i + 1] = native::fp16::from_bits(word);
  }
  for (unsigned i = 0; i < N; ++i) {
    auto word = index < 16 ? checks::float_corners[(index + i) % 16] : reference::random(state);
    p.input[i] = word;
    f[i] = std::bit_cast<float>(word);
  }
  auto save = [&](unsigned i, auto v) {
    std::array<float, N + 2> x{};
    x.front() = 123.f;
    x.back() = -123.f;
    v.store(x.data() + 1);
    if (x.front() != 123.f || x.back() != -123.f)
      return false;
    for (unsigned j = 0; j < N; ++j)
      p.widened[i][j] = std::bit_cast<std::uint32_t>(x[j + 1]);
    return true;
  };
  constexpr auto a = checks::strong;
  bool valid = save(0, native::bcstnebf16_ps<a, N>(b.data() + 1)) &&
               save(1, native::bcstnesh_ps<a, N>(h.data() + 1)) &&
               save(2, native::cvtneebf16_ps<a, N>(b.data() + 1)) &&
               save(3, native::cvtneeph_ps<a, N>(h.data() + 1)) &&
               save(4, native::cvtneobf16_ps<a, N>(b.data() + 1)) &&
               save(5, native::cvtneoph_ps<a, N>(h.data() + 1));
  auto r = native::cvtneps_bf16(native::simd<float, N, a>::load(f.data()));
  std::array<std::uint16_t, N + 2> output{};
  output.front() = 0xdead;
  output.back() = 0xbeef;
  r.store_bits(output.data() + 1);
  for (unsigned i = 0; i < N; ++i)
    p.narrowed[i] = output[i + 1];
  valid &= output.front() == 0xdead && output.back() == 0xbeef;
  if constexpr (N == 4) {
    auto register_bits = std::bit_cast<std::array<std::uint16_t, 8>>(r.to_native());
    for (unsigned i = 4; i < 8; ++i)
      valid &= register_bits[i] == 0;
  }
  return valid && checks::verify(p);
}
int main() {
  if (!native::classify_isa(native::observe_x86_capabilities(), checks::strong).admitted()) {
    std::puts("AVX-NE-CONVERT runtime skipped: CPU/OS requirements are unavailable.");
    return 77;
  }
  auto saved = _mm_getcsr();
  std::uint32_t state = 0x5739812u;
  bool valid = true;
  // All rounding modes and DAZ/FTZ combinations, with sticky flags already set.
  for (unsigned controls = 0; controls < 16 && valid; ++controls) {
    auto csr = 0x1f80u | 0x15u | ((controls & 3u) << 13) | ((controls & 4u) << 4) |
               ((controls & 8u) << 12);
    _mm_setcsr(csr);
    for (unsigned i = 0; i < 2064 && valid; ++i)
      valid = run_case<4>(i, state, false) && run_case<8>(i, state, false);
    valid &= _mm_getcsr() == csr;
  }
  _mm_setcsr(0x1f80);
  for (unsigned i = 0; i < 65536 && valid; ++i)
    valid = run_case<4>(i, state, true) && run_case<8>(i, state, true);
  valid &= _mm_getcsr() == 0x1f80;
  _mm_setcsr(saved);
  if (!valid)
    std::puts("AVX-NE-CONVERT semantics, memory guards, padding or MXCSR check failed.");
  return valid ? 0 : 1;
}
