# Static strings as a named module

`import simd.static_string;` provides the existing `simd::basic_static_string`,
standard character-width aliases, `static_c_string`, `_ss` and `_scs` literals.
Definitions belong to this module; there is no textual implementation facade.
The module is registered on `simd::common` and included in the `simd` archive.

The standard structural string-literal template maps every literal into the
same `reify<CharT, characters...>::value` storage used by the retained
`integer_sequence` constructor. Equal literals share pointers across importing
translation units. Default objects refer to canonical, null-terminated empty
storage. The old JSON helpers and their dependency are removed.

```cpp
import simd.static_string;
using namespace simd;
constexpr auto name = "interned"_ss;
static_assert(name == "interned"_ss);
static_assert(name.size() == 8);
```

Compatibility boundaries retained deliberately:

- The public `string_view` constructor is an unchecked escape hatch. Its view
  must refer to the complete, null-terminated, interned sequence. Arbitrary views
  can dangle and do not satisfy pointer equality or hashing requirements.
- `static_c_string` assumes no embedded NUL before the end. Existing conversion
  behavior is retained; converting an embedded-NUL value back recovers only the
  C-string prefix. `_ss` itself preserves embedded NULs and full lengths.
- Custom traits retain the old fallback equality and pointer hash. Traits that
  consider distinct interned sequences equal can violate the hash contract;
  this migration does not redesign that legacy customization behavior.
- `front`, `back`, and indexing retain their nonempty/in-range preconditions.
  Default data access, conversion and empty-range operations are safe.

The focused test uses three consumer TUs, two with no standard-header includes.
It checks pointer identity, integer-sequence/literal identity, hashing, ranges,
copy/move/swap, all five character widths, stream/string conversions and empty
objects. With exceptions enabled, invalid `at` access must reach the catch.

Standalone CMake target: `static_string_import`; test: `static_string.import`.
Use C++26-capable Clang and Ninja, with `SIMD_ENABLE_EXCEPTIONS=OFF/ON` and
`SIMD_ENABLE_ASAN=ON` for the sanitizer control. The normal project enables this
test through `SIMD_BUILD_TESTS`.

Windows validation: Clang 23.1.1, MSVC STL 14.44, CMake 4.4.3, Release,
exceptions OFF, ON, and ASan/OFF: one test passed in each configuration.
The initial test-scanning and deprecated-literal-spelling diagnostics were
retained and corrected. This is a Windows module/link/runtime qualification,
not an ARM, GPU, package relocation or arbitrary-traits guarantee.
