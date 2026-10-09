// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#include <native/isa.h>
import native.features;

namespace {
  constexpr char const * boolean(bool value) noexcept {
    return value ? "true" : "false";
  }

  void describe(std::FILE * out, auto const & cpu) noexcept {
    constexpr auto family = decltype(cpu.present)::family;
    std::fprintf(out, "{\n  \"architecture\": \"%s\",\n",
      family == native::x86 ? "x86" : "arm");
    if constexpr (requires { cpu.xcr0; })
      std::fprintf(out, "  \"xcr0\": \"0x%llx\",\n  \"xcr0_observed\": %s,\n",
        static_cast<unsigned long long>(cpu.xcr0), boolean(cpu.xcr0_observed));
    std::fputs("  \"features\": {\n", out);
    bool first = true;
    // Use the detector's registry rather than maintaining a second feature list.
    for (auto const & feature : native::detail::feature_registry<family>) {
      auto const admission = native::classify_isa(cpu, feature.value);
      std::fprintf(out, "%s    \"%.*s\": {\"observed\": %s, \"present\": %s, \"admitted\": %s}",
        first ? "" : ",\n", int(feature.spelling.size()), feature.spelling.data(),
        boolean(cpu.observed.has(feature.value)), boolean(cpu.present.has(feature.value)),
        boolean(admission.admitted()));
      first = false;
    }
    std::fputs("\n  }\n}\n", out);
  }
}

int main(int argc, char ** argv) noexcept {
  if (argc != 2) return 1;
  auto * out = std::fopen(argv[1], "w");
  if (!out) return 1;
  describe(out, native::observe_cpu());
  auto const failed = std::ferror(out);
  return std::fclose(out) != 0 || failed ? 1 : 0;
}
