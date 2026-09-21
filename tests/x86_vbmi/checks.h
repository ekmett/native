// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <native/attributes.h>

inline constexpr auto full_width = native::target_features<native::x86>("avx512vbmi");
inline constexpr auto short_width = native::target_features<native::x86>("avx512vbmi,avx512vl");
inline constexpr auto broad = native::target_features<native::x86>(
  "avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi");
inline constexpr auto sse2_storage = native::target_features<native::x86>("sse2");
inline constexpr auto avx_storage = native::target_features<native::x86>("avx");
inline constexpr auto avx512_storage = native::target_features<native::x86>("avx512f");

template<std::size_t N>
struct inputs {
  std::array<std::uint8_t, N> a{}, b{}, indices{}, source{};
  std::array<std::uint64_t, N / 8> words{};
};

template<std::size_t N>
constexpr inputs<N> pattern(unsigned selector) {
  inputs<N> result;
  for (std::size_t lane = 0; lane < N; ++lane) {
    result.a[lane] = static_cast<std::uint8_t>(lane);
    result.b[lane] = static_cast<std::uint8_t>(0x80 + lane);
    result.indices[lane] = static_cast<std::uint8_t>(selector + lane * 17);
    result.source[lane] = static_cast<std::uint8_t>(0xc0 + lane);
  }
  for (std::size_t qword = 0; qword < N / 8; ++qword) {
    result.words[qword] = 0x8000000000000001ull ^ (0x0123456789abcdefull * qword);
  }
  return result;
}

template<std::size_t N>
constexpr std::array<std::array<std::uint8_t, N>, 12>
reference(inputs<N> const & input, std::uint64_t mask) {
  std::array<std::array<std::uint8_t, N>, 12> result{};
  for (std::size_t lane = 0; lane < N; ++lane) {
    auto index = input.indices[lane];
    auto single = input.a[index % N];
    auto dual = (index % (2 * N)) < N ? input.a[index % N] : input.b[index % N];
    std::uint8_t window = 0;
    // Select bits individually instead of using the implementation's rotation.
    for (unsigned bit = 0; bit < 8; ++bit) {
      auto position = (unsigned(index) + bit) % 64;
      window |= ((input.words[lane / 8] >> position) & 1) << bit;
    }
    bool active = (mask >> lane) & 1;
    result[0][lane] = single;
    result[1][lane] = active ? single : input.source[lane];
    result[2][lane] = active ? single : 0;
    result[3][lane] = dual;
    result[4][lane] = active ? dual : input.a[lane];
    result[5][lane] = active ? dual : 0;
    result[6][lane] = dual;
    result[7][lane] = active ? dual : index;
    result[8][lane] = active ? dual : 0;
    result[9][lane] = window;
    result[10][lane] = active ? window : input.source[lane];
    result[11][lane] = active ? window : 0;
  }
  return result;
}

template<native::isa<native::x86> A, std::size_t N>
consteval auto calculate(inputs<N> input, std::uint64_t bits) {
  using vector_type = native::simd<std::uint8_t, N, A>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto indices = vector_type::load(input.indices.data());
  auto source = vector_type::load(input.source.data());
  auto words = native::simd<std::uint64_t, N / 8, A>::load(input.words.data());
  auto mask = native::predicate<N, A>::from_bitset(bits);
  std::array<std::array<std::uint8_t, N>, 12> result{};
  native::vpermb<A>(indices, a).store(result[0].data());
  native::mask_vpermb<A>(source, mask, indices, a).store(result[1].data());
  native::maskz_vpermb<A>(mask, indices, a).store(result[2].data());
  native::vpermt2b<A>(a, indices, b).store(result[3].data());
  native::mask_vpermt2b<A>(a, mask, indices, b).store(result[4].data());
  native::maskz_vpermt2b<A>(mask, a, indices, b).store(result[5].data());
  native::vpermi2b<A>(indices, a, b).store(result[6].data());
  native::mask_vpermi2b<A>(indices, mask, a, b).store(result[7].data());
  native::maskz_vpermi2b<A>(mask, indices, a, b).store(result[8].data());
  native::vpmultishiftqb<A>(indices, words).store(result[9].data());
  native::mask_vpmultishiftqb<A>(source, mask, indices, words).store(result[10].data());
  native::maskz_vpmultishiftqb<A>(mask, indices, words).store(result[11].data());
  return result;
}

