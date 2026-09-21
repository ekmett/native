# GFNI byte operations

GFNI multiplies bytes in a finite field and applies affine transformations
to their bits. `import native.x86.gfni;` provides these operations on raw
registers; `native.x86` and `native` re-export them. Link `native::minimal`
for the granular module or `native::native` for the hub. The implementation
header is `native/x86/gfni.h`.

| Operation | Result for each input byte |
| --- | --- |
| `gf2p8mulb<Arch>(a,b)` | Field product of corresponding bytes |
| `gf2p8affineqb<Arch,Imm8>(a,matrix)` | Binary matrix product XOR `Imm8` |
| `gf2p8affineinvqb<Arch,Imm8>(a,matrix)` | Field inverse, then matrix product XOR `Imm8` |

The field polynomial is x⁸ + x⁴ + x³ + x + 1 (0x11b). The inverse-affine
operation first takes the field inverse of each input byte, treating zero as
zero, then applies the matrix. It does not invert the matrix itself.

Each 64-bit lane of `matrix` supplies an independent 8×8 binary matrix for
the eight input bytes in that lane. Numbering bytes from the least significant
end, result bit `i` is the parity of `a_byte & matrix_byte[7-i]`, XOR bit `i`
of `Imm8`. Thus `0x0102040810204080` is the identity matrix and
`0x8040201008040201` reverses the bits in each byte. The immediate is an unsigned
template argument in [0,255], shared by all lanes.
These semantics follow Intel's
[instruction reference](https://cdrdv2-public.intel.com/868140/253666-089-sdm-vol-2a.pdf).

All operations overload `__m128i`, `__m256i` and `__m512i`.
Each also has `_mask(src,k,a,b)` and `_maskz(k,a,b)` variants;
affine variants use `matrix` in place of `b` and retain the same template
immediate. Mask types are `__mmask16`, `__mmask32` and `__mmask64`.
Mask bit `i` selects byte `i`; an inactive byte is copied from `src` or
cleared, respectively. Register-only functions have no memory side effects.

## Feature and target requirements

`Arch` must explicitly contain every feature listed below. Function target
attributes establish the corresponding compiler requirements.

| Form | Required features in `Arch` | Function target |
| --- | --- | --- |
| Unmasked 128-bit | GFNI | `gfni` |
| Unmasked 256-bit | GFNI, AVX | `avx,gfni` |
| Unmasked 512-bit | GFNI, AVX512F | `avx512f,gfni` |
| Masked 128/256-bit | GFNI, AVX512F, AVX512BW, AVX512VL | `avx512bw,avx512vl,gfni` |
| Masked 512-bit | GFNI, AVX512F, AVX512BW | `avx512bw,gfni` |

The unmasked 256-bit form needs AVX, with no AVX2 requirement.
AVX512BW is required by LLVM's byte-mask intrinsic implementation, including
its mask selection operations. The architectural GFNI EVEX encodings require
GFNI with AVX512F for 512 bits or AVX512VL for narrower widths.
The wrapper contracts follow the installed LLVM intrinsic requirements;
see the [LLVM GFNI header](https://clang.llvm.org/doxygen/gfniintrin_8h_source.html).

`x86_feature::gfni` is an independent feature bit, detected from
CPUID leaf 7, subleaf 0, ECX bit 8. Its compiler prerequisite closure adds SSE2.
It is not added to the existing AVX2 or AVX-512 presets.
Legacy 128-bit execution uses the platform's XMM state; it does not require
OSXSAVE or YMM state. VEX execution requires OS-enabled XMM/YMM state
(`XCR0 & 0x6 == 0x6`), and EVEX execution also requires opmask and ZMM state
(`XCR0 & 0xe6 == 0xe6`). Admission applies these checks when the
corresponding AVX or AVX-512 requirements are present.

Compile the caller for the required target and check CPU and OS support before
entering it. Include any additional features used to compile that function in
the runtime check. Importing the module does not enable instructions or
dispatch at runtime.

```cpp
#include <native/attributes.h>
#include <immintrin.h>
import native.x86.gfni;

constexpr native::isa requirements =
    native::x86_feature::gfni & native::x86_feature::avx;

native_noinline native_target("avx,gfni")
void multiply_bytes(void* out, void const* a, void const* b) {
  auto x = _mm256_loadu_si256(static_cast<__m256i const*>(a));
  auto y = _mm256_loadu_si256(static_cast<__m256i const*>(b));
  _mm256_storeu_si256(static_cast<__m256i*>(out),
                     native::gf2p8mulb<requirements>(x,y));
}

bool try_multiply(void* out, void const* a, void const* b) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu,requirements).admitted()) return false;
  multiply_bytes(out,a,b);
  return true;
}
```

## Validation

`tests/x86_gfni` uses the header, granular module and hub from baseline
translation units. It compares multiplication, affine and inverse-affine
results with independent scalar polynomial and binary-matrix calculations,
using selected byte and matrix cases plus deterministic random vectors.
Runtime tests check CPU and OS support before executing optional instructions.

Compiler fixtures check feature constraints, constant immediate bounds and
target mismatches. Assembly checks distinguish legacy XMM, VEX YMM and EVEX
forms with optional features disabled at the translation-unit baseline.
They establish instruction selection, not performance.
The installed consumer fixture rebuilds module interfaces from a relocated
package. A machine without AVX-512 validates those forms by compilation and
assembly inspection and reports their execution as skipped.
