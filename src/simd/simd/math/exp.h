#pragma once
#include "simd/vec.h"
#include "simd/wide_math.h"
#define SIMD_BACKEND_BODY "simd/simd/math/exp_body.h"
#include "simd/simd/for_each_backend.h"
#undef SIMD_BACKEND_BODY
