#pragma once
// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cfenv>
#include <cstdint>
#include <cstdlib>
#if defined(_M_X64) || defined(__x86_64__)
#include <xmmintrin.h>
#endif

// Test-only FP environment fixture. No dependency on a production setup API.
namespace simd::test {
  enum class fp_mode { gradual, flush };
  struct fp_state {
    std::uint64_t control, status;
    friend bool operator==(fp_state const &, fp_state const &) = default;
  };
  inline fp_state read_fp_state() noexcept {
#if defined(_M_X64) || defined(__x86_64__)
    auto word = _mm_getcsr();
    return {word & ~63u, word & 63u};
#elif defined(__aarch64__) || defined(__arm64__)
    fp_state state;
    __asm__ volatile("mrs %0, fpcr" : "=r"(state.control) : : "memory");
    __asm__ volatile("mrs %0, fpsr" : "=r"(state.status) : : "memory");
    return state;
#else
#error The numerical fixture requires x86-64 or ARM64 controls.
#endif
  }
  inline void write_fp_state(fp_state state) noexcept {
#if defined(_M_X64) || defined(__x86_64__)
    _mm_setcsr(static_cast<unsigned>(state.control | state.status));
#else
    __asm__ volatile("msr fpcr, %0\n\tisb" : : "r"(state.control) : "memory");
    __asm__ volatile("msr fpsr, %0" : : "r"(state.status) : "memory");
#endif
  }
  struct fp_scope {
  private:
    std::fenv_t environment_{};
    fp_state previous_, requested_;
  public:
    explicit fp_scope(fp_mode mode) : previous_(read_fp_state()) {
      if (std::fegetenv(&environment_) || std::fesetround(FE_TONEAREST)) std::abort();
#if defined(_M_X64) || defined(__x86_64__)
      requested_ = {(previous_.control & ~std::uint64_t(0xe040)) | 0x1f80u, 0};
      if (mode == fp_mode::flush) requested_.control |= 0x8040u;
#else
      constexpr std::uint64_t clear = 7u | (0x1full << 8) | (1ull << 15) |
        (3ull << 22) | (1ull << 24);
      requested_ = {previous_.control & ~clear, 0};
      if (mode == fp_mode::flush) requested_.control |= 1ull << 24;
#endif
      write_fp_state(requested_);
      if (read_fp_state() != requested_) std::abort();
    }
    ~fp_scope() noexcept {
      if (std::fesetenv(&environment_)) std::abort();
      write_fp_state(previous_);
    }
    fp_scope(fp_scope const &) = delete;
    fp_scope & operator=(fp_scope const &) = delete;
    bool controls_match() const noexcept { return read_fp_state().control == requested_.control; }
  };
}
