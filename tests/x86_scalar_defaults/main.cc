// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
#include <native/targets.h>
#if defined(NATIVE_SCALAR_HEADER_FIRST)
#include <native/x86/bmi1.h>
#include <native/x86/bmi2.h>
#include <native/x86/popcnt.h>
#include <native/x86/lzcnt.h>
#include <native/x86/crc32c.h>
#endif
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
import native.x86.f16c;
constexpr native::isa expected = NATIVE_BASELINE;
#include "checks.h"
int main() { return 0; }
