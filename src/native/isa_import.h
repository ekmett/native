// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Import before parsing shared headers in a module's global fragment. This
// gives Clang one owner for ISA properties while keeping header declarations
// attached to the global module and preserving their public type identity.
import native.isa;
