# AVX-NE-CONVERT qualification

The fixture imports `native.x86.avxneconvert`, `native.x86`, and `native`,
using public `native::simd` values throughout. Each form has a positive target
control and negative builds for missing ISA-tag support and missing caller
target. Each variant also rejects an absent AVX tag. Additional checks reject raw
intrinsic-vector narrowing inputs, an invalid lane count, and a seven-element input to the four-lane even conversion (which reads eight).

Constant evaluation compares all 14 variants under strong and weak tags against
an independent arithmetic half decoder and quotient/remainder BF16 rounding
reference. Tests include zeros, subnormals, infinities, quiet/signaling NaNs,
rounding ties, overflow and deterministic random bit patterns. They check the
four-lane result's zero upper register words and reject BF16 arithmetic in both
AVX-NE-CONVERT storage and existing AVX512_BF16 shapes. Generic FP16/BF16
storage also rejects mixed raw-vector operands in both orders, compound
assignment, and unary arithmetic.

The runtime test first admits the CPU/OS requirements. On supported hardware it
checks all 65,536 half encodings, random binary32 inputs, every rounding-mode /
DAZ / FTZ combination, unchanged MXCSR sticky flags, natural 2-byte input
alignment, output guards and zero upper words. Unsupported hardware returns 77;
a skipped test does not establish hardware conformance.

Fourteen raw/public object pairs per profile must have identical instruction
sequences, both for the minimum feature tag and when combined with AVX512_BF16.
Both use identically offset input pointers, with no register-alignment promise.
A separate inspection requires every instruction form and rejects helper calls.

Semantics follow [Intel SDM 089](https://cdrdv2-public.intel.com/868137/325462-089-sdm-vol-1-2abcd-3abcd-4.pdf),
Volume 2C pages 5-7, 5-8, 5-34 through 5-39, and `make_fp32` on page 5-160.
The module uses the current BF16 widening operation, which preserves subnormals
and signaling-NaN bits. Binary16 widening quiets NaNs and raises no exceptions;
BF16 narrowing uses RNE with DAZ/FTZ and quiets NaNs.

For a standalone source build, configure this directory with the supported
Clang/CMake toolchain. For an installed package consumer, add
`-DNATIVE_AVXNE_INSTALLED=ON -DCMAKE_PREFIX_PATH=<relocated-prefix>`.
Both modes build and run the same public API checks. No AVX-NE-CONVERT compiler
option is added to the project baseline; only targeted instruction functions
acquire it.

Local qualification on LLVM 23.1.1 and CMake 4.4.3, compiling macOS x86_64 on
Apple Silicon: the source fixture and the relocated installed-package fixture
each pass 53 checks and skip the one hardware test. Both profiles pass all
14 exact raw/public codegen pairs. LLVM IR inspection confirms that all twelve
memory-widening intrinsics receive pointers without stronger alignment
attributes or alignment assumptions; narrowing loads/stores use natural
4-byte/2-byte alignment. No AVX-NE-CONVERT hardware execution was available.
