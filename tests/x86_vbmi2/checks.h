// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <new>
#include <native/attributes.h>
#include "support/guarded_pages.h"

inline constexpr auto full_width = native::target_features<native::x86>("avx512vbmi2");
inline constexpr auto short_width = native::target_features<native::x86>("avx512vbmi2,avx512vl");
inline constexpr auto broad = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2");
inline constexpr auto sse2_storage = native::target_features<native::x86>("sse2");
inline constexpr auto avx_storage = native::target_features<native::x86>("avx");
inline constexpr auto avx512_storage = native::target_features<native::x86>("avx512f");

template<class T, std::size_t N>
struct inputs {
  std::array<T, N> a{}, b{}, counts{}, source{};
};

template<class T, std::size_t N>
struct outputs {
  std::array<std::array<T, N>, 18> vectors{};
  std::array<T, N + 2> memory{};
  constexpr bool operator==(outputs const &) const = default;
};

template<class T, std::size_t N>
constexpr inputs<T, N> pattern(unsigned selector) {
  inputs<T, N> result;
  for (std::size_t lane = 0; lane < N; ++lane) {
    result.a[lane] = static_cast<T>(0x8000000080008001ull ^ (0x0123456789abcdefull * lane));
    result.b[lane] = static_cast<T>(0xfedcba9876543210ull + lane);
    result.counts[lane] = static_cast<T>(selector + lane * 17);
    result.source[lane] = static_cast<T>(0xa5a5a5a5a5a5a5a5ull + lane);
  }
  return result;
}

// Model the concatenated lanes by one-bit transfers, independently of the
// implementation's pair of wide shifts and OR. Counts wrap at the lane width.
template<class T>
constexpr T shift_reference(T a, T b, unsigned count, bool right) {
  constexpr unsigned bits = sizeof(T) * 8;
  for (unsigned step = 0; step < count % bits; ++step) {
    if (right) {
      a = static_cast<T>((std::uint64_t(a) >> 1) | ((std::uint64_t(b) & 1) << (bits - 1)));
      b = static_cast<T>(std::uint64_t(b) >> 1);
    } else {
      a = static_cast<T>((std::uint64_t(a) << 1) | (std::uint64_t(b) >> (bits - 1)));
      b = static_cast<T>(std::uint64_t(b) << 1);
    }
  }
  return a;
}

template<unsigned Imm8, class T, std::size_t N>
constexpr outputs<T, N> reference(inputs<T, N> const & input, std::uint64_t mask) {
  outputs<T, N> result;
  result.memory.fill(T(0x5a));
  if constexpr (sizeof(T) <= 2) {
    result.vectors[0] = input.source;
    result.vectors[2] = input.source;
    result.vectors[4] = input.source;
    // Rank is the number of selected lanes preceding this lane, not a mask
    // applied to the compressed destination. It can cross every 128-bit block.
    for (std::size_t lane = 0; lane < N; ++lane) {
      if ((mask >> lane) & 1) {
        auto preceding = lane ? mask & ((std::uint64_t{1} << lane) - 1) : 0;
        auto rank = static_cast<std::size_t>(std::popcount(preceding));
        result.vectors[0][rank] = input.a[lane];
        result.vectors[1][rank] = input.a[lane];
        result.vectors[2][lane] = input.a[rank];
        result.vectors[3][lane] = input.a[rank];
        result.vectors[4][lane] = input.a[rank];
        result.vectors[5][lane] = input.a[rank];
        result.memory[1 + rank] = input.a[lane];
      }
    }
  }
  if constexpr (sizeof(T) >= 2) {
    constexpr unsigned offset = sizeof(T) == 2 ? 6 : 0;
    for (std::size_t lane = 0; lane < N; ++lane) {
      bool active = (mask >> lane) & 1;
      for (unsigned direction = 0; direction < 2; ++direction) {
        auto immediate = shift_reference(input.a[lane], input.b[lane], Imm8, direction);
        auto variable = shift_reference(input.a[lane], input.b[lane], unsigned(input.counts[lane]), direction);
        unsigned index = offset + direction * 6;
        result.vectors[index][lane] = immediate;
        result.vectors[index + 1][lane] = active ? immediate : input.source[lane];
        result.vectors[index + 2][lane] = active ? immediate : 0;
        result.vectors[index + 3][lane] = variable;
        result.vectors[index + 4][lane] = active ? variable : input.a[lane];
        result.vectors[index + 5][lane] = active ? variable : 0;
      }
    }
  }
  return result;
}

