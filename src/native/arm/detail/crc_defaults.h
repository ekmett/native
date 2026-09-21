// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

// Only the owning module adds defaults. Standalone headers stay independent
// of translation-unit baseline flags, and retain their explicit ISA arguments.
/// \cond
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native {
  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint32_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint32_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32(std::uint32_t accumulator, std::uint64_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32(std::uint32_t accumulator, std::uint64_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE, class A, class B>
  void crc32(A, B);

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint8_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint16_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint32_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(Arch.has(arm_feature::crc))
  constexpr std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE> requires(!Arch.has(arm_feature::crc))
  consteval std::uint32_t crc32c(std::uint32_t accumulator, std::uint64_t value) noexcept;

  template<isa Arch=NATIVE_BASELINE, class A, class B>
  void crc32c(A, B);

}
#endif
/// \endcond
