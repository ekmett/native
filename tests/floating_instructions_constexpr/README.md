# Floating instruction constant evaluation

These module consumers compare public constant-evaluated operations with the
same public operations executing their native instructions. The generated
corpus contains arbitrary representations and adversarial zeros, subnormals,
normal boundaries, infinities, signaling NaNs and quiet NaNs. Each form also
checks that a complete vector type with a weaker architecture tag produces
identical constant results. Failures print the deterministic seed, case index,
inputs, expected bits and actual bits.

The ARM fixtures cover all 104 FHM lane/shape forms, all 102 FCMA rotation,
lane and element-type forms, and all 41 BF16 forms. Native execution uses
FPCR=0, preserving and restoring the original FPCR/FPSR. The F16C fixture
covers both vector widths, all immediate-control equivalence classes, every
scalar immediate byte, and every scalar half input encoding. Its independent
integer conversion oracle checks results without floating-point arithmetic.
Native F16C execution uses MXCSR=0x1f80 and restores the caller's value.

Runtime feature observation precedes each native test. Unsupported hardware
returns the CTest skip code; malformed capability sets fail. Actual runtime
inputs to missing-feature overloads must be rejected even inside functions
whose compiler targets enable those instructions. Existing family suites
continue to check arbitrary runtime FP control settings, status effects,
independent numerical oracles, and raw/public instruction identity.

The directory can be configured independently with an installed package using
`find_package(native COMPONENTS native)`. Consumer tests use only public module
APIs; private compile-failure fixtures run in the producer build.