template<native::isa<native::x86> A, unsigned Imm8, class T, std::size_t N>
consteval auto calculate(inputs<T, N> input, std::uint64_t bits) {
  using vector_type = native::simd<T, N, A>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto counts = vector_type::load(input.counts.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, A>::from_bitset(bits);
  outputs<T, N> result;
  result.memory.fill(T(0x5a));
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<A>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressb<A>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandb<A>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandb<A>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandb<A>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandb<A>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressb<A>(result.memory.data() + 1, mask, a);
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<A>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressw<A>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandw<A>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandw<A>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandw<A>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandw<A>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressw<A>(result.memory.data() + 1, mask, a);
    native::vpshldw<A, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshldw<A, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshldw<A, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshldvw<A>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshldvw<A>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshldvw<A>(mask, a, b, counts).store(result.vectors[11].data());
    native::vpshrdw<A, Imm8>(a, b).store(result.vectors[12].data());
    native::mask_vpshrdw<A, Imm8>(source, mask, a, b).store(result.vectors[13].data());
    native::maskz_vpshrdw<A, Imm8>(mask, a, b).store(result.vectors[14].data());
    native::vpshrdvw<A>(a, b, counts).store(result.vectors[15].data());
    native::mask_vpshrdvw<A>(a, mask, b, counts).store(result.vectors[16].data());
    native::maskz_vpshrdvw<A>(mask, a, b, counts).store(result.vectors[17].data());
  }
  if constexpr (sizeof(T) == 4) {
    native::vpshldd<A, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldd<A, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldd<A, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvd<A>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvd<A>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvd<A>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdd<A, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdd<A, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdd<A, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvd<A>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvd<A>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvd<A>(mask, a, b, counts).store(result.vectors[11].data());
  }
  if constexpr (sizeof(T) == 8) {
    native::vpshldq<A, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldq<A, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldq<A, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvq<A>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvq<A>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvq<A>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdq<A, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdq<A, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdq<A, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvq<A>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvq<A>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvq<A>(mask, a, b, counts).store(result.vectors[11].data());
  }
  return result;
}

template<native::isa<native::x86> A, class T, std::size_t N, unsigned Start>
consteval bool constants() {
  for (unsigned selector = Start; selector < Start + 64; ++selector) {
    auto input = pattern<T, N>(selector);
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{1} << (N - 1)}) {
      if (calculate<A, 7>(input, mask) != reference<7>(input, mask)) {
        return false;
      }
    }
  }
  return true;
}

template<native::isa<native::x86> A, class T, std::size_t N, unsigned Imm8>
consteval bool immediate_constants() {
  auto input = pattern<T, N>(255);
  for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                    std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{1} << (N - 1)}) {
    if (calculate<A, Imm8>(input, mask) != reference<Imm8>(input, mask) ||
        calculate<A, Imm8>(input, ~mask) != reference<Imm8>(input, ~mask)) {
      return false;
    }
  }
  return true;
}
static_assert(constants<sse2_storage, std::uint8_t, 16, 0>());
static_assert(constants<sse2_storage, std::uint8_t, 16, 64>());
static_assert(constants<sse2_storage, std::uint8_t, 16, 128>());
static_assert(constants<sse2_storage, std::uint8_t, 16, 192>());
static_assert(constants<sse2_storage, std::uint16_t, 8, 0>());
static_assert(constants<sse2_storage, std::uint16_t, 8, 64>());
static_assert(constants<sse2_storage, std::uint16_t, 8, 128>());
static_assert(constants<sse2_storage, std::uint16_t, 8, 192>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 0>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 1>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 15>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 16>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 31>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 32>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 63>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 64>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 127>());
static_assert(immediate_constants<sse2_storage, std::uint16_t, 8, 255>());
static_assert(constants<sse2_storage, std::uint32_t, 4, 0>());
static_assert(constants<sse2_storage, std::uint32_t, 4, 64>());
static_assert(constants<sse2_storage, std::uint32_t, 4, 128>());
static_assert(constants<sse2_storage, std::uint32_t, 4, 192>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 0>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 1>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 15>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 16>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 31>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 32>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 63>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 64>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 127>());
static_assert(immediate_constants<sse2_storage, std::uint32_t, 4, 255>());
static_assert(constants<sse2_storage, std::uint64_t, 2, 0>());
static_assert(constants<sse2_storage, std::uint64_t, 2, 64>());
static_assert(constants<sse2_storage, std::uint64_t, 2, 128>());
static_assert(constants<sse2_storage, std::uint64_t, 2, 192>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 0>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 1>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 15>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 16>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 31>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 32>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 63>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 64>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 127>());
static_assert(immediate_constants<sse2_storage, std::uint64_t, 2, 255>());

static_assert(constants<short_width, std::uint8_t, 16, 0>());
static_assert(constants<short_width, std::uint8_t, 16, 64>());
static_assert(constants<short_width, std::uint8_t, 16, 128>());
static_assert(constants<short_width, std::uint8_t, 16, 192>());
static_assert(constants<short_width, std::uint16_t, 8, 0>());
static_assert(constants<short_width, std::uint16_t, 8, 64>());
static_assert(constants<short_width, std::uint16_t, 8, 128>());
static_assert(constants<short_width, std::uint16_t, 8, 192>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 0>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 1>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 15>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 16>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 31>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 32>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 63>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 64>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 127>());
static_assert(immediate_constants<short_width, std::uint16_t, 8, 255>());
static_assert(constants<short_width, std::uint32_t, 4, 0>());
static_assert(constants<short_width, std::uint32_t, 4, 64>());
static_assert(constants<short_width, std::uint32_t, 4, 128>());
static_assert(constants<short_width, std::uint32_t, 4, 192>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 0>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 1>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 15>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 16>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 31>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 32>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 63>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 64>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 127>());
static_assert(immediate_constants<short_width, std::uint32_t, 4, 255>());
static_assert(constants<short_width, std::uint64_t, 2, 0>());
static_assert(constants<short_width, std::uint64_t, 2, 64>());
static_assert(constants<short_width, std::uint64_t, 2, 128>());
static_assert(constants<short_width, std::uint64_t, 2, 192>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 0>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 1>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 15>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 16>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 31>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 32>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 63>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 64>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 127>());
static_assert(immediate_constants<short_width, std::uint64_t, 2, 255>());

