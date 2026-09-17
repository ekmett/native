/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_approx.hpp>

/// collect inline tests from all headers here
#define EIN_TESTING 1
// add headers below to include them in the test suite
#include "assert.hpp"
#include "cpuid.hpp"
#include "concepts.hpp"
#include "memory.hpp"
#include "numerics/bf16.hpp"
#include "numerics/fp16.hpp"
#include "numerics.hpp"
#include "profiling.hpp"
#include "static_string.hpp"
#include "types.hpp"
#include "wait.hpp"
#include "simd_data.hpp"
#include "simd.hpp"
