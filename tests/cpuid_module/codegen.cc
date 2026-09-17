// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import simd.wait;
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__WAITPKG__) || defined(__MWAITX__)
#error "The codegen consumer must compile for baseline x86-64"
#endif
// Compiled and disassembled only; these entries are never run by the fixture.
extern "C" {
  void probe_monitorx(volatile void * p) noexcept { simd::mwaitx::monitor(p); }
  void probe_mwaitx(unsigned timer) noexcept { simd::mwaitx::mwait(timer); }
  void probe_umonitor(volatile void * p) noexcept { simd::umwait::monitor(p); }
  unsigned char probe_umwait(unsigned timer) noexcept { return simd::umwait::mwait(timer); }
  void probe_pause() noexcept { simd::spin::mwait(); }
}
