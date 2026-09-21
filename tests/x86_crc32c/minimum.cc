// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Include the implementation first to check that it supplies its own inputs.
#include <native/x86/crc32c.h>
#include <native/targets.h>

static_assert(static_cast<unsigned>(native::x86_feature::waitpkg) == 29);
static_assert(native::feature_closure(native::x86_feature::crc32) ==
              native::isa<native::x86>(native::x86_feature::crc32));
#if NATIVE_TEST_MINIMUM == 1
#ifndef __CRC32__
#error CRC32 compiler feature was not enabled.
#endif
static_assert(NATIVE_TARGET_MINIMUM.has(native::x86_feature::crc32));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::x86_feature::sse42));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::x86_feature::popcnt));
static_assert(!NATIVE_TARGET_MINIMUM.has(native::x86_feature::avx));
#elif NATIVE_TEST_MINIMUM == 2
#ifndef __SSE4_2__
#error SSE4.2 compiler feature was not enabled.
#endif
// Admission retains the declared compiler bundle's conservative closure even
// when command-line options explicitly disable individual members of it.
static_assert(NATIVE_TARGET_MINIMUM.has(native::x86_feature::sse42));
static_assert(NATIVE_TARGET_MINIMUM.has(native::x86_feature::crc32));
#else
static_assert(!NATIVE_TARGET_MINIMUM.has(native::x86_feature::crc32));
#endif
