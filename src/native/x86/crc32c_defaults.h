// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "native/targets.h"

// Include only in this operation module's global module fragment, after its
// public header. Defaults belong to the provider, never to a textual consumer.
#if NATIVE_HOST_X86
namespace native {
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::crc32))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::crc32))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::crc32))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::crc32))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::crc32))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::crc32))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept;

#if defined(__x86_64__) || defined(_M_X64)
  template<isa Arch = NATIVE_BASELINE> requires(Arch.has(x86_feature::crc32))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept;

  template<isa Arch = NATIVE_BASELINE> requires(!Arch.has(x86_feature::crc32))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept;

#endif
}
#endif