// Separate constant banks retain all 256 control values within a bounded step budget.
template<native::isa<native::x86> A, std::size_t N, unsigned Start>
consteval bool constants() {
  for (unsigned selector = Start; selector < Start + 64; ++selector) {
    auto input = pattern<N>(selector);
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{1} << (N - 1)}) {
      if (calculate<A>(input, mask) != reference(input, mask)) {
        return false;
      }
    }
  }
  return true;
}

static_assert(constants<sse2_storage, 16, 0>());
static_assert(constants<sse2_storage, 16, 64>());
static_assert(constants<sse2_storage, 16, 128>());
static_assert(constants<sse2_storage, 16, 192>());

static_assert(constants<avx_storage, 32, 0>());
static_assert(constants<avx_storage, 32, 64>());
static_assert(constants<avx_storage, 32, 128>());
static_assert(constants<avx_storage, 32, 192>());

static_assert(constants<avx512_storage, 64, 0>());
static_assert(constants<avx512_storage, 64, 64>());
static_assert(constants<avx512_storage, 64, 128>());
static_assert(constants<avx512_storage, 64, 192>());

static_assert(constants<short_width, 16, 0>());
static_assert(constants<short_width, 16, 64>());
static_assert(constants<short_width, 16, 128>());
static_assert(constants<short_width, 16, 192>());
static_assert(constants<short_width, 32, 0>());
static_assert(constants<short_width, 32, 64>());
static_assert(constants<short_width, 32, 128>());
static_assert(constants<short_width, 32, 192>());

static_assert(constants<full_width, 64, 0>());
static_assert(constants<full_width, 64, 64>());
static_assert(constants<full_width, 64, 128>());
static_assert(constants<full_width, 64, 192>());

static_assert(constants<broad, 16, 0>());
static_assert(constants<broad, 16, 64>());
static_assert(constants<broad, 16, 128>());
static_assert(constants<broad, 16, 192>());
static_assert(constants<broad, 32, 0>());
static_assert(constants<broad, 32, 64>());
static_assert(constants<broad, 32, 128>());
static_assert(constants<broad, 32, 192>());
static_assert(constants<broad, 64, 0>());
static_assert(constants<broad, 64, 64>());
static_assert(constants<broad, 64, 128>());
static_assert(constants<broad, 64, 192>());

// Explicit anchors cross both 128-bit and 256-bit boundaries and select table b.
constexpr auto anchor = [] {
  auto input = pattern<64>(0);
  input.indices[0] = 63;
  input.indices[1] = 64;
  input.indices[2] = 127;
  input.indices[3] = 255;
  return input;
}();
constexpr auto anchor_result = calculate<full_width>(anchor, 3);
static_assert(anchor_result[0][0] == 63 && anchor_result[0][1] == 0);
static_assert(anchor_result[3][0] == 63 && anchor_result[3][1] == 128);
static_assert(anchor_result[3][2] == 191 && anchor_result[3][3] == 191);
static_assert(anchor_result[4][2] == 2 && anchor_result[7][2] == 127);
static_assert(anchor_result[5][2] == 0 && anchor_result[8][2] == 0);
// Bits 63 then 0 of 0x8000...0001 form the low two bits of the byte window.
static_assert(anchor_result[9][0] == 3 && anchor_result[9][3] == 3);

