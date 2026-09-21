# One hub, source-level target selection

`import native;` exposes every implemented ISA family for the host architecture.
It compiles at the configured project minimum. AVX-512, FP16 and BF16 operations
carry Clang function target attributes inside that same module, so importing it
does not strengthen an unrelated caller. `static_string`, scalar numerics and
the other common modules each retain one provider.

```cmake
find_package(native CONFIG REQUIRED COMPONENTS native)
add_executable(example example.cc)
target_link_libraries(example PRIVATE native::native)
```

## Generate only the variants you need

This x86 example compiles two constrained function templates in one translation
unit. The reusable body receives the function name and the exact ISA value.
Each expansion is inside a matching Clang target scope.

```cpp
#include <native/targets.h>
import native;

#define MY_TARGETS(X, ...) X(avx512, __VA_ARGS__) X(avx2, __VA_ARGS__)
#define DOUBLE_BODY(name, ISA)                                         \
  template<native::isa A> requires(A == ISA)                              \
  void name(float const * input, float * output) {                     \
    using V = native::simd<float, 4, A>;                                   \
    auto x = native::load_simd<V>(input);                                 \
    native::store_simd(output, x + x);                                    \
  }

NATIVE_TARGET_VARIANTS(double_four, MY_TARGETS, DOUBLE_BODY)

bool run(float const * input, float * output) {
  auto cpu = native::observe_cpu();
  return native::with_isa(NATIVE_TARGET_LIST(MY_TARGETS), cpu, [&]<native::isa A> {
    double_four<A>(input, output);
  });
}

#undef DOUBLE_BODY
#undef MY_TARGETS
```

List order is selection order. `with_isa` invokes `callback.operator()<A>()`
once for the first admitted entry, or returns `false` without calling it if none
qualifies. On AArch64 use the NEON presets; `observe_cpu()` uses the platform's
capability observer.

For an existing ISA value, [compile-time target selection](abi-lookup.md)
selects an implementation entry and its ordinal without querying the CPU.
Use `requires (target<A, Choices...> == I)` for disjoint operation overloads.
The result is an `int`, with `-1` for no match. A weaker choice before a stronger
one, or a duplicate choice, makes that direct target pack ill-formed.

The callback is ordinary code compiled where it was defined. Selecting its ISA
template argument does not change its compiler target. Keep the native body in
the generated overload, or use `NATIVE_TARGET_PUSH(name)` / `NATIVE_TARGET_POP()` around functions
you define yourself. Pointer/scalar entry parameters avoid transferring native
register values across different calling conventions.

Emit these variants outside other Clang ISA target-attribute scopes. Clang
combines nested target requirements: an AVX2 body inside an outer AVX-512 scope
can still use AVX-512, even though predefined feature macros do not reveal it.
The named pragma stack preserves the surrounding scope; it does not remove its
requirements. If nesting is intentional, include the outer scope's features in
`NATIVE_TARGET_EXTRA_MINIMUM` so the generated admission list checks them too.

## Write variants directly

You can write the overloads yourself. This kernel doubles sixteen floats,
compiling the native bodies for the current host:

```cpp
#include <native/config.h>
#include <native/targets.h>
import native;

using namespace native;

template<isa A>
void double16(float * out, float const * in) = delete;

#if NATIVE_HOST_X86
NATIVE_TARGET_PUSH(avx512)
template<isa A> requires(target<A, avx512, avx2> == 0)
void double16(float * out, float const * in) {
  using V = simd<float, 16, avx512>;
  auto x = V::load(in);
  (x + x).store(out);
}
NATIVE_TARGET_POP()

NATIVE_TARGET_PUSH(avx2)
template<isa A> requires(target<A, avx512, avx2> == 1)
void double16(float * out, float const * in) {
  using V = simd<float, 8, avx2>;
  for (unsigned i = 0; i < 16; i += 8) {
    auto x = V::load(in + i);
    (x + x).store(out + i);
  }
}
NATIVE_TARGET_POP()

#elif NATIVE_HOST_NEON
NATIVE_TARGET_PUSH(neon)
template<isa A> requires(A.has(arm_feature::neon))
void double16(float * out, float const * in) {
  using V = simd<float, 4, neon>;
  for (unsigned i = 0; i < 16; i += 4) {
    auto x = V::load(in + i);
    (x + x).store(out + i);
  }
}
NATIVE_TARGET_POP()
#endif
```

Call `double16<A>(out, in)` with a CPU/OS-admitted `A`. An `avx512_bf16`
bundle selects case zero; `avx2` selects case one. Each body uses its selected
ISA for local vectors, so additional caller features cannot strengthen the
body's compiler requirements. `target` selects the overload; push/pop supplies
its compiler flags.

`<native/config.h>` supplies the host macros; module imports do not export
preprocessor macros. These guards cover the native target scopes and concrete
vector types used above. Dependent bodies can remain shared; see the
[dependent NEON example](abi-lookup.md). Constraints do not make foreign
instruction families available.

The overload set remains open within each build. See [overload extension and
declaration order](abi-lookup.md).

## Choose feature sets

Presets are `constexpr isa` values with compiler prerequisites included. To
register another source name, give it one target feature literal:

```cpp
#define NATIVE_TARGET_avx2_half "avx2,fma,f16c"
#define MY_TARGETS(X, ...) X(avx2_half, __VA_ARGS__) X(avx2, __VA_ARGS__)
static_assert(NATIVE_TARGET_ISA(avx2_half).f16c);
```

