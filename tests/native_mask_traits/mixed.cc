// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/simd.h>
import native;
import native.scalar;
import native.types;
#include "scalar_checks.h"
#include "vector_checks.h"

// Textual headers and all public module paths retain one customization identity.
int main(int argc, char**) { return mask_fixture::check_scalar_masks(argc); }
