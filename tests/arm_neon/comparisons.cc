// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/config.h>
#include <array>
#include <bit>
#include <cfenv>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "../core_regression/support/fp_environment.h"
#if NATIVE_COMPARISON_HEADER
#include <native/simd.h>
#else
import native.arm.neon;
#endif

#if NATIVE_HOST_NEON
namespace fixture {
  template<std::size_t N> using floats = native::simd<float, N, native::neon>;
  enum class operation { equal, unequal, less, greater, less_equal, greater_equal };

  // Integer-only oracle. Zero signs compare equal; NaNs are unordered. For
  // nonzero ordered values, reversing the negative encoding gives numeric order.
  constexpr bool expected(operation op, std::uint32_t a, std::uint32_t b) noexcept {
    bool const unordered = (a & 0x7fffffffu) > 0x7f800000u ||
      (b & 0x7fffffffu) > 0x7f800000u;
    bool const equal = !unordered && (a == b || ((a | b) & 0x7fffffffu) == 0);
    auto const key_a = a >> 31 ? ~a : a ^ 0x80000000u;
    auto const key_b = b >> 31 ? ~b : b ^ 0x80000000u;
    bool const less = !unordered && !equal && key_a < key_b;
    bool const greater = !unordered && !equal && key_a > key_b;
    switch (op) {
      case operation::equal: return equal;
      case operation::unequal: return !equal;
      case operation::less: return less;
      case operation::greater: return greater;
      case operation::less_equal: return less || equal;
      case operation::greater_equal: return greater || equal;
    }
    return false;
  }
  static_assert(expected(operation::equal, 0u, 0x80000000u));
  static_assert(expected(operation::less, 0xff800000u, 0xbf800000u));
  static_assert(expected(operation::greater, 0x7f800000u, 0x7f7fffffu));
  static_assert(expected(operation::unequal, 0x7fc00000u, 0x7fc00000u));
  static_assert(!expected(operation::less_equal, 0x7f800001u, 0u));

  template<operation Op, class V> constexpr auto compare(V a, V b) {
    if constexpr (Op == operation::equal) return a == b;
    else if constexpr (Op == operation::unequal) return a != b;
    else if constexpr (Op == operation::less) return a < b;
    else if constexpr (Op == operation::greater) return a > b;
    else if constexpr (Op == operation::less_equal) return a <= b;
    else return a >= b;
  }
  // Separate calls keep runtime operands opaque and exercise each public
  // comparison independently, even when one packet contains only NaNs.
  template<operation Op, class V> [[gnu::noinline]] auto runtime_compare(V a, V b) {
    return compare<Op>(a, b);
  }
  template<operation Op, class V> constexpr auto checked_compare(V a, V b) {
    if consteval { return compare<Op>(a, b); }
    else { return runtime_compare<Op>(a, b); }
  }

  constexpr std::array edge_words{
    0x00000000u, 0x80000000u, 0x00000001u, 0x80000001u,
    0x007fffffu, 0x807fffffu, 0x00800000u, 0x80800000u,
    0x3f7fffffu, 0xbf7fffffu, 0x3f800000u, 0xbf800000u,
    0x3f800001u, 0xbf800001u, 0x7f7fffffu, 0xff7fffffu,
    0x7f800000u, 0xff800000u, 0x7f800001u, 0xff800001u,
    0x7fbfffffu, 0xffbfffffu, 0x7fc00000u, 0xffc00000u,
    0x7fc12345u, 0xffc12345u, 0x7fffffffu, 0xffffffffu
  };

  [[noreturn]] void fail(operation op, std::size_t lanes, std::size_t lane,
      std::uint32_t a, std::uint32_t b, std::uint32_t got, std::uint32_t want) {
    std::fprintf(stderr,
      "comparison=%u lanes=%zu lane=%zu a=%08x b=%08x got=%08x expected=%08x rounding=%d\n",
      unsigned(op), lanes, lane, a, b, got, want, std::fegetround());
    std::abort();
  }

