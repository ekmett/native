// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included inside each BF16 storage class. Its arithmetic API is dot2 only.
// Explicit rejection prevents a native conversion from selecting vector builtins.
#define SIMD_BF16_REJECT_BINARY(OP) \
    /** Reject elementwise BF16 operators; use dot2 for native arithmetic. */ \
    friend void operator OP(vec,vec) = delete; \
    /** Reject mixed BF16 operands instead of converting to a native builtin. */ \
    template<class U> requires (!std::same_as<std::remove_cvref_t<U>,vec>) \
    friend void operator OP(vec,U) = delete; \
    /** Reject mixed BF16 operands instead of converting to a native builtin. */ \
    template<class U> requires (!std::same_as<std::remove_cvref_t<U>,vec>) \
    friend void operator OP(U,vec) = delete;
    SIMD_BF16_REJECT_BINARY(+)
    SIMD_BF16_REJECT_BINARY(-)
    SIMD_BF16_REJECT_BINARY(*)
    SIMD_BF16_REJECT_BINARY(/)
    SIMD_BF16_REJECT_BINARY(%)
    SIMD_BF16_REJECT_BINARY(&)
    SIMD_BF16_REJECT_BINARY(|)
    SIMD_BF16_REJECT_BINARY(^)
    SIMD_BF16_REJECT_BINARY(<<)
    SIMD_BF16_REJECT_BINARY(>>)
    SIMD_BF16_REJECT_BINARY(==)
    SIMD_BF16_REJECT_BINARY(!=)
    SIMD_BF16_REJECT_BINARY(<)
    SIMD_BF16_REJECT_BINARY(<=)
    SIMD_BF16_REJECT_BINARY(>)
    SIMD_BF16_REJECT_BINARY(>=)
#undef SIMD_BF16_REJECT_BINARY
    /// Reject unary BF16 arithmetic; storage does not define it.
    friend void operator+(vec) = delete;
    /// Reject unary BF16 arithmetic; storage does not define it.
    friend void operator-(vec) = delete;
    /// Reject native fallback bitwise operations; use bits() explicitly.
    friend void operator~(vec) = delete;
    /// Reject native fallback logical operations; storage is not a predicate.
    friend void operator!(vec) = delete;