// Keep each runtime entry's literal target visible to baseline callers.
template<std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi")
void invoke_512(std::uint8_t * result, inputs<N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<std::uint8_t, N, full_width>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto indices = vector_type::load(input.indices.data());
  auto source = vector_type::load(input.source.data());
  auto words = native::simd<std::uint64_t, N / 8, full_width>::load(input.words.data());
  auto mask = native::predicate<N, full_width>::from_bitset(bits);
  native::vpermb<full_width>(indices, a).store(result + 0 * N);
  native::mask_vpermb<full_width>(source, mask, indices, a).store(result + 1 * N);
  native::maskz_vpermb<full_width>(mask, indices, a).store(result + 2 * N);
  native::vpermt2b<full_width>(a, indices, b).store(result + 3 * N);
  native::mask_vpermt2b<full_width>(a, mask, indices, b).store(result + 4 * N);
  native::maskz_vpermt2b<full_width>(mask, a, indices, b).store(result + 5 * N);
  native::vpermi2b<full_width>(indices, a, b).store(result + 6 * N);
  native::mask_vpermi2b<full_width>(indices, mask, a, b).store(result + 7 * N);
  native::maskz_vpermi2b<full_width>(mask, indices, a, b).store(result + 8 * N);
  native::vpmultishiftqb<full_width>(indices, words).store(result + 9 * N);
  native::mask_vpmultishiftqb<full_width>(source, mask, indices, words).store(result + 10 * N);
  native::maskz_vpmultishiftqb<full_width>(mask, indices, words).store(result + 11 * N);
}

// Keep each runtime entry's literal target visible to baseline callers.
template<std::size_t N>
native_noinline native_target("avx512f,avx512bw,avx512vbmi,avx512vl")
void invoke_vl(std::uint8_t * result, inputs<N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<std::uint8_t, N, short_width>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto indices = vector_type::load(input.indices.data());
  auto source = vector_type::load(input.source.data());
  auto words = native::simd<std::uint64_t, N / 8, short_width>::load(input.words.data());
  auto mask = native::predicate<N, short_width>::from_bitset(bits);
  native::vpermb<short_width>(indices, a).store(result + 0 * N);
  native::mask_vpermb<short_width>(source, mask, indices, a).store(result + 1 * N);
  native::maskz_vpermb<short_width>(mask, indices, a).store(result + 2 * N);
  native::vpermt2b<short_width>(a, indices, b).store(result + 3 * N);
  native::mask_vpermt2b<short_width>(a, mask, indices, b).store(result + 4 * N);
  native::maskz_vpermt2b<short_width>(mask, a, indices, b).store(result + 5 * N);
  native::vpermi2b<short_width>(indices, a, b).store(result + 6 * N);
  native::mask_vpermi2b<short_width>(indices, mask, a, b).store(result + 7 * N);
  native::maskz_vpermi2b<short_width>(mask, indices, a, b).store(result + 8 * N);
  native::vpmultishiftqb<short_width>(indices, words).store(result + 9 * N);
  native::mask_vpmultishiftqb<short_width>(source, mask, indices, words).store(result + 10 * N);
  native::maskz_vpmultishiftqb<short_width>(mask, indices, words).store(result + 11 * N);
}

