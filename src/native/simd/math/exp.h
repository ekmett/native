#pragma once
#include "native/vec.h"
#include "native/wide_math.h"
#define NATIVE_BACKEND_BODY "native/simd/math/exp_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY
