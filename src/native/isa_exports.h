// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Include after a named-module declaration; isa.h definitions belong to the GMF.
#include "native/isa_capability_exports.h"
export namespace native {
  using ::native::arch;
  using ::native::abi_lookup;
  using ::native::target;
}
// Source-target macro validation also runs in importing translation units.
export namespace native::detail {
  using ::native::detail::source_isa;
  using ::native::detail::known_features;
  using ::native::detail::x86_features;
  using ::native::detail::arm_features;
}
