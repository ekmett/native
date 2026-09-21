// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

namespace vnni_fixture {
  using native::x86_feature;
  constexpr auto core = native::feature_closure(native::isa<native::x86>{x86_feature::avxvnni});
  constexpr native::isa<native::x86> evex512 = native::feature_closure(x86_feature::avx512f & x86_feature::avx512vnni);
  constexpr native::isa<native::x86> evexvl = native::feature_closure(evex512 & x86_feature::avx512vl);
  constexpr auto int8 = native::feature_closure(native::isa<native::x86>{x86_feature::avxvnniint8});
  constexpr auto int16 = native::feature_closure(native::isa<native::x86>{x86_feature::avxvnniint16});


#include "api_checks.h"

  enum class saturation { modulo, signed32, unsigned32 };
  struct operation {
    char const* name;
    unsigned bits;
    bool signed_a, signed_b;
    saturation clamp;
  };
  constexpr std::array core_operations{
    operation{"dpbusd", 8, false, true, saturation::modulo},
    operation{"dpbusds", 8, false, true, saturation::signed32},
    operation{"dpwssd", 16, true, true, saturation::modulo},
    operation{"dpwssds", 16, true, true, saturation::signed32}};
  constexpr std::array int8_operations{
    operation{"dpbssd", 8, true, true, saturation::modulo},
    operation{"dpbssds", 8, true, true, saturation::signed32},
    operation{"dpbsud", 8, true, false, saturation::modulo},
    operation{"dpbsuds", 8, true, false, saturation::signed32},
    operation{"dpbuud", 8, false, false, saturation::modulo},
    operation{"dpbuuds", 8, false, false, saturation::unsigned32}};
  constexpr std::array int16_operations{
    operation{"dpwsud", 16, true, false, saturation::modulo},
    operation{"dpwsuds", 16, true, false, saturation::signed32},
    operation{"dpwusd", 16, false, true, saturation::modulo},
    operation{"dpwusds", 16, false, true, saturation::signed32},
    operation{"dpwuud", 16, false, false, saturation::modulo},
    operation{"dpwuuds", 16, false, false, saturation::unsigned32}};

  template<std::size_t N> struct inputs {
    std::array<std::uint32_t, N / 4> accumulator{};
    std::array<std::uint8_t, N> a{}, b{};
  };
  template<std::size_t N, std::size_t O>
  using outputs = std::array<std::array<std::uint32_t, N / 4>, O>;

  constexpr std::int64_t signed_value(std::uint32_t value, unsigned bits) {
    return std::int64_t(value) - ((value >> (bits - 1)) ? (std::int64_t{1} << bits) : 0);
  }

  // Decode little-endian fields explicitly. Products and the accumulator share
  // one wide sum; saturation happens only after every product has been added.
  // Intel SDM, VPDPB[SU,UU,SS]D[S] and VPDPW[SU,US,UU]D[S]:
  // https://cdrdv2-public.intel.com/835757/325383-sdm-vol-2abcd.pdf
  // The UU saturating forms use unsigned saturation and an unsigned accumulator.
  constexpr std::uint32_t oracle_lane(operation op, std::uint32_t accumulator,
      std::uint8_t const* a, std::uint8_t const* b) {
    std::int64_t signed_sum = signed_value(accumulator, 32);
    std::uint64_t unsigned_sum = accumulator;
    for (unsigned byte = 0; byte != 4; byte += op.bits / 8) {
      std::uint32_t aa = a[byte], bb = b[byte];
      if (op.bits == 16) {
        aa |= std::uint32_t(a[byte + 1]) << 8;
        bb |= std::uint32_t(b[byte + 1]) << 8;
      }
      if (op.clamp == saturation::unsigned32)
        unsigned_sum += std::uint64_t(aa) * std::uint64_t(bb);
      else
        signed_sum += (op.signed_a ? signed_value(aa, op.bits) : std::int64_t(aa)) *
                      (op.signed_b ? signed_value(bb, op.bits) : std::int64_t(bb));
    }
    if (op.clamp == saturation::unsigned32)
      return unsigned_sum > 0xffffffffull ? 0xffffffffu : std::uint32_t(unsigned_sum);
    if (op.clamp == saturation::signed32) {
      if (signed_sum > 2147483647ll) return 0x7fffffffu;
      if (signed_sum < -2147483648ll) return 0x80000000u;
    }
    return std::uint32_t(signed_sum);
  }

