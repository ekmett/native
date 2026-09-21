#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include "support/profile.h"
import native.wide;

namespace {
  enum class failure { none, operation, construction, assignment, move };
  struct events {
    int constructions = 0;
    int operations = 0;
    int assignments = 0;
    int cleanups = 0;
    int outer_cleanups = 0;
  };
  events * active = nullptr;
  struct exception {};
  struct cleanup {
    int * count;
    ~cleanup() noexcept { ++*count; }
  };
  void check(bool condition) {
    if (!condition) {
      std::fputs("wide generic exception/side-effect check failed\n", stderr);
      std::abort();
    }
  }
  template<bool Throws>
  void observe(int events::* count) noexcept(!Throws) {
    if (!active) return;
    ++(active->*count);
    cleanup guard{&active->cleanups};
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    if constexpr (Throws) throw exception{};
#endif
  }
  struct mask { bool selected; };

  template<failure Failure = failure::none, bool BoolMask = false>
  struct value {
    int data;
    value() noexcept(Failure != failure::construction) : data(0) {
      observe<Failure == failure::construction>(&events::constructions);
    }
    explicit value(int x) noexcept : data(x) {}
    value(value const &) noexcept = default;
    value(value && other) noexcept(Failure != failure::move) : data(other.data) {}
    value & operator=(value const & other) noexcept(Failure != failure::assignment) {
      observe<Failure == failure::assignment>(&events::assignments);
      data = other.data;
      return *this;
    }
    friend auto operator==(value const & a, value const & b) noexcept {
      if constexpr (BoolMask) return a.data == b.data;
      else return mask{a.data == b.data};
    }
    friend value abs(value const & a) noexcept(Failure != failure::operation) {
      observe<Failure == failure::operation>(&events::operations);
      return value(a.data);
    }
    friend value sqrt(value const & a) noexcept(Failure != failure::operation) {
      observe<Failure == failure::operation>(&events::operations);
      return value(a.data);
    }
    friend value atan2(value const & y, value const & x) noexcept(Failure != failure::operation) {
      observe<Failure == failure::operation>(&events::operations);
      return value(y.data - x.data);
    }
    friend value fma(value const & a, value const & b, value const & c)
        noexcept(Failure != failure::operation) {
      observe<Failure == failure::operation>(&events::operations);
      return value(a.data + b.data + c.data);
    }
    friend value select(mask m, value const & a, value const & b)
        noexcept(Failure != failure::operation) {
      observe<Failure == failure::operation>(&events::operations);
      return value(m.selected ? a.data : b.data);
    }
  };

  template<failure Failure, bool BoolMask, std::size_t N>
  void specifications() {
    using V = value<Failure,BoolMask>;
    using W = native::wide<V,N>;
    using M = native::wide<decltype(V{} == V{}),N>;
    // The standard-library empty-array representation can still affect object
    // construction/move traits; do not assume those from the lane count alone.
    constexpr bool storage = std::is_nothrow_default_constructible_v<W> &&
      std::is_nothrow_move_constructible_v<W>;
    constexpr bool ordinary = storage &&
      (N == 0 || (Failure != failure::operation && Failure != failure::assignment));
    constexpr bool selection = storage &&
      (N == 0 || (Failure != failure::assignment &&
        (BoolMask || Failure != failure::operation)));
    static_assert(noexcept(native::abs(std::declval<W const &>())) == ordinary);
    static_assert(noexcept(native::sqrt(std::declval<W const &>())) == ordinary);
    static_assert(noexcept(native::atan2(std::declval<W const &>(),
      std::declval<W const &>())) == ordinary);
    static_assert(noexcept(native::fma(std::declval<W const &>(),
      std::declval<W const &>(),std::declval<W const &>())) == ordinary);
    static_assert(noexcept(native::select(std::declval<M const &>(),
      std::declval<W const &>(),std::declval<W const &>())) == selection);
  }