static_assert(constants<broad, std::uint8_t, 16, 0>());
static_assert(constants<broad, std::uint8_t, 16, 64>());
static_assert(constants<broad, std::uint8_t, 16, 128>());
static_assert(constants<broad, std::uint8_t, 16, 192>());
static_assert(constants<broad, std::uint16_t, 8, 0>());
static_assert(constants<broad, std::uint16_t, 8, 64>());
static_assert(constants<broad, std::uint16_t, 8, 128>());
static_assert(constants<broad, std::uint16_t, 8, 192>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 0>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 1>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 15>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 16>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 31>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 32>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 63>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 64>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 127>());
static_assert(immediate_constants<broad, std::uint16_t, 8, 255>());
static_assert(constants<broad, std::uint32_t, 4, 0>());
static_assert(constants<broad, std::uint32_t, 4, 64>());
static_assert(constants<broad, std::uint32_t, 4, 128>());
static_assert(constants<broad, std::uint32_t, 4, 192>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 0>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 1>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 15>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 16>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 31>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 32>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 63>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 64>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 127>());
static_assert(immediate_constants<broad, std::uint32_t, 4, 255>());
static_assert(constants<broad, std::uint64_t, 2, 0>());
static_assert(constants<broad, std::uint64_t, 2, 64>());
static_assert(constants<broad, std::uint64_t, 2, 128>());
static_assert(constants<broad, std::uint64_t, 2, 192>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 0>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 1>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 15>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 16>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 31>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 32>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 63>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 64>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 127>());
static_assert(immediate_constants<broad, std::uint64_t, 2, 255>());

static_assert(constants<avx_storage, std::uint8_t, 32, 0>());
static_assert(constants<avx_storage, std::uint8_t, 32, 64>());
static_assert(constants<avx_storage, std::uint8_t, 32, 128>());
static_assert(constants<avx_storage, std::uint8_t, 32, 192>());
static_assert(constants<avx_storage, std::uint16_t, 16, 0>());
static_assert(constants<avx_storage, std::uint16_t, 16, 64>());
static_assert(constants<avx_storage, std::uint16_t, 16, 128>());
static_assert(constants<avx_storage, std::uint16_t, 16, 192>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 0>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 1>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 15>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 16>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 31>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 32>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 63>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 64>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 127>());
static_assert(immediate_constants<avx_storage, std::uint16_t, 16, 255>());
static_assert(constants<avx_storage, std::uint32_t, 8, 0>());
static_assert(constants<avx_storage, std::uint32_t, 8, 64>());
static_assert(constants<avx_storage, std::uint32_t, 8, 128>());
static_assert(constants<avx_storage, std::uint32_t, 8, 192>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 0>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 1>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 15>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 16>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 31>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 32>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 63>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 64>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 127>());
static_assert(immediate_constants<avx_storage, std::uint32_t, 8, 255>());
static_assert(constants<avx_storage, std::uint64_t, 4, 0>());
static_assert(constants<avx_storage, std::uint64_t, 4, 64>());
static_assert(constants<avx_storage, std::uint64_t, 4, 128>());
static_assert(constants<avx_storage, std::uint64_t, 4, 192>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 0>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 1>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 15>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 16>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 31>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 32>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 63>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 64>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 127>());
static_assert(immediate_constants<avx_storage, std::uint64_t, 4, 255>());

static_assert(constants<short_width, std::uint8_t, 32, 0>());
static_assert(constants<short_width, std::uint8_t, 32, 64>());
static_assert(constants<short_width, std::uint8_t, 32, 128>());
static_assert(constants<short_width, std::uint8_t, 32, 192>());
static_assert(constants<short_width, std::uint16_t, 16, 0>());
static_assert(constants<short_width, std::uint16_t, 16, 64>());
static_assert(constants<short_width, std::uint16_t, 16, 128>());
static_assert(constants<short_width, std::uint16_t, 16, 192>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 0>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 1>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 15>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 16>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 31>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 32>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 63>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 64>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 127>());
static_assert(immediate_constants<short_width, std::uint16_t, 16, 255>());
static_assert(constants<short_width, std::uint32_t, 8, 0>());
static_assert(constants<short_width, std::uint32_t, 8, 64>());
static_assert(constants<short_width, std::uint32_t, 8, 128>());
static_assert(constants<short_width, std::uint32_t, 8, 192>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 0>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 1>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 15>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 16>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 31>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 32>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 63>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 64>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 127>());
static_assert(immediate_constants<short_width, std::uint32_t, 8, 255>());
static_assert(constants<short_width, std::uint64_t, 4, 0>());
static_assert(constants<short_width, std::uint64_t, 4, 64>());
static_assert(constants<short_width, std::uint64_t, 4, 128>());
static_assert(constants<short_width, std::uint64_t, 4, 192>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 0>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 1>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 15>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 16>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 31>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 32>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 63>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 64>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 127>());
static_assert(immediate_constants<short_width, std::uint64_t, 4, 255>());

