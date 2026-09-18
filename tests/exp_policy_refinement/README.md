# Wide binary32 exp target selection

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->

`exp_target<A>` selects one of five raw x86 scopes. Each overload compares that
index, calls the existing array kernel, and preserves the complete caller tag,
lane count, pack extent, Flush parameter and conditional noexcept. Every callee
uses the same split, so exp needs no composition of identical policies.

FP32 ignores extra FP16/BF16 features in its caller tag. VL changes narrow mask
and scaling paths; BW remains a raw callee's target-attribute requirement.
Native half values use their own requirements elsewhere.

The tests cover eleven caller tags, including each half-feature combination,
under both their full caller scopes and the five scopes without half attributes.
They compare exact outputs against the generic wide implementation for one,
short and full-width vectors; zero, one and three registers; both Flush values;
and boundary/special inputs. CPU/OS admission gates actual execution.

Function-pointer checks prove ordinary exp selects the specialized overload,
while the reference selects the generic implementation. Custom array hooks,
element fallback, scalar values, exception behavior and the math namespace
export remain covered. The import-only test needs no private headers.

All 22 code-generation entries remain. The checker verifies packed FMA, SCALEF
at the existing F/VL boundaries, register width, and no outlined helper calls
in uninstrumented builds. Metadata checks cover 128 feature combinations and
exercise the shared refinement utility's priority, minima and empty-list cases.
