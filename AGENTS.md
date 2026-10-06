# native

Read [src/README.md](src/README.md) before changing source ownership.

## Global module fragment: a library invariant

**Every definition that calls a platform intrinsic belongs in the global module
fragment, between `module;` and `export module`.** This includes inline and
always-inline wrappers, template bodies, and raw instruction/assembly helpers.
This boundary is a reason the library exists: importing a module and inlining
its wrappers must not leave a consumer referring to an intrinsic declaration
that is unavailable across the module boundary. A passing build on one compiler
or one direct importer does not establish that this is safe.

The module interface may export those definitions or supply typed bindings that
call the global-module helpers. Preserve feature constraints, target attributes,
constexpr paths, SIMD traits, customization, and the zero-overhead contract.
Do not move intrinsic calls into named-module purview to remove a header.

Headers included in the global module fragment are ordinary C++ headers. They
may be shared by multiple modules and ordinary translation units; retain their
include guards, dependencies, and common declaration identity. Shared global
implementation and textual macro substitution are the reasons for implementation
headers here. Macro-selected bodies may intentionally be included repeatedly.

A single-use header may be folded into its caller only at the same semantic
location: global-fragment contents stay in the global fragment. Before deleting
one, trace its includes through other headers, macro-selected includes, modules,
and ordinary translation units. Do not infer use counts from `.ccm` files alone.

When changing this boundary, check an importing consumer, including a further
module boundary where relevant, and inspect generated code for affected runtime
wrappers. Preserve Doxygen briefs and applicable source/license notices.
