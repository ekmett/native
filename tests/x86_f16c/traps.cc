// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#include <ucontext.h>
import native.x86.f16c;
#include "checks.h"

namespace {
volatile std::sig_atomic_t expected_flags;
void trapped(int signal, siginfo_t*, void* context) {
  auto* state = static_cast<ucontext_t*>(context);
  unsigned flags = state->uc_mcontext.fpregs->mxcsr & 0x3f;
  _exit(signal == SIGFPE && flags == unsigned(expected_flags) ? 0 : 64 + flags);
}
}
int main() {
  using namespace f16c_fixture;
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, arch).admitted()) {
    std::puts("SKIP: F16C CPU/OS state unavailable"); return 77;
  }
  unsigned saved = _mm_getcsr();
  struct example { std::uint32_t bits; unsigned unmask, flags; bool widening; };
  constexpr example cases[] = {
    {0x7f800001, 0x80, 1, false}, // invalid
    {0x7c01, 0x80, 1, true},
    {0x3f801000, 0x1000, 0x20, false}, // precision, despite immediate bit 3
    {0x47800000, 0x400, 0x08, false}, // unmasked overflow
    {0x33000000, 0x800, 0x10, false}, // unmasked underflow precedes precision
    {0x33800000, 0x800, 0x10, false}, // exact tiny result, underflow unmasked
    {1, 0x100, 0x02, false}, // denormal input, DM unmasked
    {1, 0x800, 0x32, false}, // DM masked, UM unmasked: DE+UE+PE
    {1, 0x1000, 0x32, false} // DM masked, PM unmasked: DE+UE+PE
  };
  for (unsigned index = 0; index != std::size(cases); ++index) {
    auto c = cases[index];
    for (unsigned lanes : {1u, 4u, 8u}) {
      auto child = fork();
      if (child < 0) { std::perror("fork"); return 1; }
      if (!child) {
        expected_flags = c.flags;
        struct sigaction action{};
        action.sa_sigaction = trapped;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        if (sigaction(SIGFPE, &action, nullptr)) _exit(3);
        unsigned csr = controls(saved, 0, false, false) & ~c.unmask;
        if (c.widening) (void)discarded_widen(std::uint16_t(c.bits), csr, lanes);
        else (void)discarded_narrow<8>(c.bits, csr, lanes);
        _exit(4); // A discarded result must not remove the unmasked exception.
      }
      int status = 0;
      if (waitpid(child, &status, 0) != child || !WIFEXITED(status) || WEXITSTATUS(status)) {
        std::printf("trap case=%u lanes=%u wait-status=%x\n", index, lanes, status);
        return 1;
      }
    }
  }
  if (_mm_getcsr() != saved) return 1;
  std::puts("27 unmasked F16C exceptions retained with exact saved MXCSR flags; parent controls unchanged");
  return 0;
}