  template<operation Op, std::size_t N>
  constexpr bool check_one(std::array<std::uint32_t, N> const & a,
      std::array<std::uint32_t, N> const & b, floats<N> x, floats<N> y) {
    static_assert(std::same_as<decltype(compare<Op>(x, y)), typename floats<N>::mask_type>);
    auto const result = checked_compare<Op>(x, y);
    std::uint64_t bits = 0;
    for (std::size_t lane = 0; lane < N; ++lane)
      bits |= std::uint64_t(expected(Op, a[lane], b[lane])) << lane;
    // Inspect every physical lane too: true is all ones and padding stays zero.
    auto const words = std::bit_cast<std::array<std::uint32_t, 4>>(result.to_native());
    for (std::size_t lane = 0; lane < 4; ++lane) {
      auto const want = lane < N && ((bits >> lane) & 1u) ? 0xffffffffu : 0u;
      if (words[lane] != want) {
        if consteval { return false; }
        else { fail(Op, N, lane, lane < N ? a[lane] : 0u,
          lane < N ? b[lane] : 0u, words[lane], want); }
      }
    }
    if (result.to_bitset() != bits) {
      if consteval { return false; }
      else { fail(Op, N, 0, a[0], b[0], std::uint32_t(result.to_bitset()),
        std::uint32_t(bits)); }
    }
    return true;
  }
  template<std::size_t N>
  constexpr bool check_packet(std::array<std::uint32_t, N> const & a,
      std::array<std::uint32_t, N> const & b) {
    std::array<float, N> av{}, bv{};
    for (std::size_t lane = 0; lane < N; ++lane) {
      av[lane] = std::bit_cast<float>(a[lane]);
      bv[lane] = std::bit_cast<float>(b[lane]);
    }
    auto const x = floats<N>::load(av.data()), y = floats<N>::load(bv.data());
    return check_one<operation::equal>(a, b, x, y) &&
      check_one<operation::unequal>(a, b, x, y) &&
      check_one<operation::less>(a, b, x, y) &&
      check_one<operation::greater>(a, b, x, y) &&
      check_one<operation::less_equal>(a, b, x, y) &&
      check_one<operation::greater_equal>(a, b, x, y);
  }
  template<std::size_t N> consteval bool constant_edges() {
    for (std::size_t offset = 0; offset < edge_words.size(); ++offset)
      for (auto delta : {0u, 1u, 13u}) {
        std::array<std::uint32_t, N> a{}, b{};
        for (std::size_t lane = 0; lane < N; ++lane) {
          a[lane] = edge_words[(offset + lane) % edge_words.size()];
          b[lane] = edge_words[(offset + lane + delta) % edge_words.size()];
        }
        if (!check_packet(a, b)) return false;
      }
    return true;
  }
  static_assert(constant_edges<2>());
  static_assert(constant_edges<3>());
  static_assert(constant_edges<4>());

  std::uint32_t random_word(std::uint32_t & state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
  }
  template<std::size_t N> void runtime_shape() {
    std::array<std::uint32_t, N> a{}, b{};
    // All edge pairs occur in each logical lane, including equal NaNs, opposite
    // zero signs, opposite infinities, and mixed exceptional/ordinary packets.
    for (std::size_t i = 0; i < edge_words.size(); ++i)
      for (std::size_t j = 0; j < edge_words.size(); ++j) {
        for (std::size_t lane = 0; lane < N; ++lane) {
          a[lane] = edge_words[(i + lane) % edge_words.size()];
          b[lane] = edge_words[(j + lane) % edge_words.size()];
        }
        if (!check_packet(a, b)) std::abort();
      }
    std::uint32_t state = 0xe3791af5u;
    for (std::size_t packet = 0; packet < 10000; ++packet) {
      for (std::size_t lane = 0; lane < N; ++lane) {
        a[lane] = random_word(state);
        b[lane] = packet % 7 == 0 ? a[lane] : random_word(state);
      }
      if (!check_packet(a, b)) std::abort();
    }
  }

