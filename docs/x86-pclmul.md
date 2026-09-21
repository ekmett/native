<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
# Carry-less polynomial multiplication

`native.x86.pclmul` exports `pclmulqdq<Arch, Imm8>` for `__m128i`.
`native.x86.vpclmul` exports `vpclmulqdq<Arch, Imm8>` for `__m128i`,
`__m256i` and `__m512i`. Both belong to `native::minimal` and are reexported
by `native.x86` and `native`. Their implementation headers remain in the global
module fragment. Include `<immintrin.h>` before importing when naming raw
register types; use `native::simd::to_native()` and `from_native()` at the SIMD
boundary.

Each input bit is a coefficient of a polynomial over GF(2). The operation
multiplies two selected 64-bit polynomials and returns their exact 128-bit
product. Addition is XOR: there is no integer carry or polynomial reduction.
The coefficient of degree 127 is always zero. Wider registers contain two or
four independent products; there are no cross-128-bit-lane products.

`Imm8` is an unsigned template argument in 0–255. Bit 0 selects the half of
`a`, and bit 4 selects the half of `b`; 0 selects the low half and 1 the high
half. The four meaningful selectors are `0x00`, `0x01`, `0x10`, and `0x11`.
Other bits are ignored, so `0xee` means the same as `0x00`. Out-of-range,
overflowing and runtime immediates are rejected. Operands must have the exact
raw integer register type and matching widths; floating registers are not
implicitly reinterpreted.

| Operation | Register | Required `Arch` bits | Function target |
| --- | --- | --- | --- |
| `pclmulqdq` | `__m128i` | `pclmul` | `pclmul` |
| `vpclmulqdq` | `__m128i` | `pclmul`, `avx` | `avx,pclmul` |
| `vpclmulqdq` | `__m256i` | `vpclmulqdq`, `avx` | `avx,vpclmulqdq` |
| `vpclmulqdq` | `__m512i` | `vpclmulqdq`, `avx512f` | `avx512f,vpclmulqdq` |

The 256-bit form needs neither AVX2 nor AVX-512. The 512-bit form needs no
AVX512BW, AVX512DQ or AVX512VL. These instructions have no write-mask forms.
LLVM uses the same narrow intrinsics for VEX and EVEX encodings: an EVEX
128/256-bit encoding additionally requires VPCLMULQDQ and AVX512VL. Stronger
callers may select it; the API does not force an encoding. A baseline PCLMUL
caller uses legacy SSE, and an AVX caller may use VEX.

The capability `x86_feature::vpclmulqdq` records CPUID.7.0:ECX bit 10,
independently of `pclmul` at CPUID.1:ECX bit 1. Presence does not establish OS
support. Compiler closure for `vpclmulqdq` adds AVX and PCLMUL, matching Clang's
target dependencies; it does not add AVX2. `NATIVE_TARGET_MINIMUM` recognizes
`__VPCLMULQDQ__`. Unknown compiler features still fail closed.

Check `classify_isa` before calling a matching targeted function. AVX needs
observed XCR0 bits 1 and 2; AVX-512 also needs bits 5, 6 and 7. Imports enable
no instructions in the caller. All four overloads are pure integer operations
and leave floating-point status unchanged.

```cpp
#include <immintrin.h>
#include <native/attributes.h>
import native.x86.vpclmul;

constexpr auto requirements = native::x86_feature::avx &
                              native::x86_feature::vpclmulqdq;
native_target("avx,vpclmulqdq")
__m256i products(__m256i a, __m256i b) {
  return native::vpclmulqdq<requirements, 0x10>(a, b);
}
// Enter products only after classify_isa(observe_x86_capabilities(),
// requirements).admitted() succeeds.
```

The contracts follow Intel's [PCLMULQDQ instruction reference](https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2b-manual.pdf),
[VPCLMULQDQ extension reference, pages 2-25–2-27](https://kib.kiev.ua/x86docs/Intel/ISAFuture/319433-031.pdf),
and [CPUID feature table](https://cdrdv2-public.intel.com/868136/252046-081-sdm-change-document.pdf).
LLVM's [intrinsic declarations](https://clang.llvm.org/doxygen/vpclmulqdqintrin_8h_source.html)
and [target dependencies](https://github.com/llvm/llvm-project/blob/main/llvm/lib/TargetParser/X86TargetParser.cpp)
determine the compiler requirements.

The [fixture](../tests/x86_pclmul/README.md) records execution, compile-failure,
disassembly and installed-package coverage.
