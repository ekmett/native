// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.x86.wait;
#if (!NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__))) || defined(__WAITPKG__) || defined(__MWAITX__)
#error Common consumer inherited AVX-512 or optional wait ISA flags
#endif
// Compiled and disassembled only; these entries are never run by the fixture.
extern "C" {
  void probe_monitorx(volatile void * p) noexcept { native::mwaitx::monitor(p); }
  void probe_mwaitx(unsigned timer) noexcept { native::mwaitx::mwait(timer); }
  void probe_umonitor(volatile void * p) noexcept { native::umwait::monitor(p); }
  unsigned char probe_umwait(unsigned long long timer) noexcept { return native::umwait::mwait(timer); }
  void probe_pause() noexcept { native::spin::mwait(); }
}

extern "C" [[gnu::target("waitpkg")]] unsigned char probe_tpause(unsigned long long deadline) noexcept {
  return native::tpause<native::x86_feature::waitpkg>(deadline);
}
