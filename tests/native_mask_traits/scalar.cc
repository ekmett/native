// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdint>
#include <type_traits>
import native.scalar;
#include "scalar_checks.h"
#define NATIVE_MASK_TRAITS_SCALAR_ONLY
#include "vector_checks.h"

int main(int argc, char**) { return mask_fixture::check_scalar_masks(argc); }
