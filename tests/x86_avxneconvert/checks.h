// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "reference.h"
namespace checks {
  constexpr auto strong = native::target_features<native::x86>("avxneconvert");
  constexpr auto weak = native::target_features<native::x86>("avx");
  constexpr std::array<std::uint16_t, 16> half_corners{
      0,      0x8000, 1,      0x8001, 0x03ff, 0x0400, 0x7bff, 0x7c00,
      0xfc00, 0x7c01, 0xfc01, 0x7fff, 0x007f, 0x0080, 0x7f80, 0x7f81};
  constexpr std::array<std::uint32_t, 16> float_corners{
      0,          0x80000000, 1,          0x80000001, 0x007fffff, 0x807fffff,
      0x00800000, 0x7f800000, 0xff800000, 0x7f800001, 0xff800001, 0x3f808000,
      0x3f818000, 0x7f7fffff, 0x3f807fff, 0x3f808001};
  template<std::size_t N> struct packet {
    std::array<std::uint16_t, 2 * N> source{};
    std::array<std::uint32_t, N> input{};
    std::array<std::array<std::uint32_t, N>, 6> widened{};
    std::array<std::uint16_t, N> narrowed{};
    constexpr bool operator==(packet const &) const = default;
  };
  template<native::isa<native::x86> A, std::size_t N, unsigned Case>
  consteval packet<N> constants() {
    packet<N> p;
    std::array<native::bf16, 2 * N> b{};
    std::array<native::fp16, 2 * N> h{};
    std::array<float, N> f{};
    std::uint32_t state = 0x89739861u + Case;
    for (unsigned i = 0; i < 2 * N; ++i) {
      auto word =
          Case < 2 ? half_corners[(i + Case * N) % 16] : std::uint16_t(reference::random(state));
      p.source[i] = word;
      b[i] = native::bf16::from_bits(word);
      h[i] = native::fp16::from_bits(word);
    }
    for (unsigned i = 0; i < N; ++i) {
      auto word = Case < 2 ? float_corners[(i + Case * N) % 16] : reference::random(state);
      p.input[i] = word;
      f[i] = std::bit_cast<float>(word);
    }
    auto save = [&](unsigned i, auto v) {
      std::array<float, N> x{};
      v.store(x.data());
      p.widened[i] = std::bit_cast<std::array<std::uint32_t, N>>(x);
    };
    save(0, native::bcstnebf16_ps<A, N>(b.data()));
    save(1, native::bcstnesh_ps<A, N>(h.data()));
    save(2, native::cvtneebf16_ps<A, N>(b.data()));
    save(3, native::cvtneeph_ps<A, N>(h.data()));
    save(4, native::cvtneobf16_ps<A, N>(b.data()));
    save(5, native::cvtneoph_ps<A, N>(h.data()));
    native::cvtneps_bf16(native::simd<float, N, A>::load(f.data())).store_bits(p.narrowed.data());
    return p;
  }
  template<std::size_t N> constexpr bool verify(packet<N> const & p) {
    for (unsigned i = 0; i < N; ++i) {
      if (p.widened[0][i] != (std::uint32_t(p.source[0]) << 16) ||
          p.widened[1][i] != reference::half(p.source[0]) ||
          p.widened[2][i] != (std::uint32_t(p.source[2 * i]) << 16) ||
          p.widened[3][i] != reference::half(p.source[2 * i]) ||
          p.widened[4][i] != (std::uint32_t(p.source[2 * i + 1]) << 16) ||
          p.widened[5][i] != reference::half(p.source[2 * i + 1]) ||
          p.narrowed[i] != reference::narrow(p.input[i]))
        return false;
    }
    return true;
  }
  template<unsigned Case> constexpr bool constant_case() {
    constexpr auto a = constants<strong, 4, Case>(), b = constants<weak, 4, Case>();
    constexpr auto c = constants<strong, 8, Case>(), d = constants<weak, 8, Case>();
    static_assert(a == b && c == d);
    return verify(a) && verify(c);
  }
} // namespace checks