static_assert(constants<broad, std::uint8_t, 32, 0>());
static_assert(constants<broad, std::uint8_t, 32, 64>());
static_assert(constants<broad, std::uint8_t, 32, 128>());
static_assert(constants<broad, std::uint8_t, 32, 192>());
static_assert(constants<broad, std::uint16_t, 16, 0>());
static_assert(constants<broad, std::uint16_t, 16, 64>());
static_assert(constants<broad, std::uint16_t, 16, 128>());
static_assert(constants<broad, std::uint16_t, 16, 192>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 0>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 1>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 15>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 16>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 31>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 32>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 63>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 64>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 127>());
static_assert(immediate_constants<broad, std::uint16_t, 16, 255>());
static_assert(constants<broad, std::uint32_t, 8, 0>());
static_assert(constants<broad, std::uint32_t, 8, 64>());
static_assert(constants<broad, std::uint32_t, 8, 128>());
static_assert(constants<broad, std::uint32_t, 8, 192>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 0>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 1>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 15>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 16>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 31>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 32>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 63>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 64>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 127>());
static_assert(immediate_constants<broad, std::uint32_t, 8, 255>());
static_assert(constants<broad, std::uint64_t, 4, 0>());
static_assert(constants<broad, std::uint64_t, 4, 64>());
static_assert(constants<broad, std::uint64_t, 4, 128>());
static_assert(constants<broad, std::uint64_t, 4, 192>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 0>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 1>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 15>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 16>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 31>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 32>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 63>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 64>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 127>());
static_assert(immediate_constants<broad, std::uint64_t, 4, 255>());

static_assert(constants<avx512_storage, std::uint8_t, 64, 0>());
static_assert(constants<avx512_storage, std::uint8_t, 64, 64>());
static_assert(constants<avx512_storage, std::uint8_t, 64, 128>());
static_assert(constants<avx512_storage, std::uint8_t, 64, 192>());
static_assert(constants<avx512_storage, std::uint16_t, 32, 0>());
static_assert(constants<avx512_storage, std::uint16_t, 32, 64>());
static_assert(constants<avx512_storage, std::uint16_t, 32, 128>());
static_assert(constants<avx512_storage, std::uint16_t, 32, 192>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 0>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 1>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 15>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 16>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 31>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 32>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 63>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 64>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 127>());
static_assert(immediate_constants<avx512_storage, std::uint16_t, 32, 255>());
static_assert(constants<avx512_storage, std::uint32_t, 16, 0>());
static_assert(constants<avx512_storage, std::uint32_t, 16, 64>());
static_assert(constants<avx512_storage, std::uint32_t, 16, 128>());
static_assert(constants<avx512_storage, std::uint32_t, 16, 192>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 0>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 1>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 15>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 16>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 31>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 32>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 63>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 64>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 127>());
static_assert(immediate_constants<avx512_storage, std::uint32_t, 16, 255>());
static_assert(constants<avx512_storage, std::uint64_t, 8, 0>());
static_assert(constants<avx512_storage, std::uint64_t, 8, 64>());
static_assert(constants<avx512_storage, std::uint64_t, 8, 128>());
static_assert(constants<avx512_storage, std::uint64_t, 8, 192>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 0>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 1>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 15>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 16>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 31>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 32>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 63>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 64>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 127>());
static_assert(immediate_constants<avx512_storage, std::uint64_t, 8, 255>());

static_assert(constants<full_width, std::uint8_t, 64, 0>());
static_assert(constants<full_width, std::uint8_t, 64, 64>());
static_assert(constants<full_width, std::uint8_t, 64, 128>());
static_assert(constants<full_width, std::uint8_t, 64, 192>());
static_assert(constants<full_width, std::uint16_t, 32, 0>());
static_assert(constants<full_width, std::uint16_t, 32, 64>());
static_assert(constants<full_width, std::uint16_t, 32, 128>());
static_assert(constants<full_width, std::uint16_t, 32, 192>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 0>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 1>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 15>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 16>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 31>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 32>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 63>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 64>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 127>());
static_assert(immediate_constants<full_width, std::uint16_t, 32, 255>());
static_assert(constants<full_width, std::uint32_t, 16, 0>());
static_assert(constants<full_width, std::uint32_t, 16, 64>());
static_assert(constants<full_width, std::uint32_t, 16, 128>());
static_assert(constants<full_width, std::uint32_t, 16, 192>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 0>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 1>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 15>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 16>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 31>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 32>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 63>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 64>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 127>());
static_assert(immediate_constants<full_width, std::uint32_t, 16, 255>());
static_assert(constants<full_width, std::uint64_t, 8, 0>());
static_assert(constants<full_width, std::uint64_t, 8, 64>());
static_assert(constants<full_width, std::uint64_t, 8, 128>());
static_assert(constants<full_width, std::uint64_t, 8, 192>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 0>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 1>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 15>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 16>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 31>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 32>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 63>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 64>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 127>());
static_assert(immediate_constants<full_width, std::uint64_t, 8, 255>());

