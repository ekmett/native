// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included in the hub purview after the separately owned wide module is imported.
#if SIMD_HOST_X86
export namespace simd {
  /// Evaluate the unchanged array exp graph under the common callee ABI policy.
  /// Binary32 callees require only the raw backend, including result construction.
  /// The caller's complete architecture, lane count and pack extent are retained.
#define SIMD_EMIT_WIDE_EXP(i,name) \
  SIMD_TARGET_PUSH(name) \
  template<bool Flush=false,std::size_t L,std::size_t N,architecture Arch> \
    requires (detail::exp_target<Arch> == i) \
  simd_nodiscard simd_inline constexpr wide<vec<float,L,Arch>,N> \
  exp(wide<vec<float,L,Arch>,N> const & input) \
      noexcept(noexcept(wide<vec<float,L,Arch>,N>{::simd::exp<Flush>(input.registers)})) { \
    return wide<vec<float,L,Arch>,N>{::simd::exp<Flush>(input.registers)}; \
  } \
  SIMD_TARGET_POP()
  SIMD_EXP_TARGETS(SIMD_EMIT_WIDE_EXP)
#undef SIMD_EMIT_WIDE_EXP
}
#endif
