<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

# Integer intermediate shapes

The same source runs from headers and `import simd`, with scalar, AVX2,
AVX-512 F/DQ, F/DQ/VL, F/DQ/BW and full F/DQ/BW/VL tags. Native calls are
admitted by CPU and OS state; their arguments cross only pointer/scalar
boundaries. ARM builds exercise scalar and NEON, without claiming x86 execution.

Each supported shape checks lane populations against `std::popcount`, signed
bit reinterpretation, exact adjacent widening and exact widened reductions.
The input bank includes zero, all ones, every single bit, and 193 or more mixed
patterns. Reduction covers every prefix, including the empty prefix. The
two- and three-lane uint32 shapes poison physical padding, check that reductions
ignore it, and check that returned populations clear it. There is no zero-lane
integer vector; its absence is checked at compile time.

Packing checks each available unsigned 64→32, 32→16 and 16→8 shape against
scalar truncation, with lane-order and high-bit inputs. Deposit checks include
empty/full masks and a deterministic mixed bank. Result types retain the
original architecture value and public `noexcept` guarantees.

The shape audit is:

| Operation | Intermediate/output requirement |
|---|---|
| `reinterpret_bits` | Source and destination shapes exist and physical native sizes match. Logical short storage cannot be bit-cast to a scalar native object of a different size. |
| `pairwise_add_widened` | Full registers widen into an available shape. A short uint32 pair produces one uint64 lane from its two live inputs. |
| `popcount` | Short uint32 lanes use their full storage then restore logical shape. F/DQ 512-bit uint32/uint64 lanes use lane-local SWAR, avoiding unavailable BW-only byte/word intermediates. Existing byte-table/CNT paths remain for other shapes. |
| `reduce_add_widened` | Full registers widen before reducing. Short uint32 reductions sum only live lanes. |
| `narrow_concat` | Both input and output shapes are required. F/DQ supports 512-bit 64→32 packing; 32→16 and 16→8 require the existing BW output/input shapes. |
| `deposit_bits` | Scalar uint64 input/output; no vector intermediate. |

The ordinary x86 object checker asks LLVM to isolate each of twelve named
pointer boundaries. It requires the expected function marker, a complete body,
native 512-bit operations and no helper calls/tail transfers. The six F/DQ
boundaries admit only reviewed F/DQ EVEX operations; non-VL boundaries also
reject narrow EVEX operations. Clang may recognize 64-bit SWAR and lower it to
two VEX.256 byte-table/SAD reductions joined into the 512-bit result; that
reviewed AVX2 lowering needs no BW and the checker verifies both SAD widths.
Checker mutation controls reject BW instructions,
helper calls, tail jumps, wrong widths and unrelated function labels.
Instrumented builds run functional tests and checker controls; ordinary-object
codegen checks are not registered when AddressSanitizer is enabled.

For a standalone source build, configure this directory. To register the fixture
in the main test driver, add this inside its existing test-enabled block:

```cmake
add_subdirectory(tests/kernel_integer_refinement)
```