static_assert(constants<broad, std::uint8_t, 64, 0>());
static_assert(constants<broad, std::uint8_t, 64, 64>());
static_assert(constants<broad, std::uint8_t, 64, 128>());
static_assert(constants<broad, std::uint8_t, 64, 192>());
static_assert(constants<broad, std::uint16_t, 32, 0>());
static_assert(constants<broad, std::uint16_t, 32, 64>());
static_assert(constants<broad, std::uint16_t, 32, 128>());
static_assert(constants<broad, std::uint16_t, 32, 192>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 0>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 1>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 15>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 16>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 31>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 32>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 63>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 64>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 127>());
static_assert(immediate_constants<broad, std::uint16_t, 32, 255>());
static_assert(constants<broad, std::uint32_t, 16, 0>());
static_assert(constants<broad, std::uint32_t, 16, 64>());
static_assert(constants<broad, std::uint32_t, 16, 128>());
static_assert(constants<broad, std::uint32_t, 16, 192>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 0>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 1>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 15>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 16>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 31>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 32>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 63>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 64>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 127>());
static_assert(immediate_constants<broad, std::uint32_t, 16, 255>());
static_assert(constants<broad, std::uint64_t, 8, 0>());
static_assert(constants<broad, std::uint64_t, 8, 64>());
static_assert(constants<broad, std::uint64_t, 8, 128>());
static_assert(constants<broad, std::uint64_t, 8, 192>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 0>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 1>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 15>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 16>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 31>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 32>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 63>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 64>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 127>());
static_assert(immediate_constants<broad, std::uint64_t, 8, 255>());

template<native::isa<native::x86> A, class T, std::size_t N>
consteval bool memory_constants() {
  using vector_type = native::simd<T, N, A>;
  auto input = pattern<T, N>(0);
  auto value = vector_type::load(input.a.data());
  auto source = vector_type::load(input.source.data());
  for (std::size_t count = 0; count <= N; ++count) {
    auto bits = count == 0 && N == 64 ? std::uint64_t{0} :
      count == N ? ~std::uint64_t{0} : (~std::uint64_t{0} << (N - count));
    // Unused high predicate bits must not turn an empty logical mask into accesses.
    auto mask = native::predicate<N, A>::from_bitset(bits);
    T * memory = count ? new T[count]{} : nullptr;
    std::array<T, N> merged{}, zeroed{}, mutable_load{};
    if constexpr (sizeof(T) == 1) {
      native::mask_vpcompressb<A>(memory, mask, value);
      native::mask_vpexpandb<A>(source, mask, static_cast<T const *>(memory)).store(merged.data());
      native::maskz_vpexpandb<A>(mask, static_cast<T const *>(memory)).store(zeroed.data());
      native::maskz_vpexpandb<A>(mask, memory).store(mutable_load.data());
    }
    if constexpr (sizeof(T) == 2) {
      native::mask_vpcompressw<A>(memory, mask, value);
      native::mask_vpexpandw<A>(source, mask, static_cast<T const *>(memory)).store(merged.data());
      native::maskz_vpexpandw<A>(mask, static_cast<T const *>(memory)).store(zeroed.data());
      native::maskz_vpexpandw<A>(mask, memory).store(mutable_load.data());
    }
    for (std::size_t lane = 0; lane < N; ++lane) {
      bool active = lane >= N - count;
      if (merged[lane] != (active ? input.a[lane] : input.source[lane]) ||
          zeroed[lane] != (active ? input.a[lane] : T(0)) || mutable_load != zeroed) {
        delete[] memory;
        return false;
      }
    }
    delete[] memory;
  }
  return true;
}
static_assert(memory_constants<sse2_storage, std::uint8_t, 16>());
static_assert(memory_constants<sse2_storage, std::uint16_t, 8>());
static_assert(memory_constants<short_width, std::uint8_t, 16>());
static_assert(memory_constants<short_width, std::uint16_t, 8>());
static_assert(memory_constants<broad, std::uint8_t, 16>());
static_assert(memory_constants<broad, std::uint16_t, 8>());
static_assert(memory_constants<avx_storage, std::uint8_t, 32>());
static_assert(memory_constants<avx_storage, std::uint16_t, 16>());
static_assert(memory_constants<short_width, std::uint8_t, 32>());
static_assert(memory_constants<short_width, std::uint16_t, 16>());
static_assert(memory_constants<broad, std::uint8_t, 32>());
static_assert(memory_constants<broad, std::uint16_t, 16>());
static_assert(memory_constants<avx512_storage, std::uint8_t, 64>());
static_assert(memory_constants<avx512_storage, std::uint16_t, 32>());
static_assert(memory_constants<full_width, std::uint8_t, 64>());
static_assert(memory_constants<full_width, std::uint16_t, 32>());
static_assert(memory_constants<broad, std::uint8_t, 64>());
static_assert(memory_constants<broad, std::uint16_t, 32>());

