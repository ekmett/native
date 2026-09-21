#pragma once
#include "native/vec.h"
#include "native/x86/bmi2.h"


#define NATIVE_BACKEND_BODY "native/packing_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY
