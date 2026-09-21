// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstddef>
#include <cstdint>
#include <native/attributes.h>
import native.simd;

// Compact masks have scalar storage, even when their tag selects vector kernels.
// Constructing or observing those bits must not require the vector profile.
constexpr auto profile = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl");

template<std::size_t N>
native_noinline constexpr bool roundtrip(std::uint64_t bits) noexcept {
  using M = native::predicate<N, profile>;
  constexpr auto active = ~std::uint64_t{0} >> (64 - N);
  auto mask = M::from_bitset(bits);
  auto native_bits = mask.to_native();
  return mask.to_bitset() == (bits & active) &&
    M::from_native(native_bits).to_bitset() == (bits & active) &&
    M::unsafe_from_native(static_cast<typename M::native_type>(bits)).to_bitset() == (bits & active);
}

static_assert(roundtrip<1>(~std::uint64_t{0}));
static_assert(roundtrip<3>(0xaaaa));
static_assert(roundtrip<2>(~std::uint64_t{0}));
static_assert(roundtrip<4>(0xaaaa));
static_assert(roundtrip<8>(0x5555));
static_assert(roundtrip<16>(~std::uint64_t{0}));
static_assert(roundtrip<32>(~std::uint64_t{0}));
static_assert(roundtrip<64>(~std::uint64_t{0}));

int main(int argc, char **) {
  auto state = std::uint64_t(argc);
  for (unsigned i = 0; i < 256; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    if (!roundtrip<1>(state) || !roundtrip<3>(state) || !roundtrip<2>(state) || !roundtrip<4>(state) || !roundtrip<8>(state) ||
        !roundtrip<16>(state) || !roundtrip<32>(state) || !roundtrip<64>(state)) {
      return 1;
    }
  }
  return 0;
}