constexpr auto anchor = [] {
  inputs<std::uint8_t, 64> input;
  for (std::size_t lane = 0; lane < 64; ++lane) {
    input.a[lane] = static_cast<std::uint8_t>(lane + 1);
    input.source[lane] = static_cast<std::uint8_t>(128 + lane);
  }
  return input;
}();
constexpr auto anchor_result = calculate<full_width, 7>(anchor, 0x8000000100010001ull);
static_assert(anchor_result.vectors[0][0] == 1 && anchor_result.vectors[0][1] == 17);
static_assert(anchor_result.vectors[0][2] == 33 && anchor_result.vectors[0][3] == 64);
static_assert(anchor_result.vectors[0][4] == 132 && anchor_result.vectors[0][16] == 144);
static_assert(anchor_result.vectors[2][16] == 2 && anchor_result.vectors[2][32] == 3);
static_assert(anchor_result.vectors[2][63] == 4 && anchor_result.vectors[2][1] == 129);
static_assert(shift_reference<std::uint16_t>(0x8001, 0x4002, 0, false) == 0x8001);
static_assert(shift_reference<std::uint16_t>(0x8001, 0x4002, 1, false) == 2);
static_assert(shift_reference<std::uint16_t>(0x8001, 0x4003, 1, true) == 0xc000);

// Literal runtime target boundaries remain visible to the baseline driver.
template<unsigned Imm8, class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
void invoke_512(outputs<T, N> & output, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, full_width>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto counts = vector_type::load(input.counts.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, full_width>::from_bitset(bits);
  outputs<T, N> result;
  result.memory.fill(T(0x5a));
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<full_width>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressb<full_width>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandb<full_width>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandb<full_width>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandb<full_width>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandb<full_width>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressb<full_width>(result.memory.data() + 1, mask, a);
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<full_width>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressw<full_width>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandw<full_width>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandw<full_width>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandw<full_width>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandw<full_width>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressw<full_width>(result.memory.data() + 1, mask, a);
    native::vpshldw<full_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshldw<full_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshldw<full_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshldvw<full_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshldvw<full_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshldvw<full_width>(mask, a, b, counts).store(result.vectors[11].data());
    native::vpshrdw<full_width, Imm8>(a, b).store(result.vectors[12].data());
    native::mask_vpshrdw<full_width, Imm8>(source, mask, a, b).store(result.vectors[13].data());
    native::maskz_vpshrdw<full_width, Imm8>(mask, a, b).store(result.vectors[14].data());
    native::vpshrdvw<full_width>(a, b, counts).store(result.vectors[15].data());
    native::mask_vpshrdvw<full_width>(a, mask, b, counts).store(result.vectors[16].data());
    native::maskz_vpshrdvw<full_width>(mask, a, b, counts).store(result.vectors[17].data());
  }
  if constexpr (sizeof(T) == 4) {
    native::vpshldd<full_width, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldd<full_width, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldd<full_width, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvd<full_width>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvd<full_width>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvd<full_width>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdd<full_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdd<full_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdd<full_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvd<full_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvd<full_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvd<full_width>(mask, a, b, counts).store(result.vectors[11].data());
  }
  if constexpr (sizeof(T) == 8) {
    native::vpshldq<full_width, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldq<full_width, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldq<full_width, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvq<full_width>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvq<full_width>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvq<full_width>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdq<full_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdq<full_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdq<full_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvq<full_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvq<full_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvq<full_width>(mask, a, b, counts).store(result.vectors[11].data());
  }
  output = result;
}

template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi2")
void memory_512(T * destination, T const * memory, std::array<T, N> & merged,
  std::array<T, N> & zeroed, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, full_width>;
  auto value = vector_type::load(input.a.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, full_width>::from_bitset(bits);
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<full_width>(destination, mask, value);
    native::mask_vpexpandb<full_width>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandb<full_width>(mask, memory).store(zeroed.data());
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<full_width>(destination, mask, value);
    native::mask_vpexpandw<full_width>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandw<full_width>(mask, memory).store(zeroed.data());
  }
}

// Literal runtime target boundaries remain visible to the baseline driver.
template<unsigned Imm8, class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void invoke_vl(outputs<T, N> & output, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, short_width>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto counts = vector_type::load(input.counts.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, short_width>::from_bitset(bits);
  outputs<T, N> result;
  result.memory.fill(T(0x5a));
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<short_width>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressb<short_width>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandb<short_width>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandb<short_width>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandb<short_width>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandb<short_width>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressb<short_width>(result.memory.data() + 1, mask, a);
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<short_width>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressw<short_width>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandw<short_width>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandw<short_width>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandw<short_width>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandw<short_width>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressw<short_width>(result.memory.data() + 1, mask, a);
    native::vpshldw<short_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshldw<short_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshldw<short_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshldvw<short_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshldvw<short_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshldvw<short_width>(mask, a, b, counts).store(result.vectors[11].data());
    native::vpshrdw<short_width, Imm8>(a, b).store(result.vectors[12].data());
    native::mask_vpshrdw<short_width, Imm8>(source, mask, a, b).store(result.vectors[13].data());
    native::maskz_vpshrdw<short_width, Imm8>(mask, a, b).store(result.vectors[14].data());
    native::vpshrdvw<short_width>(a, b, counts).store(result.vectors[15].data());
    native::mask_vpshrdvw<short_width>(a, mask, b, counts).store(result.vectors[16].data());
    native::maskz_vpshrdvw<short_width>(mask, a, b, counts).store(result.vectors[17].data());
  }
  if constexpr (sizeof(T) == 4) {
    native::vpshldd<short_width, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldd<short_width, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldd<short_width, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvd<short_width>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvd<short_width>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvd<short_width>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdd<short_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdd<short_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdd<short_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvd<short_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvd<short_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvd<short_width>(mask, a, b, counts).store(result.vectors[11].data());
  }
  if constexpr (sizeof(T) == 8) {
    native::vpshldq<short_width, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldq<short_width, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldq<short_width, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvq<short_width>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvq<short_width>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvq<short_width>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdq<short_width, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdq<short_width, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdq<short_width, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvq<short_width>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvq<short_width>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvq<short_width>(mask, a, b, counts).store(result.vectors[11].data());
  }
  output = result;
}