  template<class F>
  void side_effects(F action, int operations, int assignments, int constructions) {
    events state;
    active = &state;
    action();
    active = nullptr;
    check(state.operations == operations && state.assignments == assignments &&
      state.constructions == constructions &&
      state.cleanups == operations + assignments + constructions);
  }

#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  template<class F>
  void catches(F action) {
    events state;
    active = &state;
    bool caught = false;
    try {
      cleanup guard{&state.outer_cleanups};
      action();
    } catch (exception const &) {
      caught = true;
    }
    active = nullptr;
    check(caught && state.outer_cleanups == 1 && state.cleanups > 0);
  }
#endif

  template<failure Failure, bool BoolMask>
  void checks() {
    specifications<Failure,BoolMask,0>();
    specifications<Failure,BoolMask,1>();
    specifications<Failure,BoolMask,2>();
    using V = value<Failure,BoolMask>;
    using W = native::wide<V,2>;
    using M = decltype(V{} == V{});
    W a{V(4),V(9)}, b{V(1),V(2)}, c{V(3),V(4)};
    native::wide<M,2> masks{M{true},M{false}};
    if constexpr (Failure == failure::none) {
      side_effects([&] { (void)native::abs(a); },2,2,2);
      side_effects([&] { (void)native::sqrt(a); },2,2,2);
      side_effects([&] { (void)native::fma(a,b,c); },2,2,2);
      side_effects([&] { (void)native::atan2(a,b); },2,2,2);
      side_effects([&] { (void)native::select(masks,a,b); },BoolMask ? 0 : 2,2,2);
    }
    if constexpr (BoolMask && Failure == failure::operation)
      side_effects([&] { (void)native::select(masks,a,b); },0,2,2);
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    if constexpr (Failure == failure::operation ||
        Failure == failure::construction || Failure == failure::assignment) {
      catches([&] { (void)native::abs(a); });
      catches([&] { (void)native::sqrt(a); });
      catches([&] { (void)native::fma(a,b,c); });
      catches([&] { (void)native::atan2(a,b); });
      if constexpr (!(BoolMask && Failure == failure::operation))
        catches([&] { (void)native::select(masks,a,b); });
    }
#endif
  }

  template<std::size_t L>
  void fixed_raw_specifications() {
    using V = test_vec<float,L>;
    using W = native::wide<V,2>;
    using M = native::wide<typename V::mask,2>;
    static_assert(noexcept(native::abs(std::declval<W const &>())));
    static_assert(noexcept(native::sqrt(std::declval<W const &>())) == noexcept(sqrt(std::declval<V>())));
    static_assert(noexcept(native::fma(std::declval<W const &>(),
      std::declval<W const &>(),std::declval<W const &>())) ==
      noexcept(fma(std::declval<V>(),std::declval<V>(),std::declval<V>())));
    static_assert(noexcept(native::select(std::declval<M const &>(),
      std::declval<W const &>(),std::declval<W const &>())) ==
      noexcept(select(std::declval<typename V::mask>(),std::declval<V>(),std::declval<V>())));
    native::wide<V,0> empty;
    native::wide<typename V::mask,0> no_masks;
    (void)native::abs(empty);
    (void)native::sqrt(empty);
    (void)native::fma(empty,empty,empty);
    (void)native::select(no_masks,empty,empty);
  }
}

int main() {
  checks<failure::none,false>();
  checks<failure::none,true>();
  checks<failure::operation,false>();
  checks<failure::operation,true>();
  checks<failure::construction,false>();
  checks<failure::assignment,false>();
  checks<failure::assignment,true>();
  // Return-value move may be elided, so qualify its promise at compile time.
  specifications<failure::move,false,0>();
  specifications<failure::move,false,2>();
  fixed_raw_specifications<1>();
#if defined(__AVX2__) || defined(__ARM_NEON)
  fixed_raw_specifications<4>();
#endif
#if defined(__AVX2__)
  fixed_raw_specifications<8>();
#endif
#if defined(__AVX512F__) && defined(__AVX512DQ__)
  fixed_raw_specifications<16>();
#endif
  std::puts("wide generic construction, ADL, assignment and cleanup passed");
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
