# Native BF16 profile

The optional `AVX512_BF16` profile adds `simd::avx512_bf16`, a distinct
instruction-profile tag, and a separate `simd::avx512_bf16` archive/module
provider. Existing tags, default profiles, the configured minimum, and scalar
`fp16`/`bf16` storage and conversion semantics are unchanged.

```cpp
import simd.avx512_bf16;
using B = simd::vec<simd::bf16,32,simd::avx512_bf16>;
using F = simd::vec<float,16,simd::avx512_bf16>;
F result = simd::dot2(B::load(a), B::load(b), accumulator);
```

`B` occupies one 512-bit register. The same API also provides 8 and 16 BF16
lanes in one 128- or 256-bit register, with 4 or 8 FP32 accumulator/result lanes.
Its typed memory operations and uint16 bit bridges preserve every representation, including subnormals, signed zero and
signaling NaNs. Partial operations access exactly the requested prefix; zero
length permits null pointers. Only the 8-, 16-, and 32-lane BF16 shapes are
supplied. The profile retains ordinary AVX-512 raw float, integer and mask shapes.

`dot2` uses native `VDPBF16PS`: output lane `i` first accumulates
`a[2*i+1]*b[2*i+1]`, then `a[2*i]*b[2*i]`. Each FP32 FMA rounds to nearest,
ties to even. Denormal inputs become zero and denormal outputs are flushed.
MXCSR controls and exception flags are neither read nor changed by the
instruction. NaNs follow instruction propagation priority, with low source
lanes ahead of high lanes and the accumulator. This is not a single-rounding
sum of three terms. See the [Intel instruction reference, pages 76-77](https://www.intel.com/content/dam/develop/external/us/en/documents/architecture-instruction-set-extensions-programming-reference-737410.pdf).

No BF16 elementwise arithmetic or native narrowing operation is introduced.
Other half profiles and any FTZ numerical policy remain separate work. The
smaller widths use AVX512VL, which the existing profile already requires;
profile admission and scalar conversions are unchanged.

## Build and admission

Configure the producer with `-DSIMD_PROFILES="AVX2;AVX512;AVX512_BF16"`.
It probes actual BF16 intrinsic compilation. Link a kernel against
`simd::minimal` and `simd::avx512_bf16`, then apply
`simd_target_profile(kernel AVX512_BF16)`. Keep the caller at the configured
minimum and pass pointers/scalars across the call boundary. The caller must
admit `classify_x86_profile(observe_x86_capabilities(),
x86_profile::avx512_bf16)` before calling the kernel. This requires the entire
existing AVX512 profile, CPUID.7.1 EAX bit 5, and OS-enabled XCR0 state `0xe6`.
Vendor names do not grant admission.

`import simd;` re-exports this profile when configured and requires BF16
compilation too. Common modules still have one BMI at the chosen minimum.
ISA flags belong to the profile producer/consumer, not the baseline caller.

## Verification

The root CTest suite includes the focused tests only when the profile is
configured. They check all 65,536 BF16 storage encodings at each width,
protected-page partial loads/stores of every prefix through 8, 16, and 32
lanes, zero-length null accesses, and representation-preserving native/unsigned
bridges. Instruction fixtures cover
accumulation order, nearest-even ties, underflow/overflow, signed zeros, NaN
priority and signaling-NaN quieting, plus 4,096 bounded finite reference lanes.
All cases run at all three widths across 32 MXCSR control/status combinations
and require exact state preservation. Partitioning each 32-lane input into
four 8-lane or two 16-lane operations must agree bit-for-bit with the 32-lane
result as well as the architectural/finite reference cases. The native test
skips with status 77 when admission fails; the `none` control enters no BF16 code. A disassembly test, when llvm-objdump is
available, requires one XMM, YMM, or ZMM `vdpbf16ps` in the respective ordinary
probe, with no wider register, conversion, FP32 emulation, or call.

The same sources are an installed-package fixture:

```sh
cmake --install build --prefix "build/relocated package"
cmake -S tests/bf16_profile -B build/bf16-package -G Ninja \
  -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release \
  -Dsimd_DIR="/absolute/path/to/relocated package/lib/cmake/simd"
cmake --build build/bf16-package --parallel 2
ctest --test-dir build/bf16-package --output-on-failure
```

It compiles granular and omnibus import consumers (the latter with a PCH), checks old/new type and
mask identities, and verifies one BMI for each common dependency and the new
profile. It uses only installed production sources; guarded pages come from
the adjacent test-only `../core_regression/support` directory.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
