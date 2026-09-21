# Integer fused multiply-add of 52-bit values

`import native.x86.ifma;` provides `madd52lo` and `madd52hi` on
`simd<std::uint64_t,N,Arch>`, for `N` equal to 2, 4 or 8. Link `native::native`;
the `native.x86` and `native` hubs also export these operations.

Each lane multiplies the low 52 bits of `a` and `b`, producing an unsigned
104-bit product. `madd52lo<Arch>(accumulator,a,b)` adds product bits 0–51 to the
full 64-bit accumulator. `madd52hi` adds product bits 52–103 instead. Both
additions wrap modulo 2⁶⁴; neither saturates or propagates carry to another
lane. The upper 12 bits of each multiplicand are ignored, while every
accumulator bit participates.

EVEX forms provide hardware masks:

- `mask_madd52lo<Arch>(accumulator,mask,a,b)` retains the accumulator in
  inactive lanes; `mask_madd52hi` does the same for high products.
- `maskz_madd52lo<Arch>(mask,accumulator,a,b)` clears inactive lanes;
  `maskz_madd52hi` does the same for high products.

The mask is `predicate<N,Arch>`. Bit `i` controls result lane `i`; bits above
the lane count do not affect the result.

| Forms | Required architecture features | Caller target |
| --- | --- | --- |
| Unmasked 128/256-bit VEX | AVX-IFMA and AVX | `"avxifma"` |
| 128/256-bit EVEX, all mask modes | AVX512IFMA, AVX512F and AVX512VL | `"avx512ifma,avx512vl"` |
| 512-bit EVEX, all mask modes | AVX512IFMA and AVX512F | `"avx512ifma"` |

Unmasked short forms prefer the AVX-IFMA overload when the architecture tag
contains its features, following the VNNI family convention. Masked runtime
forms always require the EVEX features. AVX512BW and AVX512DQ are independent
and unnecessary. Use `target_features<native::x86>(target)` and admit the
complete target, including OS vector state, before entering the caller.

`x86_feature::avx512ifma` uses CPUID leaf 7, subleaf 0, EBX bit 21. The separate
`avxifma` feature uses leaf 7, subleaf 1, EAX bit 23; an unobserved subleaf does
not establish support. Hardware AVX-IFMA requires AVX, while Clang 23's target
also enables AVX2. The library records that distinction between
`feature_closure` and `target_features`. Neither extension is added to the
general `avx2` or `avx512` profiles.

All forms support exact constant evaluation. Strong tags use `constexpr`
overloads with native runtime paths; tags missing instruction features use
`consteval` overloads. Weak forms still require complete register storage:
SSE2 for 128 bits, AVX for 256 bits or AVX512F for 512 bits, with prerequisites.
They preserve the tag and provide no runtime software fallback. The constant
implementation uses 26-bit limbs and does not require a 128-bit integer type.

The [fixture](../tests/x86_ifma/README.md) describes independent product
references, masks, feature/target boundaries and raw/public assembly pairs.
Instruction semantics follow Intel's
[Software Developer's Manual](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf);
intrinsic forms follow Clang's
[AVX-IFMA](https://clang.llvm.org/doxygen/avxifmaintrin_8h_source.html),
[AVX512IFMA](https://clang.llvm.org/doxygen/avx512ifmaintrin_8h_source.html) and
[VL IFMA](https://clang.llvm.org/doxygen/avx512ifmavlintrin_8h_source.html) headers.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
