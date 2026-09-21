// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace gfni_fixture {
  using native::x86_feature;
  constexpr native::isa arch128{x86_feature::gfni};
  constexpr native::isa arch256 = arch128 & x86_feature::avx;
  constexpr native::isa arch512 = arch128 & x86_feature::avx512f;
  constexpr native::isa mask512 = arch512 & x86_feature::avx512bw;
  constexpr native::isa mask_narrow = mask512 & x86_feature::avx512vl;
  static_assert(!arch128.has(x86_feature::avx));
  static_assert(!arch256.has(x86_feature::avx2));
  static_assert(!arch512.has(x86_feature::avx512bw));

  template<native::isa A, class V, unsigned Imm8 = 0>
  concept has_operations = requires(V x) {
    { native::gf2p8mulb<A>(x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineqb<A, Imm8>(x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineinvqb<A, Imm8>(x, x) } noexcept -> std::same_as<V>;
  };
  template<native::isa A, class V, class K, unsigned Imm8 = 0>
  concept has_masks = requires(V x, K k) {
    { native::gf2p8mulb_mask<A>(x, k, x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8mulb_maskz<A>(k, x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineqb_mask<A, Imm8>(x, k, x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineqb_maskz<A, Imm8>(k, x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineinvqb_mask<A, Imm8>(x, k, x, x) } noexcept -> std::same_as<V>;
    { native::gf2p8affineinvqb_maskz<A, Imm8>(k, x, x) } noexcept -> std::same_as<V>;
  };
  template<native::isa A, class V>
  concept rejects_operations =
    !requires(V x) { native::gf2p8mulb<A>(x, x); } &&
    !requires(V x) { native::gf2p8affineqb<A, 0>(x, x); } &&
    !requires(V x) { native::gf2p8affineinvqb<A, 0>(x, x); };
  template<native::isa A, class V, class K>
  concept rejects_masks =
    !requires(V x, K k) { native::gf2p8mulb_mask<A>(x, k, x, x); } &&
    !requires(V x, K k) { native::gf2p8mulb_maskz<A>(k, x, x); } &&
    !requires(V x, K k) { native::gf2p8affineqb_mask<A, 0>(x, k, x, x); } &&
    !requires(V x, K k) { native::gf2p8affineqb_maskz<A, 0>(k, x, x); } &&
    !requires(V x, K k) { native::gf2p8affineinvqb_mask<A, 0>(x, k, x, x); } &&
    !requires(V x, K k) { native::gf2p8affineinvqb_maskz<A, 0>(k, x, x); };

  static_assert(has_operations<arch128, __m128i, 255>);
  static_assert(has_operations<arch256, __m256i, 255>);
  static_assert(has_operations<arch512, __m512i, 255>);
  static_assert(rejects_operations<native::scalar, __m128i>);
  static_assert(rejects_operations<native::avx2, __m128i>);
  static_assert(rejects_operations<arch128, __m256i>);
  static_assert(rejects_operations<arch256, __m512i>);
  static_assert(!has_operations<arch128, __m128i, 256>);
  static_assert(!has_operations<arch256, __m256i, 256>);
  static_assert(!has_operations<arch512, __m512i, 256>);
  static_assert(has_masks<mask_narrow, __m128i, __mmask16, 255>);
  static_assert(has_masks<mask_narrow, __m256i, __mmask32, 255>);
  static_assert(has_masks<mask512, __m512i, __mmask64, 255>);
  static_assert(rejects_masks<mask512, __m128i, __mmask16>);
  static_assert(rejects_masks<mask512, __m256i, __mmask32>);
  static_assert(rejects_masks<arch512, __m512i, __mmask64>);
  static_assert(rejects_masks<arch512 & x86_feature::avx512vl, __m128i, __mmask16>);
  static_assert(rejects_masks<arch512 & x86_feature::avx512vl, __m256i, __mmask32>);
  static_assert(rejects_masks<native::avx512, __m128i, __mmask16>);
  static_assert(rejects_masks<native::avx512, __m256i, __mmask32>);
  static_assert(rejects_masks<native::avx512, __m512i, __mmask64>);
  static_assert(!has_masks<mask_narrow, __m128i, __mmask16, 256>);
  static_assert(!has_masks<mask_narrow, __m256i, __mmask32, 256>);
  static_assert(!has_masks<mask512, __m512i, __mmask64, 256>);

  using byte = std::uint8_t;
  template<std::size_t N> using bytes = std::array<byte, N>;
  template<std::size_t N> struct inputs { bytes<N> a, b, source; };
  // Product, three affine constants, then the same inverse-affine constants.
  template<std::size_t N> using outputs = std::array<bytes<N>, 7>;
  constexpr std::array<unsigned, 3> constants{0, 0xa5, 0xff};

  // Multiply binary polynomials, then divide by x^8+x^4+x^3+x+1.
  constexpr byte multiply(byte a, byte b) {
    unsigned product = 0;
    for (unsigned bit = 0; bit != 8; ++bit)
      if ((unsigned(b) >> bit) & 1u) product ^= unsigned(a) << bit;
    for (unsigned bit = 15; bit-- != 8;)
      if ((product >> bit) & 1u) product ^= 0x11bu << (bit - 8);
    return byte(product);
  }

  constexpr byte inverse(byte a) {
    if (!a) return 0;
    byte result = 1;
    byte power = a;
    for (unsigned exponent = 254; exponent; exponent >>= 1) {
      if (exponent & 1u) result = multiply(result, power);
      power = multiply(power, power);
    }
    return result;
  }

  constexpr auto inverses = [] {
    std::array<byte, 256> result{};
    for (unsigned i = 0; i != result.size(); ++i) result[i] = inverse(byte(i));
    return result;
  }();
  static_assert(multiply(0x57, 0x83) == 0xc1);
  static_assert(multiply(0x80, 2) == 0x1b);
  static_assert(inverses[0] == 0 && inverses[1] == 1);
  static_assert([] {
    for (unsigned i = 1; i != 256; ++i)
      if (multiply(byte(i), inverses[i]) != 1) return false;
    return true;
  }());

  // Each qword supplies eight rows in reverse byte order: byte 7 is row 0.
  constexpr byte affine(byte x, byte const* matrix, unsigned constant) {
    unsigned result = constant;
    for (unsigned row = 0; row != 8; ++row) {
      unsigned parity = 0;
      for (unsigned column = 0; column != 8; ++column)
        parity ^= ((unsigned(x) >> column) & 1u) &
                  ((unsigned(matrix[7 - row]) >> column) & 1u);
      result ^= parity << row;
    }
    return byte(result);
  }
  constexpr bytes<8> identity_matrix{0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
  constexpr bytes<8> aes_matrix{0xf8, 0x7c, 0x3e, 0x1f, 0x8f, 0xc7, 0xe3, 0xf1};
  static_assert(affine(inverses[0x53], aes_matrix.data(), 0x63) == 0xed);
  static_assert(affine(inverses[0], aes_matrix.data(), 0x63) == 0x63);
  static_assert([] {
    for (unsigned i = 0; i != 256; ++i)
      if (affine(byte(i), identity_matrix.data(), 0) != i) return false;
    return true;
  }());

  template<std::size_t N> outputs<N> oracle(inputs<N> const& input) {
    outputs<N> result{};
    for (std::size_t lane = 0; lane != N; ++lane) {
      auto const* matrix = input.b.data() + (lane / 8) * 8;
      result[0][lane] = multiply(input.a[lane], input.b[lane]);
      for (std::size_t c = 0; c != constants.size(); ++c) {
        result[1 + c][lane] = affine(input.a[lane], matrix, constants[c]);
        result[4 + c][lane] = affine(inverses[input.a[lane]], matrix, constants[c]);
      }
    }
    return result;
  }

  // Pointer arguments keep optional vector ABIs behind the admission boundary.
#define GFNI_EVALUATE(NAME, V, N, ARCH, TARGET) \
  native_target(TARGET) native_noinline \
  void NAME(inputs<N> const& input, outputs<N>& output) { \
    V a, b; \
    __builtin_memcpy(&a, input.a.data(), N); \
    __builtin_memcpy(&b, input.b.data(), N); \
    V result[] = { \
      native::gf2p8mulb<ARCH>(a, b), \
      native::gf2p8affineqb<ARCH, 0>(a, b), \
      native::gf2p8affineqb<ARCH, 0xa5>(a, b), \
      native::gf2p8affineqb<ARCH, 0xff>(a, b), \
      native::gf2p8affineinvqb<ARCH, 0>(a, b), \
      native::gf2p8affineinvqb<ARCH, 0xa5>(a, b), \
      native::gf2p8affineinvqb<ARCH, 0xff>(a, b) \
    }; \
    __builtin_memcpy(output.data(), result, sizeof(result)); \
  }
  GFNI_EVALUATE(evaluate128, __m128i, 16, arch128, "gfni,no-avx")
  GFNI_EVALUATE(evaluate256, __m256i, 32, arch256, "avx,gfni,no-avx2")
  GFNI_EVALUATE(evaluate512, __m512i, 64, arch512, "avx512f,gfni")
#undef GFNI_EVALUATE

#define GFNI_EVALUATE_MASK(NAME, V, K, N, ARCH, TARGET) \
  native_target(TARGET) native_noinline \
  void NAME(inputs<N> const& input, std::uint64_t bits, outputs<N>& merged, outputs<N>& zeroed) { \
    V a, b, source; \
    __builtin_memcpy(&a, input.a.data(), N); \
    __builtin_memcpy(&b, input.b.data(), N); \
    __builtin_memcpy(&source, input.source.data(), N); \
    K k = K(bits); \
    V merge_results[] = { \
      native::gf2p8mulb_mask<ARCH>(source, k, a, b), \
      native::gf2p8affineqb_mask<ARCH, 0>(source, k, a, b), \
      native::gf2p8affineqb_mask<ARCH, 0xa5>(source, k, a, b), \
      native::gf2p8affineqb_mask<ARCH, 0xff>(source, k, a, b), \
      native::gf2p8affineinvqb_mask<ARCH, 0>(source, k, a, b), \
      native::gf2p8affineinvqb_mask<ARCH, 0xa5>(source, k, a, b), \
      native::gf2p8affineinvqb_mask<ARCH, 0xff>(source, k, a, b) \
    }; \
    V zero_results[] = { \
      native::gf2p8mulb_maskz<ARCH>(k, a, b), \
      native::gf2p8affineqb_maskz<ARCH, 0>(k, a, b), \
      native::gf2p8affineqb_maskz<ARCH, 0xa5>(k, a, b), \
      native::gf2p8affineqb_maskz<ARCH, 0xff>(k, a, b), \
      native::gf2p8affineinvqb_maskz<ARCH, 0>(k, a, b), \
      native::gf2p8affineinvqb_maskz<ARCH, 0xa5>(k, a, b), \
      native::gf2p8affineinvqb_maskz<ARCH, 0xff>(k, a, b) \
    }; \
    __builtin_memcpy(merged.data(), merge_results, sizeof(merge_results)); \
    __builtin_memcpy(zeroed.data(), zero_results, sizeof(zero_results)); \
  }
  GFNI_EVALUATE_MASK(evaluate_mask128, __m128i, __mmask16, 16, mask_narrow, "gfni,avx512f,avx512bw,avx512vl")
  GFNI_EVALUATE_MASK(evaluate_mask256, __m256i, __mmask32, 32, mask_narrow, "gfni,avx512f,avx512bw,avx512vl")
  GFNI_EVALUATE_MASK(evaluate_mask512, __m512i, __mmask64, 64, mask512, "gfni,avx512f,avx512bw")
#undef GFNI_EVALUATE_MASK

  std::uint64_t next(std::uint64_t& seed) {
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;
    return seed;
  }

  template<std::size_t N> using evaluator = void (*)(inputs<N> const&, outputs<N>&);
  template<std::size_t N> using mask_evaluator =
    void (*)(inputs<N> const&, std::uint64_t, outputs<N>&, outputs<N>&);

  template<std::size_t N>
  bool equal(outputs<N> const& actual, outputs<N> const& expected, char const* kind) {
    for (std::size_t operation = 0; operation != actual.size(); ++operation)
      for (std::size_t lane = 0; lane != N; ++lane)
        if (actual[operation][lane] != expected[operation][lane]) {
          std::fprintf(stderr, "%s GFNI %zu-bit operation %zu lane %zu: got %02x, expected %02x\n",
            kind, N * 8, operation, lane, unsigned(actual[operation][lane]),
            unsigned(expected[operation][lane]));
          return false;
        }
    return true;
  }

  template<std::size_t N>
  bool check_one(inputs<N> const& input, evaluator<N> evaluate) {
    outputs<N> actual{};
    evaluate(input, actual);
    return equal(actual, oracle(input), "unmasked");
  }

  template<std::size_t N>
  bool check_width(evaluator<N> evaluate, std::uint64_t seed) {
    inputs<N> input{};
    // All 65,536 products also exercise every byte as an inverse input.
    for (unsigned pair = 0; pair != 65536; pair += N) {
      for (std::size_t lane = 0; lane != N; ++lane) {
        input.a[lane] = byte((pair + lane) >> 8);
        input.b[lane] = byte(pair + lane);
      }
      if (!check_one(input, evaluate)) return false;
    }
    // One matrix coefficient at a time identifies row reversal and bit order.
    for (unsigned row = 0; row != 8; ++row)
      for (unsigned column = 0; column != 8; ++column) {
        input.b.fill(0);
        for (std::size_t lane = 0; lane != N; ++lane) {
          input.a[lane] = byte(1u << (lane % 8));
          if (lane % 8 == 7 - row) input.b[lane] = byte(1u << column);
        }
        if (!check_one(input, evaluate)) return false;
      }
    for (unsigned value = 0; value != 256; ++value) {
      input.a.fill(byte(value));
      input.b.fill(0);
      if (!check_one(input, evaluate)) return false;
      input.b.fill(0xff);
      if (!check_one(input, evaluate)) return false;
      for (std::size_t lane = 0; lane != N; ++lane)
        input.b[lane] = byte(1u << (7 - lane % 8));
      if (!check_one(input, evaluate)) return false;
      // Different rotations in adjacent qwords detect incorrect matrix reuse.
      for (std::size_t lane = 0; lane != N; ++lane)
        input.b[lane] = byte(1u << ((7 - lane % 8 + lane / 8) % 8));
      if (!check_one(input, evaluate)) return false;
    }
    for (unsigned iteration = 0; iteration != 2048; ++iteration) {
      for (std::size_t lane = 0; lane != N; ++lane) {
        input.a[lane] = byte(next(seed));
        input.b[lane] = byte(next(seed));
      }
      if (!check_one(input, evaluate)) return false;
    }
    return true;
  }

  template<std::size_t N>
  bool check_mask_one(inputs<N> const& input, mask_evaluator<N> evaluate, std::uint64_t mask) {
    outputs<N> merged{}, zeroed{};
    evaluate(input, mask, merged, zeroed);
    auto merge_expected = oracle(input);
    auto zero_expected = merge_expected;
    for (std::size_t operation = 0; operation != merged.size(); ++operation)
      for (std::size_t lane = 0; lane != N; ++lane)
        if (((mask >> lane) & 1u) == 0) {
          merge_expected[operation][lane] = input.source[lane];
          zero_expected[operation][lane] = 0;
        }
    return equal(merged, merge_expected, "merge-mask") &&
           equal(zeroed, zero_expected, "zero-mask");
  }

  template<std::size_t N>
  bool check_masks(mask_evaluator<N> evaluate, std::uint64_t seed) {
    inputs<N> input{};
    for (unsigned iteration = 0; iteration != 1024; ++iteration) {
      for (std::size_t lane = 0; lane != N; ++lane) {
        input.a[lane] = byte(next(seed));
        input.b[lane] = byte(next(seed));
        input.source[lane] = byte(next(seed) | 1u);
      }
      for (auto mask : std::array<std::uint64_t, 5>{0, ~0ull, 0xaaaaaaaaaaaaaaaaull,
            0x5555555555555555ull, next(seed)})
        if (!check_mask_one(input, evaluate, mask)) return false;
      if (iteration == 0)
        for (std::size_t lane = 0; lane != N; ++lane)
          if (!check_mask_one(input, evaluate, std::uint64_t{1} << lane)) return false;
    }
    return true;
  }

  int run(int argc) {
    auto cpu = native::observe_x86_capabilities();
    std::printf("CPUID.7.0:ECX=%08x GFNI=%u AVX=%u AVX2=%u AVX512F=%u "
                "XCR0 observed=%u value=%016llx\n",
      unsigned(cpu.raw.leaf7_ecx), unsigned(cpu.present.has(x86_feature::gfni)),
      unsigned(cpu.present.has(x86_feature::avx)), unsigned(cpu.present.has(x86_feature::avx2)),
      unsigned(cpu.present.has(x86_feature::avx512f)), unsigned(cpu.xcr0_observed),
      static_cast<unsigned long long>(cpu.xcr0));
    auto seed = 0x243f6a8885a308d3ull ^ unsigned(argc);
    unsigned executed = 0;
    auto run_one = [&](char const* name, native::isa requirements, auto operation) {
      auto admission = native::classify_isa(cpu, requirements);
      if (!admission.admitted()) {
        std::printf("SKIP %s: %s\n", name, admission.reason());
        return true;
      }
      if (!operation()) return false;
      std::printf("PASS %s: scalar oracle verified by native execution\n", name);
      ++executed;
      return true;
    };
    if (!run_one("GFNI legacy128", arch128, [&] { return check_width<16>(evaluate128, seed); }) ||
        !run_one("GFNI VEX256 (AVX, no AVX2)", arch256, [&] { return check_width<32>(evaluate256, seed); }) ||
        !run_one("GFNI EVEX512", arch512, [&] { return check_width<64>(evaluate512, seed); }) ||
        !run_one("GFNI masked128", mask_narrow, [&] { return check_masks<16>(evaluate_mask128, seed); }) ||
        !run_one("GFNI masked256", mask_narrow, [&] { return check_masks<32>(evaluate_mask256, seed); }) ||
        !run_one("GFNI masked512", mask512, [&] { return check_masks<64>(evaluate_mask512, seed); }))
      return 1;
    return executed ? 0 : 77;
  }
}