  constexpr bool check_scalar_reference() {
    constexpr std::array<std::uint8_t, 4> min_words{0, 128, 0, 128};
    constexpr std::array<std::uint8_t, 4> max_words{255, 127, 255, 127};
    constexpr std::array<std::uint8_t, 4> ones{1, 1, 1, 1};
    constexpr std::array<std::uint8_t, 4> all{255, 255, 255, 255};
    constexpr std::array<std::uint8_t, 4> byte_a{128, 255, 127, 1};
    constexpr std::array<std::uint8_t, 4> byte_b{255, 2, 3, 4};
    return oracle_lane(core_operations[0], 0, byte_a.data(), byte_b.data()) == 767 &&
      oracle_lane(core_operations[1], 0x7fffffffu, byte_a.data(), byte_b.data()) == 0x7fffffffu &&
      oracle_lane(core_operations[2], 0, min_words.data(), min_words.data()) == 0x80000000u &&
      oracle_lane(core_operations[3], 0, min_words.data(), min_words.data()) == 0x7fffffffu &&
      oracle_lane(core_operations[3], 0xffffffffu, min_words.data(), min_words.data()) == 0x7fffffffu &&
      oracle_lane(core_operations[3], 0x80000000u, min_words.data(), min_words.data()) == 0 &&
      oracle_lane(int16_operations[1], 0x7fffffffu, min_words.data(), all.data()) == 0x8000ffffu &&
      oracle_lane(int16_operations[1], 0x80000000u, max_words.data(), all.data()) == 0x7ffd0002u &&
      oracle_lane(int16_operations[0], 0, min_words.data(), all.data()) == 0x00010000u &&
      oracle_lane(int16_operations[2], 0, all.data(), min_words.data()) == 0x00010000u &&
      oracle_lane(int16_operations[3], 0x7fffffffu, all.data(), min_words.data()) == 0x8000ffffu &&
      oracle_lane(int16_operations[4], 0, all.data(), all.data()) == 0xfffc0002u &&
      oracle_lane(int16_operations[5], 0, all.data(), all.data()) == 0xffffffffu &&
      oracle_lane(int8_operations[0], 0, byte_a.data(), byte_b.data()) == 511 &&
      oracle_lane(int8_operations[1], 0x7fffffffu, byte_a.data(), byte_b.data()) == 0x7fffffffu &&
      oracle_lane(int8_operations[2], 0, byte_a.data(), byte_b.data()) == 0xffff81ffu &&
      oracle_lane(int8_operations[3], 0x80000000u, byte_a.data(), byte_b.data()) == 0x80000000u &&
      oracle_lane(int8_operations[4], 0, byte_a.data(), byte_b.data()) == 0x000082ffu &&
      oracle_lane(int8_operations[5], 0x80000000u, ones.data(), ones.data()) == 0x80000004u &&
      oracle_lane(int8_operations[5], 0xffffffffu, ones.data(), ones.data()) == 0xffffffffu &&
      oracle_lane(core_operations[1], 0x80000000u, ones.data(), all.data()) == 0x80000000u;
  }
  static_assert(check_scalar_reference());

  template<std::size_t N, std::size_t O>
  outputs<N, O> oracle(inputs<N> const& input, std::array<operation, O> const& operations) {
    outputs<N, O> result{};
    for (std::size_t op = 0; op != O; ++op)
      for (std::size_t lane = 0; lane != N / 4; ++lane)
        result[op][lane] = oracle_lane(operations[op], input.accumulator[lane],
          input.a.data() + lane * 4, input.b.data() + lane * 4);
    return result;
  }

  // All target boundaries use pointers and integer masks, never vector values.
#define NATIVE_VNNI_LOAD(V) \
    V acc, a, b; \
    __builtin_memcpy(&acc, input->accumulator.data(), sizeof(V)); \
    __builtin_memcpy(&a, input->a.data(), sizeof(V)); \
    __builtin_memcpy(&b, input->b.data(), sizeof(V))
#define NATIVE_VNNI_CORE_RESULTS(V, A) \
    V result[] = {native::dpbusd<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), native::dpbusds<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), \
      native::dpwssd<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native(), native::dpwssds<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native()}; \
    __builtin_memcpy(plain, result, sizeof(result))
