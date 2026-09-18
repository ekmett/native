// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <concepts>
#include <type_traits>

namespace simd::test {
  template<class V>
  concept native_bridge = std::is_trivially_copyable_v<V>
    && std::convertible_to<V,typename V::native_type>
    && std::convertible_to<typename V::native_type,V>
    && std::is_nothrow_constructible_v<V,typename V::native_type>;

  // A native projection must not silently introduce BF16 vector arithmetic.
  template<class V> concept bf16_storage_only =
    !requires(V a) { a+a; } && !requires(V a) { a-a; }
    && !requires(V a) { a*a; } && !requires(V a) { a/a; }
    && !requires(V a) { a+1.0f; } && !requires(V a) { 1.0f+a; }
    && !requires(V a, typename V::native_type n) { a+n; }
    && !requires(V a, typename V::native_type n) { n+a; }
    && !requires(V a) { a==a; } && !requires(V a) { a<a; }
    && !requires(V a) { -a; } && !requires(V a) { +a; };
}
