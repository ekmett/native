// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included in the hub purview after the separately owned wide module is imported.
#if NATIVE_HOST_X86
export namespace native {
  /// Evaluate the unchanged array exp graph under the common callee ABI policy.
  /// Binary32 callees require only the raw backend, including result construction.
  /// The caller's complete architecture, lane count and pack extent are retained.
#define NATIVE_EMIT_WIDE_EXP(i,name) \
  NATIVE_TARGET_PUSH(name) \
  template<bool Flush=false,std::size_t L,std::size_t N,isa Arch> \
    requires (detail::exp_target<Arch> == i) \
  native_nodiscard native_inline constexpr wide<simd<float,L,Arch>,N> \
  exp(wide<simd<float,L,Arch>,N> const & input) \
      noexcept(noexcept(wide<simd<float,L,Arch>,N>{::native::exp<Flush>(input.registers)})) { \
    return wide<simd<float,L,Arch>,N>{::native::exp<Flush>(input.registers)}; \
  } \
  NATIVE_TARGET_POP()
  NATIVE_EXP_TARGETS(NATIVE_EMIT_WIDE_EXP)
#undef NATIVE_EMIT_WIDE_EXP
}
#endif