#define NATIVE_VNNI_CORE(Name, V, N) \
  extern "C" native_noinline __attribute__((target("avxvnni"))) void Name( \
      inputs<N> const* input, outputs<N, 4>* plain, unsigned, outputs<N, 4>*, outputs<N, 4>*) { \
    NATIVE_VNNI_LOAD(V); \
    NATIVE_VNNI_CORE_RESULTS(V, core); \
  }
  NATIVE_VNNI_CORE(invoke_core128, __m128i, 16)
  NATIVE_VNNI_CORE(invoke_core256, __m256i, 32)
#undef NATIVE_VNNI_CORE

#define NATIVE_VNNI_EVEX(Name, V, K, N, A, Target) \
  extern "C" native_noinline __attribute__((target(Target))) void Name( \
      inputs<N> const* input, outputs<N, 4>* plain, unsigned bits, \
      outputs<N, 4>* merged, outputs<N, 4>* zeroed) { \
    NATIVE_VNNI_LOAD(V); \
    NATIVE_VNNI_CORE_RESULTS(V, A); \
    K k = static_cast<K>(bits); \
    V merge[] = {native::mask_dpbusd<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), native::mask_dpbusds<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), \
      native::mask_dpwssd<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native(), native::mask_dpwssds<A>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native()}; \
    V zero[] = {native::maskz_dpbusd<A>(native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), native::maskz_dpbusds<A>(native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), A>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), A>::from_native(b)).to_native(), \
      native::maskz_dpwssd<A>(native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native(), native::maskz_dpwssds<A>(native::predicate<sizeof(acc) / sizeof(std::int32_t), A>::from_bitset(k), native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), A>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), A>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), A>::from_native(b)).to_native()}; \
    __builtin_memcpy(merged, merge, sizeof(merge)); \
    __builtin_memcpy(zeroed, zero, sizeof(zero)); \
  }
  NATIVE_VNNI_EVEX(invoke_evex128, __m128i, __mmask8, 16, evexvl, "avx512f,avx512vnni,avx512vl")
  NATIVE_VNNI_EVEX(invoke_evex256, __m256i, __mmask8, 32, evexvl, "avx512f,avx512vnni,avx512vl")
  NATIVE_VNNI_EVEX(invoke_evex512, __m512i, __mmask16, 64, evex512, "avx512f,avx512vnni")
#undef NATIVE_VNNI_EVEX
#undef NATIVE_VNNI_CORE_RESULTS

#define NATIVE_VNNI_INT8(Name, V, N) \
  extern "C" native_noinline __attribute__((target("avxvnniint8"))) void Name( \
      inputs<N> const* input, outputs<N, 6>* plain, unsigned, outputs<N, 6>*, outputs<N, 6>*) { \
    NATIVE_VNNI_LOAD(V); \
    V result[] = {native::dpbssd<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native(), native::dpbssds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::int8_t, sizeof(b) / sizeof(std::int8_t), int8>::from_native(b)).to_native(), \
      native::dpbsud<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native(), native::dpbsuds<int8>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int8>::from_native(acc), native::simd<std::int8_t, sizeof(a) / sizeof(std::int8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native(), \
      native::dpbuud<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native(), native::dpbuuds<int8>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int8>::from_native(acc), native::simd<std::uint8_t, sizeof(a) / sizeof(std::uint8_t), int8>::from_native(a), native::simd<std::uint8_t, sizeof(b) / sizeof(std::uint8_t), int8>::from_native(b)).to_native()}; \
    __builtin_memcpy(plain, result, sizeof(result)); \
  }
  NATIVE_VNNI_INT8(invoke_int8_128, __m128i, 16)
  NATIVE_VNNI_INT8(invoke_int8_256, __m256i, 32)