template<class T, std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi2,avx512vl")
void memory_vl(T * destination, T const * memory, std::array<T, N> & merged,
  std::array<T, N> & zeroed, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, short_width>;
  auto value = vector_type::load(input.a.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, short_width>::from_bitset(bits);
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<short_width>(destination, mask, value);
    native::mask_vpexpandb<short_width>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandb<short_width>(mask, memory).store(zeroed.data());
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<short_width>(destination, mask, value);
    native::mask_vpexpandw<short_width>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandw<short_width>(mask, memory).store(zeroed.data());
  }
}

// Literal runtime target boundaries remain visible to the baseline driver.
template<unsigned Imm8, class T, std::size_t N>
native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void invoke_broad(outputs<T, N> & output, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, broad>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto counts = vector_type::load(input.counts.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, broad>::from_bitset(bits);
  outputs<T, N> result;
  result.memory.fill(T(0x5a));
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<broad>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressb<broad>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandb<broad>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandb<broad>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandb<broad>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandb<broad>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressb<broad>(result.memory.data() + 1, mask, a);
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<broad>(source, mask, a).store(result.vectors[0].data());
    native::maskz_vpcompressw<broad>(mask, a).store(result.vectors[1].data());
    native::mask_vpexpandw<broad>(source, mask, a).store(result.vectors[2].data());
    native::maskz_vpexpandw<broad>(mask, a).store(result.vectors[3].data());
    native::mask_vpexpandw<broad>(source, mask, input.a.data()).store(result.vectors[4].data());
    native::maskz_vpexpandw<broad>(mask, input.a.data()).store(result.vectors[5].data());
    native::mask_vpcompressw<broad>(result.memory.data() + 1, mask, a);
    native::vpshldw<broad, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshldw<broad, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshldw<broad, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshldvw<broad>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshldvw<broad>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshldvw<broad>(mask, a, b, counts).store(result.vectors[11].data());
    native::vpshrdw<broad, Imm8>(a, b).store(result.vectors[12].data());
    native::mask_vpshrdw<broad, Imm8>(source, mask, a, b).store(result.vectors[13].data());
    native::maskz_vpshrdw<broad, Imm8>(mask, a, b).store(result.vectors[14].data());
    native::vpshrdvw<broad>(a, b, counts).store(result.vectors[15].data());
    native::mask_vpshrdvw<broad>(a, mask, b, counts).store(result.vectors[16].data());
    native::maskz_vpshrdvw<broad>(mask, a, b, counts).store(result.vectors[17].data());
  }
  if constexpr (sizeof(T) == 4) {
    native::vpshldd<broad, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldd<broad, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldd<broad, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvd<broad>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvd<broad>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvd<broad>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdd<broad, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdd<broad, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdd<broad, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvd<broad>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvd<broad>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvd<broad>(mask, a, b, counts).store(result.vectors[11].data());
  }
  if constexpr (sizeof(T) == 8) {
    native::vpshldq<broad, Imm8>(a, b).store(result.vectors[0].data());
    native::mask_vpshldq<broad, Imm8>(source, mask, a, b).store(result.vectors[1].data());
    native::maskz_vpshldq<broad, Imm8>(mask, a, b).store(result.vectors[2].data());
    native::vpshldvq<broad>(a, b, counts).store(result.vectors[3].data());
    native::mask_vpshldvq<broad>(a, mask, b, counts).store(result.vectors[4].data());
    native::maskz_vpshldvq<broad>(mask, a, b, counts).store(result.vectors[5].data());
    native::vpshrdq<broad, Imm8>(a, b).store(result.vectors[6].data());
    native::mask_vpshrdq<broad, Imm8>(source, mask, a, b).store(result.vectors[7].data());
    native::maskz_vpshrdq<broad, Imm8>(mask, a, b).store(result.vectors[8].data());
    native::vpshrdvq<broad>(a, b, counts).store(result.vectors[9].data());
    native::mask_vpshrdvq<broad>(a, mask, b, counts).store(result.vectors[10].data());
    native::maskz_vpshrdvq<broad>(mask, a, b, counts).store(result.vectors[11].data());
  }
  output = result;
}

