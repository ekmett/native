# WAITPKG

The fixture checks the complete 64-bit UMWAIT deadline signature, TPAUSE's
baseline-provider default, feature/target/control rejection, and identical
TPAUSE code generation against the compiler intrinsic. The runtime smoke uses
an expired deadline and skips when WAITPKG is not admitted.

Configure this directory or run the root `native.x86.wait` tests. On an Apple
host cross-compiling x86_64, Rosetta can execute the baseline observer and
negative/codegen controls but does not supply native WAITPKG qualification.

The LLVM 23.1.1 x86_64/macOS build passes the four compile/codegen checks. Its
WAITPKG runtime test skips. The separate `tests/cpuid_module` build passes all
three baseline tests under Rosetta. Disassembly retains the high 32 deadline
bits in EDX for both UMWAIT and TPAUSE; the TPAUSE leaf matches the intrinsic.
