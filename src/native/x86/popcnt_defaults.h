// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/targets.h"

// Include only in this operation module's global module fragment, after its
// public header. Defaults belong to the provider, never to a textual consumer.
#if NATIVE_HOST_X86
namespace native {
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::popcnt))
  std::uint16_t popcnt(std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::popcnt))
  std::uint32_t popcnt(std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::popcnt))
  std::uint64_t popcnt(std::uint64_t value) noexcept;

}
#endif