// Keep each runtime entry's literal target visible to baseline callers.
template<std::size_t N>
native_noinline native_target("avx2,avx512f,avx512dq,avx512bw,avx512vl,avx512vbmi")
void invoke_broad(std::uint8_t * result, inputs<N> const & input, std::uint64_t bits) noexcept {
  using vector_type = native::simd<std::uint8_t, N, broad>;
  auto a = vector_type::load(input.a.data());
  auto b = vector_type::load(input.b.data());
  auto indices = vector_type::load(input.indices.data());
  auto source = vector_type::load(input.source.data());
  auto words = native::simd<std::uint64_t, N / 8, broad>::load(input.words.data());
  auto mask = native::predicate<N, broad>::from_bitset(bits);
  native::vpermb<broad>(indices, a).store(result + 0 * N);
  native::mask_vpermb<broad>(source, mask, indices, a).store(result + 1 * N);
  native::maskz_vpermb<broad>(mask, indices, a).store(result + 2 * N);
  native::vpermt2b<broad>(a, indices, b).store(result + 3 * N);
  native::mask_vpermt2b<broad>(a, mask, indices, b).store(result + 4 * N);
  native::maskz_vpermt2b<broad>(mask, a, indices, b).store(result + 5 * N);
  native::vpermi2b<broad>(indices, a, b).store(result + 6 * N);
  native::mask_vpermi2b<broad>(indices, mask, a, b).store(result + 7 * N);
  native::maskz_vpermi2b<broad>(mask, indices, a, b).store(result + 8 * N);
  native::vpmultishiftqb<broad>(indices, words).store(result + 9 * N);
  native::mask_vpmultishiftqb<broad>(source, mask, indices, words).store(result + 10 * N);
  native::maskz_vpmultishiftqb<broad>(mask, indices, words).store(result + 11 * N);
}

inline std::uint64_t random_word(std::uint64_t & state) noexcept {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

template<std::size_t N, bool Broad = false>
bool check_case(inputs<N> const & input, std::uint64_t mask) {
  std::array<std::uint8_t, N * 12> actual{};
  if constexpr (Broad) {
    invoke_broad<N>(actual.data(), input, mask);
  } else if constexpr (N == 64) {
    invoke_512<N>(actual.data(), input, mask);
  } else {
    invoke_vl<N>(actual.data(), input, mask);
  }
  auto expected = reference(input, mask);
  for (unsigned form = 0; form < 12; ++form) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      if (actual[form * N + lane] != expected[form][lane]) {
        std::printf("VBMI mismatch: lanes=%zu form=%u lane=%zu mask=%llx expected=%u actual=%u\n",
          N, form, lane, static_cast<unsigned long long>(mask),
          unsigned(expected[form][lane]), unsigned(actual[form * N + lane]));
        return false;
      }
    }
  }
  return true;
}

template<std::size_t N, bool Broad = false>
bool check_vectors(std::uint64_t & state) {
  for (unsigned selector = 0; selector < 256; ++selector) {
    auto input = pattern<N>(selector);
    for (auto mask : {std::uint64_t{0}, ~std::uint64_t{0},
                      std::uint64_t{0xaaaaaaaaaaaaaaaa}, std::uint64_t{0x5555555555555555}}) {
      if (!check_case<N, Broad>(input, mask)) {
        return false;
      }
    }
  }
  auto input = pattern<N>(63);
  if constexpr (N == 16) {
    // Exhaust all logical masks; also set every unused predicate bit.
    for (std::uint64_t mask = 0; mask < 65536; ++mask) {
      if (!check_case<N, Broad>(input, mask) ||
          !check_case<N, Broad>(input, mask | 0xffffffffffff0000ull)) {
        return false;
      }
    }
  }
  for (std::size_t lane = 0; lane < N; ++lane) {
    auto mask = std::uint64_t{1} << lane;
    if (!check_case<N, Broad>(input, mask) || !check_case<N, Broad>(input, ~mask)) {
      return false;
    }
  }
  for (unsigned trial = 0; trial < 2048; ++trial) {
    for (std::size_t lane = 0; lane < N; ++lane) {
      input.a[lane] = static_cast<std::uint8_t>(random_word(state));
      input.b[lane] = static_cast<std::uint8_t>(random_word(state));
      input.indices[lane] = static_cast<std::uint8_t>(random_word(state));
      input.source[lane] = static_cast<std::uint8_t>(random_word(state));
    }
    for (auto & word : input.words) {
      word = random_word(state);
    }
    if (!check_case<N, Broad>(input, random_word(state))) {
      return false;
    }
  }
  return true;
}