That same literal supplies the Clang attribute and `NATIVE_TARGET_ISA(name)` value
used for admission. The registry accounts for compiler-implied prerequisites, and the generated
list includes inherited translation-unit requirements. Reordering feature
names or repeating an implied feature does not make another ISA value. Do not put
two spellings of the same canonical feature set in one list.

Supported positive feature names may be combined freely within one host
architecture. Unknown features, CPU-name shortcuts and negative feature strings
are rejected: silently guessing their admission requirements would make the
dispatch unsafe. The registry in `native/isa.h` defines the supported vocabulary.
Clang target pragmas do not change predefined macros such as `__AVX512F__`;
write variant choices using `A.has(native::x86_feature::avx512f)`, `A.avx512f`, or
subset comparisons such as `native::avx512 <= A`.

Ordinary feature construction and conjunction do not add prerequisites:
`native::isa(native::x86_feature::avx2)` has exactly the AVX2 bit. Use
`feature_closure` when constructing compiler requirements yourself. The named
presets are feature bundles; CPU-model bundles remain future work.

## Native intrinsics and packages

Instruction extensions take and return `simd` values directly. For an operation
that needs explicit intrinsic interoperation, supported shapes expose
`V::from_native(register_value)` and `value.to_native()`. These bridges copy
the register representation without a numerical conversion. Intrinsics still
require the same compiler target support as in ordinary Clang code.

The arithmetic profiles retain their existing implicit register conversions.
Instruction-only storage shapes use the explicit bridges; their existence
does not promise the arithmetic interface of a full profile.

The hub already guards its intrinsic headers by CPU family. Use the same
boundary when including them yourself:

```cpp
#include <native/config.h>
#if NATIVE_HOST_X86
#include <immintrin.h>
#elif NATIVE_HOST_NEON
#include <arm_neon.h>
#endif
import native;
```

These guards describe the compilation target, not a runtime CPU check. Keep
foreign Clang target attributes behind the same boundary: constraints defer
dependent C++ bodies, not preprocessing or attribute validation.

Installed packages distribute module sources. CMake builds one compatible hub
BMI and one provider for each common module; target variants do not multiply
them. Compiler, C++ dialect, exception mode and standard-library configuration
must still agree. Consumer PCHs remain optional and belong to the consumer.

Structural ISA value arguments replace the former architecture tag types. This
changes template identity and symbol names in compiled interfaces. Rebuild BMIs
and code that exchanges these vector types across a library boundary when updating;
ordinary pointer/scalar entry interfaces keep their declared ABI.

The CMake targets named `native::avx2`, `native::avx512` and the native-half profiles
are compatibility aliases for `native::native`. The old ISA-specific module names are replaced by the
hub import. `native_target_omnibus` is retained as a compatibility no-op.
`native_target_profile` remains available for applications that explicitly want
whole-translation-unit targeting; it is not needed for source target lists.

Project setup chooses `NATIVE_MINIMAL_COMPILE_OPTIONS`. Its empty default retains
the toolchain's baseline. The process must satisfy that minimum before executing
any code, including the dispatcher.

The source helper records registered features advertised by Clang's predefined
macros. CPU-model options can enable additional instructions without a matching
macro, so it cannot infer every requirement of an arbitrary `-mcpu` or `-march`
name. When needed, define `NATIVE_TARGET_EXTRA_MINIMUM` before including
`<native/targets.h>` as an additional ISA value, for example
`native::target_features("avx2,f16c")`. This adds to admission requirements;
it does not change compiler flags or make startup safe below the project minimum.

## Capability module migration

Import `native.isa` for the shared feature/ISA vocabulary and admission interfaces
alone. It is the sole module provider of those declarations. Import `native.features`
to add the native platform's CPU utilities without the vector hub. It re-exports `native.x86.features`
on x86, or `native.arm.features` on AArch64. Both architectures' feature names remain
available on either host.

The platform modules are `native.x86.features` and `native.arm.features`. Each re-exports
`native.isa` and adds its native capability snapshot and observer. Native snapshots expose architecture-typed `present`
and `observed` sets; both must contain a required feature. Their nested `raw`
query results are diagnostics, not a second admission source. Standalone capability consumers link `native::common`;
they do not need the vector hub. The raw `cpuid` function and vendor query remain
in `native.x86.features`, and waiting instructions remain in `native.x86.wait`.

Replace `simd.cpu.x86` (or the older `simd.cpuid`) with `native.x86.features`, and
`simd.cpu.arm` (or the older `simd.arm`) with `native.arm.features`.
Use `native.features` for portable imports. The fixed `x86_profile`
and `arm_profile` enums, their classifiers and per-platform admission records
have been removed. Pass the existing ISA values to `classify_isa(cpu, avx2)` or
`classify_isa(cpu, neon_fp16)`, or use a finite list with `with_isa` when selecting
an implementation. An additional ISA minimum is admitted together with the
requested features. Failed observations, stale bits and unknown requirements
cannot authorize optional instructions.

The shared result retains all `missing_features` and `missing_xcr0` bits.
`reason()` returns the target spelling of the first unavailable feature, an
OS-state description, or `"admitted"`. Unavailable features include both failed
queries and observed absence; inspect the native snapshot when that distinction
matters. Rebuild module producers and consumers together after this API change.
