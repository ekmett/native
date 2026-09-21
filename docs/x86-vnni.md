# VNNI integer dot products

`import native.x86.vnni;` exposes integer dot products on raw x86 registers.
The module belongs to `native::minimal` and is reexported by `native.x86` and
`native`. Importing it leaves the caller's instruction target unchanged.

Every operation takes an explicit `isa` template argument and three equal-width
integer registers: `(accumulator, a, b)`. Each 32-bit result lane accumulates
products from the corresponding group of four bytes or two 16-bit words.
Non-saturating operations return the low 32 bits of the complete sum.

| Operations | Input a | Input b | Group size | Saturating variant |
| --- | --- | --- | --- | --- |
| `dpbusd`, `dpbusds` | unsigned byte | signed byte | 4 | signed 32-bit |
| `dpwssd`, `dpwssds` | signed word | signed word | 2 | signed 32-bit |
| `dpbssd`, `dpbssds` | signed byte | signed byte | 4 | signed 32-bit |
| `dpbsud`, `dpbsuds` | signed byte | unsigned byte | 4 | signed 32-bit |
| `dpbuud`, `dpbuuds` | unsigned byte | unsigned byte | 4 | unsigned 32-bit |
| `dpwsud`, `dpwsuds` | signed word | unsigned word | 2 | signed 32-bit |
| `dpwusd`, `dpwusds` | unsigned word | signed word | 2 | signed 32-bit |
| `dpwuud`, `dpwuuds` | unsigned word | unsigned word | 2 | unsigned 32-bit |

The second name in each pair saturates. It clamps the complete mathematical sum,
including the accumulator. For example, two products of `-32768 * -32768` sum
to `2147483648`; an accumulator of `-1` brings that back to `INT32_MAX` before
saturation. Wrapping or saturating the product sum first gives the wrong result.
`dpbuuds` and `dpwuuds` interpret the accumulator as unsigned and clamp to
`UINT32_MAX`. The other saturating forms use a signed accumulator. None changes
integer flags or floating-point status. The instruction definitions and LLVM
intrinsics specify these arithmetic rules:
[Intel instruction reference](https://cdrdv2-public.intel.com/835757/325383-sdm-vol-2abcd.pdf),
[LLVM core](https://clang.llvm.org/doxygen/avxvnniintrin_8h_source.html),
[LLVM INT8](https://clang.llvm.org/doxygen/avxvnniint8intrin_8h_source.html),
[LLVM INT16](https://clang.llvm.org/doxygen/avxvnniint16intrin_8h_source.html).

| Forms | Register widths | Exact features in `Arch` |
| --- | --- | --- |
| Core, unmasked VEX | 128, 256 | `avxvnni` |
| Core, unmasked EVEX | 128, 256 | `avx512f`, `avx512vnni`, `avx512vl` |
| Core, unmasked EVEX | 512 | `avx512f`, `avx512vnni` |
| Core, masked EVEX | 128, 256 | `avx512f`, `avx512vnni`, `avx512vl` |
| Core, masked EVEX | 512 | `avx512f`, `avx512vnni` |
| INT8 signedness extensions | 128, 256 | `avxvnniint8` |
| INT16 signedness extensions | 128, 256 | `avxvnniint16` |

The core comprises `dpbusd`, `dpbusds`, `dpwssd` and `dpwssds`. Its narrow
unmasked overloads select AVX-VNNI when `Arch` contains `avxvnni`, including
when both VNNI features are present. Otherwise they select AVX512VNNI.
The signedness extensions have no masked or 512-bit forms in these AVX
extensions. They are independent features; enabling basic AVX-VNNI does not
enable either extension.

For each core name, `mask_NAME<Arch>(accumulator, mask, a, b)` retains the
accumulator in inactive lanes, and `maskz_NAME<Arch>(mask, accumulator, a, b)`
zeros them. Mask bit `i` selects 32-bit lane `i`. The mask type is `__mmask8`
for 128/256 bits and `__mmask16` for 512 bits; the high four mask bits are
ignored at 128 bits. Register operands must have the same exact integer
register type. Floating-point registers and mixed shapes are rejected.

```cpp
#include <immintrin.h>
#include <native/targets.h>
import native.x86.vnni;

#define NATIVE_TARGET_dot "avxvnni"
constexpr auto dot_isa = NATIVE_TARGET_ISA(dot);

NATIVE_TARGET_PUSH(dot)
void accumulate(int* out, int const* acc, void const* a, void const* b) {
  __m256i s, x, y;
  __builtin_memcpy(&s, acc, sizeof(s));
  __builtin_memcpy(&x, a, sizeof(x));
  __builtin_memcpy(&y, b, sizeof(y));
  auto result = native::dpbusd<dot_isa>(s, x, y);
  __builtin_memcpy(out, &result, sizeof(result));
}
NATIVE_TARGET_POP()
```

Call this function only after `classify_isa(observe_x86_capabilities(), dot_isa,
NATIVE_TARGET_MINIMUM).admitted()`. `simd::to_native()` and `simd::from_native()`
provide the boundary to `native::simd` when the selected profile supports it.
The raw instruction module itself does not depend on the SIMD provider.

Detection preserves CPUID presence separately from executable OS state.
`avxvnni` is leaf 7, subleaf 1, EAX bit 4; `avx512vnni` is leaf 7, subleaf 0,
ECX bit 11; `avxvnniint8` and `avxvnniint16` are leaf 7, subleaf 1, EDX bits
4 and 10. VEX admission needs XMM/YMM state; EVEX admission needs opmask and
ZMM state too. Compiler closure adds AVX2 to each VEX feature and AVX512F to
AVX512VNNI, without adding another VNNI family. See
[Intel CPUID definitions](https://cdrdv2-public.intel.com/874240/325462-090-sdm-vol-1-2abcd-3abcd-4.pdf)
and [LLVM feature prerequisites](https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/X86/X86.td).

The [VNNI fixture](../tests/x86_vnni/README.md) exercises the textual header, granular module,
x86 umbrella and main hub. It also runs as an installed module consumer.
Independent wide scalar arithmetic checks grouping, wrapping, saturation and
masks, including cancellation after an overflowing signed word product pair.
Separate object probes check instruction encodings and reject missing features,
caller targets and register shapes. Unsupported runtime families return a skip;
successful compilation does not establish native execution.
