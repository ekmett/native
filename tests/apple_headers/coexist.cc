// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#if SIMD_APPLE_HEADER_FIRST
#include <simd/simd.h> // Apple SDK, not a project forwarding header.
#include <simd/vec.h>
#else
#include <simd/vec.h>
#include <simd/simd.h>
#endif

#include <type_traits>

using library_vector = simd::vec<float,3,SIMD_ARCH>;
using sdk_vector = simd::float3;
static_assert(sizeof(library_vector) == 16);
static_assert(sizeof(sdk_vector) == 16);
static_assert(!std::is_same_v<library_vector,sdk_vector>);

int main() {
  sdk_vector sdk{1.f,2.f,3.f};
  library_vector library{sdk.x,sdk.y,sdk.z};
  auto reverse = library.zyx;
  return reverse.x == sdk.z && reverse.y == sdk.y && reverse.z == sdk.x ? 0 : 1;
}
