// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
#if NATIVE_CONSTEXPR_INTERFACE == 0
#include <native/x86/bmi1.h>
#include <native/x86/bmi2.h>
#include <native/x86/popcnt.h>
#include <native/x86/lzcnt.h>
#include <native/x86/crc32c.h>
#elif NATIVE_CONSTEXPR_INTERFACE == 1
import native.x86.bmi1;
import native.x86.bmi2;
import native.x86.popcnt;
import native.x86.lzcnt;
import native.x86.crc32c;
#else
import native;
#endif
#include "checks.h"
int main() { return 0; }
