// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include "native/config.h"
#include "native/attributes.h"
#include "native/isa.h"
#if NATIVE_HOST_X86
#include <immintrin.h>

namespace native::detail::x86_sha {
  template<isa<x86> Arch, unsigned Selector> requires(Arch.has(x86_feature::sha) && Selector < 4)
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1rnds4(__m128i a, __m128i b) noexcept {
    return _mm_sha1rnds4_epu32(a, b, Selector);
  }

  template<isa<x86> Arch, unsigned Selector, class... Args>
  void sha1rnds4(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1nexte(__m128i a, __m128i b) noexcept {
    return _mm_sha1nexte_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1nexte(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1msg1(__m128i a, __m128i b) noexcept {
    return _mm_sha1msg1_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1msg1(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha1msg2(__m128i a, __m128i b) noexcept {
    return _mm_sha1msg2_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha1msg2(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256rnds2(__m128i a, __m128i b, __m128i c) noexcept {
    return _mm_sha256rnds2_epu32(a, b, c);
  }

  template<isa<x86> Arch, class... Args>
  void sha256rnds2(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256msg1(__m128i a, __m128i b) noexcept {
    return _mm_sha256msg1_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha256msg1(Args...) = delete;

  template<isa<x86> Arch> requires(Arch.has(x86_feature::sha))
  native_nodiscard native_inline native_const native_target("sha")
  __m128i sha256msg2(__m128i a, __m128i b) noexcept {
    return _mm_sha256msg2_epu32(a, b);
  }

  template<isa<x86> Arch, class... Args>
  void sha256msg2(Args...) = delete;

}
#endif