template<class T, std::size_t N>
native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi2")
void memory_broad(T * destination, T const * memory, std::array<T, N> & merged,
  std::array<T, N> & zeroed, inputs<T, N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<T, N, broad>;
  auto value = vector_type::load(input.a.data());
  auto source = vector_type::load(input.source.data());
  auto mask = native::predicate<N, broad>::from_bitset(bits);
  if constexpr (sizeof(T) == 1) {
    native::mask_vpcompressb<broad>(destination, mask, value);
    native::mask_vpexpandb<broad>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandb<broad>(mask, memory).store(zeroed.data());
  }
  if constexpr (sizeof(T) == 2) {
    native::mask_vpcompressw<broad>(destination, mask, value);
    native::mask_vpexpandw<broad>(source, mask, memory).store(merged.data());
    native::maskz_vpexpandw<broad>(mask, memory).store(zeroed.data());
  }
}

inline std::uint64_t random_word(std::uint64_t & state) noexcept {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

template<class T, std::size_t N, bool Broad, unsigned Imm8 = 7>
bool check_case(inputs<T, N> const & input, std::uint64_t mask) {
  outputs<T, N> actual;
  if constexpr (Broad) {
    invoke_broad<Imm8>(actual, input, mask);
  } else if constexpr (sizeof(T) * N == 64) {
    invoke_512<Imm8>(actual, input, mask);
  } else {
    invoke_vl<Imm8>(actual, input, mask);
  }
  if (actual != reference<Imm8>(input, mask)) {
    std::printf("VBMI2 mismatch: bits=%zu lanes=%zu immediate=%u mask=%llx broad=%d\n",
      sizeof(T) * 8, N, Imm8, static_cast<unsigned long long>(mask), Broad);
    return false;
  }
  return true;
}

template<class T, std::size_t N, bool Broad>
bool check_memory() {
  native::test::guarded_pages input_page, output_page;
  auto input = pattern<T, N>(0);
  for (std::size_t count = 0; count <= N; ++count) {
    auto bits = count == 0 && N == 64 ? std::uint64_t{0} :
      count == N ? ~std::uint64_t{0} : (~std::uint64_t{0} << (N - count));
    auto * memory = ::new (input_page.end() - (count + 1) * sizeof(T)) T[count + 1];
    auto * destination = ::new (output_page.end() - (count + 1) * sizeof(T)) T[count + 1];
    memory[0] = destination[0] = T(0x5a);
    for (std::size_t lane = 0; lane < count; ++lane) {
      memory[lane + 1] = input.a[lane];
      destination[lane + 1] = T(0x5a);
    }
    std::array<T, N> merged{}, zeroed{};
    for (bool null_empty : {false, true}) {
      auto * out = count || !null_empty ? destination + 1 : nullptr;
      auto const * in = count || !null_empty ? memory + 1 : nullptr;
      if constexpr (Broad) {
        memory_broad(out, in, merged, zeroed, input, bits);
      } else if constexpr (sizeof(T) * N == 64) {
        memory_512(out, in, merged, zeroed, input, bits);
      } else {
        memory_vl(out, in, merged, zeroed, input, bits);
      }
      auto expected = reference<7>(input, bits);
      if (merged != expected.vectors[4] || zeroed != expected.vectors[5] ||
          memory[0] != T(0x5a) || destination[0] != T(0x5a)) {
        return false;
      }
      for (std::size_t lane = 0; lane < count; ++lane) {
        if (destination[lane + 1] != input.a[N - count + lane]) {
          return false;
        }
      }
    }
  }
  return true;
}

template<class T, std::size_t N, bool Broad = false>
bool check_vectors(std::uint64_t & state) {
  for (unsigned selector = 0; selector < 256; ++selector) {
    auto input = pattern<T, N>(selector);
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{0x5555555555555555}}) {
      if (!check_case<T, N, Broad>(input, mask)) {
        return false;
      }
    }
  }
  auto input = pattern<T, N>(255);
  if constexpr (sizeof(T) * N == 16) {
    // Exhaust every 128-bit logical mask and repeat with all unused bits set.
    for (std::uint64_t mask = 0; mask < (std::uint64_t{1} << N); ++mask) {
      if (!check_case<T, N, Broad>(input, mask) ||
          !check_case<T, N, Broad>(input, mask | (~std::uint64_t{0} << N))) {
        return false;
      }
    }
  }
  for (std::size_t lane = 0; lane < N; ++lane) {
    auto mask = std::uint64_t{1} << lane;
    if (!check_case<T, N, Broad>(input, mask) || !check_case<T, N, Broad>(input, ~mask)) {
      return false;
    }
  }
  if (!check_case<T, N, Broad, 0>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 1>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 15>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 16>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 31>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 32>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 63>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 64>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 127>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  if (!check_case<T, N, Broad, 255>(input, 0xaaaaaaaaaaaaaaaaull)) {
    return false;
  }
  for (unsigned trial = 0; trial < 512; ++trial) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input.a[lane] = static_cast<T>(random_word(state));
      input.b[lane] = static_cast<T>(random_word(state));
      input.counts[lane] = static_cast<T>(random_word(state));
      input.source[lane] = static_cast<T>(random_word(state));
    }
    if (!check_case<T, N, Broad>(input, random_word(state))) {
      return false;
    }
  }
  if constexpr (sizeof(T) <= 2) {
    return check_memory<T, N, Broad>();
  }
  return true;
}