  template<operation Op, std::size_t N>
  void environment_packet(std::array<std::uint32_t, N> const & a,
      std::array<std::uint32_t, N> const & b, bool flush, std::uint64_t raised) {
    std::array<float, N> av{}, bv{};
    for (std::size_t lane = 0; lane < N; ++lane) {
      av[lane] = std::bit_cast<float>(a[lane]);
      bv[lane] = std::bit_cast<float>(b[lane]);
    }
    auto const x = floats<N>::load(av.data()), y = floats<N>::load(bv.data());
    auto const previous = native::test::read_fp_state();
    auto const control = flush ? previous.control | (1ull << 24)
                               : previous.control & ~(1ull << 24);
    // Seed IXC to check that comparisons preserve an unrelated sticky flag.
    // Inline compares between the writes/reads exercise ordering directly.
    constexpr std::uint64_t sticky = 1ull << 4;
    native::test::write_fp_state({control, sticky});
    auto const result = compare<Op>(x, y);
    auto const after = native::test::read_fp_state();
    if (after.control != control || after.status != (sticky | raised))
      fail(Op, N, 0, a[0], b[0], std::uint32_t(after.status),
        std::uint32_t(sticky | raised));
    auto const flushed = [flush](std::uint32_t word) {
      return flush && (word & 0x7fffffffu) < 0x00800000u ? word & 0x80000000u : word;
    };
    std::uint64_t bits = 0;
    for (std::size_t lane = 0; lane < N; ++lane)
      bits |= std::uint64_t(expected(Op, flushed(a[lane]), flushed(b[lane]))) << lane;
    if (result.to_bitset() != bits)
      fail(Op, N, 0, a[0], b[0], std::uint32_t(result.to_bitset()), std::uint32_t(bits));

    native::test::write_fp_state({control, sticky});
    (void)compare<Op>(x, y);
    auto const discarded = native::test::read_fp_state();
    if (discarded.control != control || discarded.status != (sticky | raised))
      fail(Op, N, 0, a[0], b[0], std::uint32_t(discarded.status),
        std::uint32_t(sticky | raised));
    native::test::write_fp_state(previous);
  }

  template<operation Op, std::size_t N> void environment_operation() {
    std::array<std::uint32_t, N> a{}, b{};
    for (auto nan : {0x7fc12345u, 0xffc12345u, 0x7f812345u, 0xff812345u})
      for (std::size_t lane = 0; lane < N; ++lane)
        for (bool right : {false, true}) {
          a.fill(0x3f800000u);
          b.fill(0x3f800000u);
          (right ? b : a)[lane] = nan;
          // FCMEQ is quiet for qNaNs; FCMGT/FCMGE signal all NaNs.
          // Every comparison signals sNaNs, including != through FCMEQ.
          bool const signaling = (nan & 0x00400000u) == 0;
          bool const ordered = Op != operation::equal && Op != operation::unequal;
          environment_packet<Op>(a, b, false, signaling || ordered ? 1u : 0u);
        }
    for (auto tiny : {0x00000001u, 0x80000001u, 0x007fffffu, 0x807fffffu}) {
      a.fill(tiny);
      b.fill(0u);
      // FZ changes a tiny input to signed zero and sets FPSR.IDC. Alternating
      // the mode in the same call site catches comparison reuse across FPCR.
      for (bool flush : {false, true, false})
        environment_packet<Op>(a, b, flush, flush ? 1u << 7 : 0u);
    }
  }
  template<std::size_t N> void environment_shape() {
    environment_operation<operation::equal, N>();
    environment_operation<operation::unequal, N>();
    environment_operation<operation::less, N>();
    environment_operation<operation::greater, N>();
    environment_operation<operation::less_equal, N>();
    environment_operation<operation::greater_equal, N>();
  }
}

int main() {
  auto const before = native::test::read_fp_state();
  {
    native::test::fp_scope scope(native::test::fp_mode::gradual);
    for (auto rounding : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
      if (std::fesetround(rounding)) return 2;
      auto const control = native::test::read_fp_state().control;
      fixture::runtime_shape<2>();
      fixture::runtime_shape<3>();
      fixture::runtime_shape<4>();
      fixture::environment_shape<2>();
      fixture::environment_shape<3>();
      fixture::environment_shape<4>();
      if (native::test::read_fp_state().control != control) return 3;
    }
  }
  if (native::test::read_fp_state() != before) return 4;
  std::puts("ARM comparisons: six operators, constexpr, padded2/3/full4 masks, edge cross-product, 10000 random packets per shape, four rounding modes, FPSR NaN flags, FPCR FZ ordering passed");
}
#else
int main() { return 77; }
#endif
