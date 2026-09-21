# BMI2 integer operations

`import native`, `import native.x86`, or `import native.x86.bmi2` exposes the
BMI2 family on x86-64. Source-tree header consumers can include
`<native/x86/bmi2.h>`; the installed public API uses the named modules.
Every operation takes an explicit `isa` template argument containing
`x86_feature::bmi2`; no AVX, AVX2, FMA, or BMI1 feature is required.

For `U = std::uint32_t` or `std::uint64_t`, the overloads are:

| Operation | Result |
| --- | --- |
| `bzhi<Arch>(U value, unsigned index)` | Clear bits at and above `index & 255`; retain the whole value when that index reaches the operand width. |
| `mulx<Arch>(U a, U b, U* high)` | Return the low half of the unsigned product and write the high half through a valid `high` pointer. |
| `pdep<Arch>(U value, U mask)` | Deposit consecutive low input bits into the positions selected by the mask. |
| `pext<Arch>(U value, U mask)` | Gather selected input bits into consecutive low result bits. |
| `shlx<Arch>(U value, unsigned count)` | Logical left shift with count reduced modulo 32 or 64. |
| `shrx<Arch>(U value, unsigned count)` | Logical right shift with count reduced modulo 32 or 64. |
| `rorx<Arch, Imm8>(U value)` | Right rotation by a compile-time byte, reduced modulo 32 or 64; `Imm8` must be at most 255. |

`sarx<Arch>(std::int32_t value, unsigned count)` and its `std::int64_t`
overload propagate the sign bit with the same masked shift counts. All wrappers
are `noexcept` and always inline into a compatible BMI2 target scope. Run
`classify_isa` with observed CPU capabilities before entering that scope.

The API specifies values and the `mulx` output write. C++ callers have no flag
preservation contract. Compilers may fold constants or use equivalent
instructions; for example, 32-bit multiplication can become a 64-bit `imul`.

The behavior follows the BZHI, MULX, PDEP, PEXT, RORX, and SARX/SHLX/SHRX entries
in Intel's [Software Developer's Manual, Volume 2](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).
The instruction entries distinguish the low byte used by BZHI from the low
five or six count bits used by shifts and rotations.
