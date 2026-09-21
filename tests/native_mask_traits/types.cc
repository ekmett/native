// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <type_traits>
import native.types;
#include "scalar_checks.h"

int main(int argc, char**) { return mask_fixture::check_scalar_masks(argc); }
