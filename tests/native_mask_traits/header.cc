// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/mask_traits.h>
#include "scalar_checks.h"
// Check the standalone trait header before bringing in SIMD declarations.
#include <native/simd.h>
#include "vector_checks.h"

int main(int argc, char**) { return mask_fixture::check_scalar_masks(argc); }
