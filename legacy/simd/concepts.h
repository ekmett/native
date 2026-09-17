#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

    \def simd_ensures(concept,x)

    \brief statically checks that a concept is satisfied by the type of an expression

    \details

      This is expected most often to be used inline in a return statement:

      \code{.cpp}
      auto foo(integral auto x, integral auto y) {
        return simd_ensures(integral, x + y);
      }
      \endcode

    \param concept the name of a C++ concept to check
    \param x the expression to check returns a result that satisfies it
    \returns x */

#include <concepts>

#define simd_ensures(concept, x) \
  ( \
    []<typename T> static constexpr noexcept { \
      static_assert(concept<T>, #concept " not satisfied"); \
    }.template operator()<decltype(x)>(), \
    (x) \
  )

#if defined(SIMD_TESTING) || defined(SIMD_TESTING_CONCEPTS)
#include <string>

// Sample functions using simd_ensures for test cases
static auto add_integers(std::integral auto x, std::integral auto y) {
  return simd_ensures(std::integral, x + y);
}

static auto add_floats(std::floating_point auto x, std::floating_point auto y) {
  return simd_ensures(std::floating_point, x + y);
}

template <typename T>
concept Stringifiable = requires(T a) {
  { std::to_string(a) } -> std::convertible_to<std::string>;
};

static auto stringifyable_addition(Stringifiable auto x, Stringifiable auto y) {
  return simd_ensures(Stringifiable, x + y);
}

TEST_CASE("concepts","[concepts]") {
  SECTION("Test integral addition with simd_ensures") {
    CHECK(add_integers(2, 3) == 5);
    CHECK(add_integers(-10, 4) == -6);
  }

  SECTION("Test floating-point addition with simd_ensures") {
    CHECK(add_floats(1.5, 2.0) == Catch::Approx(3.5));
    CHECK(add_floats(-10.0, 4.5) == Catch::Approx(-5.5));
  }

  SECTION("Test custom concept Stringifiable with simd_ensures") {
    CHECK(stringifyable_addition(10, 20) == 30);
  }

  SECTION("Test simd_ensures with non-matching concept should compile-time fail") {
      // Intentionally left commented as examples of cases that should fail to compile
      // REQUIRE(add_integers(2.5, 3.5) == 6.0); // Should fail on non-integral inputs
      // REQUIRE(add_floats(1, 2) == 3);         // Should fail on non-floating point inputs
  }
}
#endif
