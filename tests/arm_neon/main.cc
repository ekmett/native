// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
import native.arm.neon;
#include "reference.h"

constexpr auto architecture = native::feature_closure(native::arm_feature::neon);
template <class T, std::size_t N> using V = native::simd<T, N, architecture>;
#pragma clang attribute push(__attribute__((target("neon"))), apply_to = function)

// Logical lane packing also covers the established padded int32x2 shape.
template <class Vector> constexpr auto lanes(Vector value) {
  std::array<typename Vector::value_type, Vector::lanes> result{};
  value.store(result.data());
  return result;
}

template <class T, class S, std::size_t N, class F>
constexpr bool constant_binary(F operation, reference::operation kind, bool round, bool saturate) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < values.size(); ++trial) {
    std::array<T, N> a{}, expected{};
    std::array<S, N> b{};
    for (unsigned i = 0; i < N; ++i) {
      a[i] = values[(trial + i) % values.size()];
      b[i] = S(values[(trial + i + 1) % values.size()]);
      expected[i] = reference::binary(kind, a[i], b[i], round, saturate).value;
    }
    if (lanes(operation(V<T, N>::load(a.data()), V<S, N>::load(b.data()))) != expected)
      return false;
  }
  return true;
}

template <class T, class D, std::size_t N, class Low, class High>
constexpr bool constant_narrow(Low low_operation, High high_operation) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < values.size(); ++trial) {
    std::array<T, N> a{};
    std::array<D, N> bottom{}, expected{};
    std::array<D, 2 * N> combined{};
    for (unsigned i = 0; i < N; ++i) {
      a[i] = values[(trial + i) % values.size()];
      bottom[i] = D(i + 7);
      expected[i] = reference::clamp<D>(a[i]).value;
      combined[i] = bottom[i];
      combined[i + N] = expected[i];
    }
    auto input = V<T, N>::load(a.data());
    if (lanes(low_operation(input)) != expected ||
        lanes(high_operation(V<D, N>::load(bottom.data()), input)) != combined)
      return false;
  }
  return true;
}

#include "constant_cases.h"

std::uint64_t state = 0x79842e0bb81df063ull;

std::uint64_t random_word() {
  state ^= state << 13;
  state ^= state >> 7;
  state ^= state << 17;
  return state;
}

std::uint64_t fpsr() {
  std::uint64_t value;
  asm volatile("mrs %0, FPSR" : "=r"(value));
  return value;
}

void set_fpsr(std::uint64_t value) {
  asm volatile("msr FPSR, %0" : : "r"(value));
}

constexpr std::uint64_t qc = 1ull << 27;

template <class T, class S, std::size_t N, class F>
bool check_binary(char const *name, F operation, reference::operation kind, bool round,
                  bool saturate) {
  auto values = reference::boundaries<T>();
  // Counts exercise low-byte decoding, both width boundaries and wrapping high bits.
  constexpr int counts[]{-128, -65, -64, -63, -33, -32, -31, -17, -16,  -15, -9,
                         -8,   -7,  -1,  0,   1,   7,   8,   15,  16,   31,  32,
                         63,   64,  65,  127, 128, 255, 256, 257, -256, -257};
  for (unsigned trial = 0; trial < 256; ++trial) {
    std::array<T, N> a{}, expected{};
    std::array<S, N> b{};
    bool saturated = false;
    for (unsigned i = 0; i < N; ++i) {
      a[i] = trial < 64 ? values[(trial / 8 + i) % values.size()] : T(random_word());
      b[i] = kind == reference::operation::shift ? S(counts[(trial + i) % std::size(counts)])
             : trial < 64                        ? S(values[(trial + i) % values.size()])
                                                 : S(random_word());
      auto result = reference::binary(kind, a[i], b[i], round, saturate);
      expected[i] = result.value;
      saturated |= result.saturated;
    }
    auto left = V<T, N>::load(a.data());
    auto right = V<S, N>::load(b.data());
    set_fpsr(fpsr() & ~qc);
    auto result = operation(left, right);
    auto status = fpsr();
    if (lanes(result) != expected || bool(status & qc) != saturated) {
      std::printf("%s trial %u: value or QC mismatch\n", name, trial);
      return false;
    }
    set_fpsr(status & ~qc);
    (void)operation(left, right);
    if (bool(fpsr() & qc) != saturated) {
      std::printf("%s discarded result lost QC effect\n", name);
      return false;
    }
    set_fpsr(status | qc);
    (void)operation(left, right);
    if (!(fpsr() & qc)) {
      std::printf("%s cleared sticky QC\n", name);
      return false;
    }
  }
  return true;
}

template <class T, class D, std::size_t N, class Low, class High>
bool check_narrow(char const *name, Low low_operation, High high_operation) {
  auto values = reference::boundaries<T>();
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<T, N> a{};
    std::array<D, N> bottom{}, expected{};
    std::array<D, 2 * N> combined{};
    bool saturated = false;
    for (unsigned i = 0; i < N; ++i) {
      a[i] = trial < 8 ? values[(trial + i) % values.size()] : T(random_word());
      bottom[i] = D(random_word());
      auto result = reference::clamp<D>(a[i]);
      expected[i] = result.value;
      saturated |= result.saturated;
      combined[i] = bottom[i];
      combined[i + N] = expected[i];
    }
    auto input = V<T, N>::load(a.data());
    auto low = V<D, N>::load(bottom.data());
    set_fpsr(fpsr() & ~qc);
    auto small = low_operation(input);
    auto status = fpsr();
    if (lanes(small) != expected || bool(status & qc) != saturated) {
      std::printf("%s low trial %u mismatch\n", name, trial);
      return false;
    }
    set_fpsr(status & ~qc);
    auto full = high_operation(low, input);
    status = fpsr();
    if (lanes(full) != combined || bool(status & qc) != saturated) {
      std::printf("%s high trial %u mismatch\n", name, trial);
      return false;
    }
  }
  return true;
}

bool run() {
#include "cases.h"
  return true;
}

#pragma clang attribute pop

int main() {
  if (!native::classify_isa(native::observe_arm_capabilities(), architecture).admitted())
    return 77;
  auto old = fpsr();
  bool ok = run();
  set_fpsr(old);
  return ok ? 0 : 1;
}
