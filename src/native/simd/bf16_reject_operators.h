// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included inside each BF16 storage class. Its arithmetic API is dot2 only.
// Explicit rejection prevents a native conversion from selecting vector builtins.
#define NATIVE_BF16_REJECT_BINARY(OP) \
    /** Reject elementwise BF16 operators; use dot2 for native arithmetic. */ \
    friend void operator OP(simd,simd) = delete; \
    /** Reject mixed BF16 operands instead of converting to a native builtin. */ \
    template<class U> requires (!std::same_as<std::remove_cvref_t<U>,simd>) \
    friend void operator OP(simd,U) = delete; \
    /** Reject mixed BF16 operands instead of converting to a native builtin. */ \
    template<class U> requires (!std::same_as<std::remove_cvref_t<U>,simd>) \
    friend void operator OP(U,simd) = delete;
    NATIVE_BF16_REJECT_BINARY(+)
    NATIVE_BF16_REJECT_BINARY(-)
    NATIVE_BF16_REJECT_BINARY(*)
    NATIVE_BF16_REJECT_BINARY(/)
    NATIVE_BF16_REJECT_BINARY(%)
    NATIVE_BF16_REJECT_BINARY(&)
    NATIVE_BF16_REJECT_BINARY(|)
    NATIVE_BF16_REJECT_BINARY(^)
    NATIVE_BF16_REJECT_BINARY(<<)
    NATIVE_BF16_REJECT_BINARY(>>)
    NATIVE_BF16_REJECT_BINARY(==)
    NATIVE_BF16_REJECT_BINARY(!=)
    NATIVE_BF16_REJECT_BINARY(<)
    NATIVE_BF16_REJECT_BINARY(<=)
    NATIVE_BF16_REJECT_BINARY(>)
    NATIVE_BF16_REJECT_BINARY(>=)
#undef NATIVE_BF16_REJECT_BINARY
    /// Reject unary BF16 arithmetic; storage does not define it.
    friend void operator+(simd) = delete;
    /// Reject unary BF16 arithmetic; storage does not define it.
    friend void operator-(simd) = delete;
    /// Reject native fallback bitwise operations; use bits() explicitly.
    friend void operator~(simd) = delete;
    /// Reject native fallback logical operations; storage is not a predicate.
    friend void operator!(simd) = delete;
