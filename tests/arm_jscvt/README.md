# JSCVT qualification

The fixture imports `native.arm.jscvt`, `native.arm`, and `native` separately.
The direct scalar-module consumer links only `native::minimal`; the hubs link
`native::native`. It can also be configured against an installed package.

Constant assertions cover signed zero, fractions, signed and unsigned word
boundaries, large exponents, infinities, NaNs and subnormals. A seeded bank of
2,048 arbitrary binary64 encodings compares the strong and weak-tag constexpr
results, then checks both against hardware and an independent truncation/modulo
reference at runtime. Another 100,000 encodings exercise runtime conversion.
Hardware execution requires observed JSCVT support; unsupported hosts skip it.

Paired codegen leaves require an identical instruction sequence to the compiler
builtin with the same scalar signature and target. Two actual failed compiles
check runtime use with a weak ISA and a caller lacking the instruction target.
The latter uses a generic ARMv8 baseline, so a stronger build host cannot satisfy
its missing target accidentally.
