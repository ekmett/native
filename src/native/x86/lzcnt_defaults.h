// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/targets.h"

// Include only in this operation module's global module fragment, after its
// public header. Defaults belong to the provider, never to a textual consumer.
#if NATIVE_HOST_X86
namespace native {
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::lzcnt))
  constexpr std::uint16_t lzcnt(std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::lzcnt))
  consteval std::uint16_t lzcnt(std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::lzcnt))
  constexpr std::uint32_t lzcnt(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::lzcnt))
  consteval std::uint32_t lzcnt(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::lzcnt))
  constexpr std::uint64_t lzcnt(std::uint64_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::lzcnt))
  consteval std::uint64_t lzcnt(std::uint64_t value) noexcept;

}
#endif
