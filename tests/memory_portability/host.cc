#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>
import simd.memory;

namespace {
  void require(bool condition) {
    if (!condition) {
      std::fputs("portable memory ownership check failed\n", stderr);
      std::abort();
    }
  }
  struct nontrivial { ~nontrivial() {} };
  static_assert(simd::trivially_destructible<int>);
  static_assert(simd::trivially_destructible<char[]>);
  static_assert(!simd::trivially_destructible<nontrivial>);
  static_assert(sizeof(simd::unique_c_ptr<int>) == sizeof(int *));
  static_assert(!std::is_copy_constructible_v<simd::unique_str>);
  static_assert(std::is_nothrow_move_constructible_v<simd::unique_str>);
  static_assert(noexcept(simd::dup("copy")));
}

int main() {
  for (unsigned iteration = 0; iteration < 128; ++iteration) {
    simd::unique_c_ptr<int[]> values(static_cast<int *>(std::malloc(16 * sizeof(int))));
    require(bool(values));
    for (int i = 0; i < 16; ++i) values[i] = i + int(iteration);
    int * original = values.get();
    auto moved = std::move(values);
    require(!values && moved.get() == original && moved[15] == 15 + int(iteration));
    simd::unique_c_ptr<int[]> destination(static_cast<int *>(std::malloc(4 * sizeof(int))));
    require(bool(destination));
    destination = std::move(moved); // Releases its previous allocation.
    require(!moved && destination.get() == original);
    auto * replacement = static_cast<int *>(std::malloc(2 * sizeof(int)));
    require(replacement != nullptr);
    replacement[0] = 42;
    destination.reset(replacement); // Releases the transferred allocation.
    require(destination.get() == replacement && destination[0] == 42);
    auto * released = destination.release();
    require(!destination && released == replacement);
    simd::c_free{}(released);
    destination.reset();
  }
  simd::c_free{}(static_cast<int *>(nullptr));

  char original[] = "independent copy";
  auto first = simd::dup(original);
  auto second = simd::dup(original);
  require(first && second && first.get() != second.get() && first.get() != original);
  require(std::strcmp(first.get(), original) == 0 && std::strcmp(second.get(), original) == 0);
  original[0] = 'X';
  require(first.get()[0] == 'i' && second.get()[0] == 'i');
  auto moved = std::move(first);
  require(!first && moved && std::strcmp(moved.get(), "independent copy") == 0);
  auto empty = simd::dup("");
  require(empty && empty.get()[0] == '\0');
  second.reset();
  require(!second && std::strcmp(moved.get(), "independent copy") == 0);
  moved.reset();
  std::puts("portable allocation, free, move, reset, release and independent duplication passed");
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
