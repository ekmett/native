#pragma once
#include "simd/vec.h"

#if SIMD_HOST_X86 && (!defined(SIMD_PROFILE) || SIMD_PROFILE != 0)
namespace simd {
  /// Parallel bit deposit (PDEP): place the low popcount(mask) bits into mask's set positions, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  simd_nodiscard simd_inline simd_const __attribute__((target("bmi2")))
  std::uint64_t pdep(std::uint64_t value, std::uint64_t mask) noexcept {
    return _pdep_u64(value, mask);
  }

  /// Parallel bit extract (PEXT): gather mask's selected bits into the low bits, in order.
  template<isa Arch> requires(Arch.has(x86_feature::bmi2))
  simd_nodiscard simd_inline simd_const __attribute__((target("bmi2")))
  std::uint64_t pext(std::uint64_t value, std::uint64_t mask) noexcept {
    return _pext_u64(value, mask);
  }
}
#endif

#define SIMD_BACKEND_BODY "simd/packing_body.h"
#include "simd/simd/for_each_backend.h"
#undef SIMD_BACKEND_BODY
