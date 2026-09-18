# One hub, source-level target selection

`import simd;` exposes every implemented ISA family for the host architecture.
It compiles at the configured project minimum. AVX-512, FP16 and BF16 operations
carry Clang function target attributes inside that same module, so importing it
does not strengthen an unrelated caller. `static_string`, scalar numerics and
the other common modules each retain one provider.

```cmake
find_package(simd CONFIG REQUIRED COMPONENTS simd)
add_executable(example example.cc)
target_link_libraries(example PRIVATE simd::simd)
```

## Generate only the variants you need

This x86 example compiles two overloads in one translation unit. The reusable
body receives the function name and the exact architecture type. Each expansion
is inside a matching Clang target scope.

```cpp
#include <simd/targets.h>
import simd;

#define MY_TARGETS(X, ...) X(avx512, __VA_ARGS__) X(avx2, __VA_ARGS__)
#define DOUBLE_BODY(name, tag)                                           \
  void name(tag, float const * input, float * output) {                 \
    using V = simd::vec<float, 4, tag>;                                 \
    auto x = simd::load_simd<V>(input);                                 \
    simd::store_simd(output, x + x);                                    \
  }

SIMD_TARGET_VARIANTS(double_four, MY_TARGETS, DOUBLE_BODY)

bool run(float const * input, float * output) {
  auto cpu = simd::observe_x86_capabilities();
  return simd::with_isa(SIMD_TARGET_LIST(MY_TARGETS), cpu, [&](auto arch) {
    double_four(arch, input, output);
  });
}

#undef DOUBLE_BODY
#undef MY_TARGETS
```

List order is selection order. `with_isa` calls the callback once for the first
admitted entry, or returns `false` without calling it if none qualifies. On
AArch64 use the NEON presets and `observe_arm_capabilities()`.

For an existing architecture type, [compile-time policy lookup](abi-lookup.md)
selects an implementation entry and its ordinal without querying the CPU.
`requires_abi` uses that ordinal to constrain disjoint operation overloads.

The callback is ordinary code compiled where it was defined. Passing a tag
does not change its compiler target. Keep the native body in the generated
overload, or use `SIMD_TARGET_PUSH(name)` / `SIMD_TARGET_POP()` around functions
you define yourself. Pointer/scalar entry parameters avoid transferring native
register values across different calling conventions.

Emit these variants outside other Clang ISA target-attribute scopes. Clang
combines nested target requirements: an AVX2 body inside an outer AVX-512 scope
can still use AVX-512, even though predefined feature macros do not reveal it.
The named pragma stack preserves the surrounding scope; it does not remove its
requirements. If nesting is intentional, include the outer scope's features in
`SIMD_TARGET_EXTRA_MINIMUM` so the generated admission list checks them too.

## Choose feature sets

Presets are names for canonical `isa<feature bits>` types. To register another
source name, give it one target feature literal:

```cpp
#define SIMD_TARGET_avx2_half "avx2,fma,bmi2,f16c"
#define MY_TARGETS(X, ...) X(avx2_half, __VA_ARGS__) X(avx2, __VA_ARGS__)
```

That same literal supplies the Clang attribute and the type used for admission.
The registry accounts for compiler-implied prerequisites, and the generated
list includes inherited translation-unit requirements. Reordering feature
names or repeating an implied feature does not make another type. Do not put
two spellings of the same canonical feature set in one list.

Supported positive feature names may be combined freely within one host
architecture. Unknown features, CPU-name shortcuts and negative feature strings
are rejected: silently guessing their admission requirements would make the
dispatch unsafe. The registry in `simd/isa.h` defines the supported vocabulary.
Clang target pragmas do not change predefined macros such as `__AVX512F__`;
write variant choices using the tag and `has_feature`.

## Native intrinsics and packages

Vector types retain implicit conversion to and from their native register
representation. An attributed body can mix standard intrinsics with SIMD
operations without explicit bridge calls. Those intrinsics still require the
same target support as they would in ordinary Clang code.

Installed packages distribute module sources. CMake builds one compatible hub
BMI and one provider for each common module; target variants do not multiply
them. Compiler, C++ dialect, exception mode and standard-library configuration
must still agree. Consumer PCHs remain optional and belong to the consumer.

Canonical feature tags replace the former empty tag structs. This changes the
names of vector template instantiations in compiled interfaces. Rebuild code
that exchanges these vector types across a library boundary when updating;
ordinary pointer/scalar entry interfaces keep their declared ABI.

`simd::avx2`, `simd::avx512` and the native-half CMake targets are compatibility
aliases for `simd::simd`. The old ISA-specific module names are replaced by the
hub import. `simd_target_omnibus` is retained as a compatibility no-op.
`simd_target_profile` remains available for applications that explicitly want
whole-translation-unit targeting; it is not needed for source target lists.

Project setup chooses `SIMD_MINIMAL_COMPILE_OPTIONS`. Its default is AVX2/FMA/BMI2
on x86 and NEON on ARM. The process must satisfy that minimum before executing
any code, including the dispatcher.

The source helper records registered features advertised by Clang's predefined
macros. CPU-model options can enable additional instructions without a matching
macro, so it cannot infer every requirement of an arbitrary `-mcpu` or `-march`
name. When needed, define `SIMD_TARGET_EXTRA_MINIMUM` before including
`<simd/targets.h>` as an additional registered feature mask, for example
`simd::target_features("avx2,f16c")`. This adds to admission requirements;
it does not change compiler flags or make startup safe below the project minimum.
