// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdint>
#include <type_traits>
import native;
#include "scalar_checks.h"
#include "vector_checks.h"

static_assert(std::same_as<native::mask<native::fp16>, bool>);
static_assert(std::same_as<native::mask<native::bf16 const&>, bool>);
static_assert(std::same_as<native::mask<std::array<native::fp16, 3>>, std::array<bool, 3>>);

int main(int argc, char**) { return mask_fixture::check_scalar_masks(argc); }