#undef NATIVE_VNNI_INT8
#define NATIVE_VNNI_INT16(Name, V, N) \
  extern "C" native_noinline __attribute__((target("avxvnniint16"))) void Name( \
      inputs<N> const* input, outputs<N, 6>* plain, unsigned, outputs<N, 6>*, outputs<N, 6>*) { \
    NATIVE_VNNI_LOAD(V); \
    V result[] = {native::dpwsud<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native(), native::dpwsuds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::int16_t, sizeof(a) / sizeof(std::int16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native(), \
      native::dpwusd<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native(), native::dpwusds<int16>(native::simd<std::int32_t, sizeof(acc) / sizeof(std::int32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::int16_t, sizeof(b) / sizeof(std::int16_t), int16>::from_native(b)).to_native(), \
      native::dpwuud<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native(), native::dpwuuds<int16>(native::simd<std::uint32_t, sizeof(acc) / sizeof(std::uint32_t), int16>::from_native(acc), native::simd<std::uint16_t, sizeof(a) / sizeof(std::uint16_t), int16>::from_native(a), native::simd<std::uint16_t, sizeof(b) / sizeof(std::uint16_t), int16>::from_native(b)).to_native()}; \
    __builtin_memcpy(plain, result, sizeof(result)); \
  }
  NATIVE_VNNI_INT16(invoke_int16_128, __m128i, 16)
  NATIVE_VNNI_INT16(invoke_int16_256, __m256i, 32)
#undef NATIVE_VNNI_INT16
#undef NATIVE_VNNI_LOAD

  inline std::uint64_t next_random(std::uint64_t& state) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
  }
  template<std::size_t N, std::size_t O>
  using evaluator = void (*)(inputs<N> const*, outputs<N, O>*, unsigned, outputs<N, O>*, outputs<N, O>*);

  template<std::size_t N, std::size_t O>
  bool check_one(inputs<N> const& input, std::array<operation, O> const& operations,
      evaluator<N, O> evaluate, bool masked, unsigned mask) {
    outputs<N, O> plain{}, merged{}, zeroed{};
    evaluate(&input, &plain, mask, &merged, &zeroed);
    auto expected = oracle(input, operations);
    for (std::size_t op = 0; op != O; ++op)
      for (std::size_t lane = 0; lane != N / 4; ++lane) {
        bool active = (mask >> lane) & 1u;
        auto merge_expected = active ? expected[op][lane] : input.accumulator[lane];
        auto zero_expected = active ? expected[op][lane] : 0u;
        if (plain[op][lane] != expected[op][lane] ||
            (masked && (merged[op][lane] != merge_expected || zeroed[op][lane] != zero_expected))) {
          std::fprintf(stderr, "%s %zu-bit lane %zu mask=%04x accumulator=%08x: "
            "plain=%08x expected=%08x merge=%08x expected=%08x zero=%08x expected=%08x\n",
            operations[op].name, N * 8, lane, mask, input.accumulator[lane],
            plain[op][lane], expected[op][lane], merged[op][lane], merge_expected,
            zeroed[op][lane], zero_expected);
          return false;
        }
      }
    return true;
  }

  template<std::size_t N, std::size_t O>
  bool check_width(evaluator<N, O> evaluate, std::array<operation, O> const& operations,
      bool masked, std::uint64_t seed) {
    inputs<N> input{};
    constexpr std::array<std::uint32_t, 10> accumulators{
      0, 1, 0xffffffffu, 0x7fffffffu, 0x80000000u, 0x7ffffffeu,
      0x80000001u, 0xffff0000u, 0x00010000u, 0x55555555u};
    constexpr std::array<unsigned, 8> bytes{0, 1, 127, 128, 129, 254, 255, 85};
    constexpr std::array<unsigned, 8> words{0, 1, 32767, 32768, 32769, 65534, 65535, 21845};
    constexpr unsigned mask_bits = N == 64 ? 16 : 8;
    constexpr unsigned all_masks = (1u << mask_bits) - 1;
    auto check = [&](unsigned mask = 0xffffu) {
      return check_one(input, operations, evaluate, masked, mask);
    };
    // Cross all signedness boundaries with accumulator endpoints. The word
    // cases include the two -32768 products and cancellation before saturation.
    for (unsigned width : {8u, 16u})
      for (unsigned a : width == 8 ? bytes : words)
        for (unsigned b : width == 8 ? bytes : words)
          for (std::uint32_t accumulator : accumulators) {
            input.accumulator.fill(accumulator);
            for (std::size_t byte = 0; byte != N; ++byte) {
              auto shift = width == 16 && (byte & 1u) ? 8u : 0u;
              input.a[byte] = std::uint8_t(a >> shift);
              input.b[byte] = std::uint8_t(b >> shift);
            }
            if (!check()) return false;
          }
    // Move a single byte through each group and register quarter. Unequal
    // multipliers and accumulators expose input reversal or cross-lane mixing.
    for (std::size_t position = 0; position != N; ++position) {
      input.a.fill(0);
      input.a[position] = 0x81;
      for (std::size_t byte = 0; byte != N; ++byte) input.b[byte] = std::uint8_t(7 + byte * 13);
      for (std::size_t lane = 0; lane != N / 4; ++lane)
        input.accumulator[lane] = 0x7fffffffu - std::uint32_t(lane * 71);
      if (!check()) return false;
      auto saved = input.a;
      input.a = input.b;
      input.b = saved;
      if (!check()) return false;
    }
    if (masked) {
      // All representable masks include every ignored high-bit combination for
      // 128-bit results, plus every lane and alternating mask at wider widths.
      for (std::size_t lane = 0; lane != N / 4; ++lane) input.accumulator[lane] = 0x12345678u + lane;
      for (std::size_t byte = 0; byte != N; ++byte) {
        input.a[byte] = std::uint8_t(1 + byte * 17);
        input.b[byte] = std::uint8_t(3 + byte * 31);
      }
      for (unsigned mask = 0; mask <= all_masks; ++mask)
        if (!check(mask)) return false;
    }
    for (unsigned iteration = 0; iteration != 4096; ++iteration) {
      for (std::size_t lane = 0; lane != N / 4; ++lane)
        input.accumulator[lane] = std::uint32_t(next_random(seed));
      for (std::size_t byte = 0; byte != N; ++byte) {
        input.a[byte] = std::uint8_t(next_random(seed));
        input.b[byte] = std::uint8_t(next_random(seed));
      }
      if (!check(unsigned(next_random(seed)))) return false;
    }
    return true;
  }

  inline int run(char const* selected) {
    if (!check_scalar_reference()) return 1;
    auto cpu = native::observe_x86_capabilities();
    std::printf("AVX-VNNI=%u AVX512-VNNI=%u AVX-VNNI-INT8=%u AVX-VNNI-INT16=%u "
      "AVX512F=%u AVX512VL=%u XCR0=%016llx (observed=%u)\n",
      unsigned(cpu.present.has(x86_feature::avxvnni)), unsigned(cpu.present.has(x86_feature::avx512vnni)),
      unsigned(cpu.present.has(x86_feature::avxvnniint8)), unsigned(cpu.present.has(x86_feature::avxvnniint16)),
      unsigned(cpu.present.has(x86_feature::avx512f)), unsigned(cpu.present.has(x86_feature::avx512vl)),
      static_cast<unsigned long long>(cpu.xcr0), unsigned(cpu.xcr0_observed));
    auto execute = [&](native::isa<native::x86> requirements, auto body) {
      auto admission = native::classify_isa(cpu, requirements, NATIVE_TARGET_MINIMUM);
      if (!admission.admitted()) {
        std::printf("SKIP %s: %s; scalar reference and compile-time constraints passed\n", selected, admission.reason());
        return 77;
      }
      if (!body()) return 1;
      std::printf("PASS %s: native execution agrees with the wide scalar oracle\n", selected);
      return 0;
    };
    constexpr std::uint64_t seed = 0x243f6a8885a308d3ull;
    if (std::strcmp(selected, "core") == 0)
      return execute(core, [&] { return check_width(invoke_core128, core_operations, false, seed) &&
        check_width(invoke_core256, core_operations, false, seed); });
    if (std::strcmp(selected, "evex512") == 0)
      return execute(evex512, [&] { return check_width(invoke_evex512, core_operations, true, seed); });
    if (std::strcmp(selected, "evexvl") == 0)
      return execute(evexvl, [&] { return check_width(invoke_evex128, core_operations, true, seed) &&
        check_width(invoke_evex256, core_operations, true, seed); });
    if (std::strcmp(selected, "int8") == 0)
      return execute(int8, [&] { return check_width(invoke_int8_128, int8_operations, false, seed) &&
        check_width(invoke_int8_256, int8_operations, false, seed); });
    if (std::strcmp(selected, "int16") == 0)
      return execute(int16, [&] { return check_width(invoke_int16_128, int16_operations, false, seed) &&
        check_width(invoke_int16_256, int16_operations, false, seed); });
    std::fprintf(stderr, "Unknown VNNI selection '%s'; use core, evex512, evexvl, int8 or int16\n", selected);
    return 2;
  }
}
