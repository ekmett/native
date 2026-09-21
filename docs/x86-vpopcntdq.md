# Packed population counts

VPOPCNTDQ counts the set bits in each 32-bit or 64-bit integer lane.
`import native.x86.vpopcntdq;` provides these operations through
`native::native`; `native.x86` and `native` re-export them. The implementation
header is `native/x86/vpopcntdq.h`. Installed consumers import the modules;
the package keeps implementation headers private for BMI regeneration.

Every operation takes a `template<native::isa Arch>` argument and typed
`native::simd` vectors. Use `target_features(...)` or `feature_closure(...)`
to include register prerequisites in the tag. `Arch` must contain `avx512f` and `avx512vpopcntdq`;
128-bit and 256-bit overloads also require `avx512vl`.

| Operation | Result in each lane |
| --- | --- |
| `vpopcntd<Arch>(value)` | Number of set bits in a 32-bit lane, from 0 to 32 |
| `vpopcntq<Arch>(value)` | Number of set bits in a 64-bit lane, from 0 to 64 |
| `mask_vpopcntd<Arch>(source, mask, value)` | Count where the mask bit is set; otherwise retain `source` |
| `mask_vpopcntq<Arch>(source, mask, value)` | Same merge rule for 64-bit lanes |
| `maskz_vpopcntd<Arch>(mask, value)` | Count where the mask bit is set; otherwise zero |
| `maskz_vpopcntq<Arch>(mask, value)` | Same zero rule for 64-bit lanes |

The dword operations accept `simd<std::uint32_t,N,Arch>` with N = 4, 8 or 16;
the qword operations accept `simd<std::uint64_t,N,Arch>` with N = 2, 4 or 8.
Masks use `native::predicate<N,Arch>` for the corresponding lane count. Bit zero controls
the lowest lane; constructing a mask clears bits above its logical lane count. Counts retain
their lane width. Operations are `noexcept`, have no memory or flag effects,
and carry constant-function and target attributes. The intrinsic definitions
follow LLVM's [512-bit](https://clang.llvm.org/doxygen/avx512vpopcntdqintrin_8h_source.html)
and [VL](https://clang.llvm.org/doxygen/avx512vpopcntdqvlintrin_8h_source.html)
interfaces.

```cpp
#include <cstdint>
#include <native/targets.h>
import native.x86.vpopcntdq;

#define NATIVE_TARGET_count_lanes "avx512f,avx512vpopcntdq"
constexpr auto count_requirements = NATIVE_TARGET_ISA(count_lanes);

NATIVE_TARGET_PUSH(count_lanes)
void count_lanes(std::uint32_t const* input, std::uint32_t* output) {
  auto bits = native::simd<std::uint32_t,16,count_requirements>::load(input);
  auto counts = native::vpopcntd<count_requirements>(bits);
  counts.store(output);
}
NATIVE_TARGET_POP()

bool try_count_lanes(std::uint32_t const* input, std::uint32_t* output) {
  auto cpu = native::observe_x86_capabilities();
  if (!native::classify_isa(cpu, count_requirements,
                          NATIVE_TARGET_MINIMUM).admitted()) return false;
  count_lanes(input, output);
  return true;
}
```

The buffers in this example each provide 64 accessible bytes. Pointer arguments
keep the baseline call boundary independent of vector register calling conventions.
Importing a module does not enable instructions in its caller.

`x86_feature::avx512vpopcntdq` represents CPUID.7.0 ECX bit 14. Scalar POPCNT
and BITALG do not establish this bit. Observation records the bit exactly;
the catalog adds AVX512F when computing compiler prerequisite closure. Short
forms add AVX512VL explicitly. The shared closure includes Clang's existing
AVX512F prerequisites, including scalar POPCNT; it does not substitute that
bit for VPOPCNTDQ. See LLVM's [CPUID definitions](https://clang.llvm.org/doxygen/cpuid_8h_source.html).

Admission requires observed and present CPU features plus readable XCR0 with
XMM, YMM, opmask, upper ZMM and high ZMM state enabled (`(XCR0 & 0xe6) == 0xe6`).
This also applies to VL forms. Missing CPU support or OS state prevents the call.

`tests/x86_vpopcntdq` uses the granular module and hub. On admitted
hardware, a baseline scalar bit loop checks selected inputs, every one-hot bit,
complements, lane order, masks and random vectors. Separate baseline tests cover
metadata and admission using synthetic capability records. Compilation must
fail for missing feature sets or target attributes; disassembly checks the
instruction, register width and masking for every form.

Runtime tests return CTest skip code 77 when AVX-512 cannot execute. The
compilation and disassembly checks establish instruction selection, with no
claim of native execution or performance. The same module tests support a
relocated installed package.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
