#pragma once
#include "native/vec.h"

namespace native::detail {
  // A complete instruction register may provide only storage and transfers.
  // Integer algorithms need the arithmetic operations of a kernel shape.
  template<class T, std::size_t N, isa<> Arch>
  concept integer_arithmetic = simd_integer_element<T> && requires(simd<T,N,Arch> value) {
    typename simd<T,N,Arch>::native_type;
    { value + value } -> std::same_as<simd<T,N,Arch>>;
    { value - value } -> std::same_as<simd<T,N,Arch>>;
    { value & value } -> std::same_as<simd<T,N,Arch>>;
    { value.template right<1>() } -> std::same_as<simd<T,N,Arch>>;
  };
}

#define NATIVE_BACKEND_BODY "native/integer_body.h"
#include "native/simd/for_each_backend.h"
#undef NATIVE_BACKEND_BODY
