# SPDX-FileCopyrightText: 2024, 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

find_package(Doxygen 1.18 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_RECURSIVE YES)
# The README is rendered from the staged Markdown copy below.
set(DOXYGEN_EXCLUDE "${PROJECT_SOURCE_DIR}/src/README.md")
set(DOXYGEN_FILE_PATTERNS *.h *.ccm *.md)
set(DOXYGEN_EXTENSION_MAPPING "ccm=C++")
set(DOXYGEN_EXTRACT_ALL NO)
set(DOXYGEN_EXTRACT_PRIVATE NO)
set(DOXYGEN_EXTRACT_STATIC NO)
set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
set(DOXYGEN_HIDE_UNDOC_CLASSES YES)
set(DOXYGEN_EXCLUDE_SYMBOLS "*::detail::*")
set(DOXYGEN_EXAMPLE_PATH "${PROJECT_SOURCE_DIR}/tests/api")
set(DOXYGEN_EXAMPLE_RECURSIVE YES)
set(DOXYGEN_ENABLE_PREPROCESSING YES)
set(DOXYGEN_MACRO_EXPANSION YES)
set(DOXYGEN_SEARCH_INCLUDES NO)
set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
# Doxygen misparses conditional explicit constructors; keep their actual
# contracts in the comments while rendering the ordinary constructor signature.
# Select a representative SIMD family and omit raw compiler attributes.
# NATIVE_DOXYGEN also exposes independently guarded ARM instruction declarations.
set(DOXYGEN_PREDEFINED
  "NATIVE_DOXYGEN:=1"
  "NATIVE_HOST_X86:=1" "NATIVE_HOST_NEON:=0"
  "NATIVE_HAS_NATIVE_FP16=1" "NATIVE_HAS_NATIVE_BF16=1"
  "explicit(...):=explicit" "__declspec(x):=" "__attribute__(x):=" "__cplusplus=202600L" "NATIVE_PROFILE=512" "NATIVE_BACKEND:=avx512_backend" "NATIVE_ARCH:=::native::avx512"
  "NATIVE_BACKEND_NAMESPACE:=native::detail::avx512_backend"
  "NATIVE_ARCH_REQUIRES(Arch):=(::native::avx512 <= Arch)"
  "NATIVE_WIDE_FAMILY:=0" "NATIVE_WIDE_DETAIL:=detail::wide_generic_detail"
  "NATIVE_WIDE_TARGET:="
  "NATIVE_HAS_AVX2:=1" "NATIVE_HAS_AVX512F:=1" "NATIVE_HAS_AVX512DQ:=1"
  "NATIVE_HAS_AVX512BW:=1" "NATIVE_HAS_AVX512VL:=1" "NATIVE_HAS_ARM_NEON:=0"
  "native_inline:=inline" "native_flatten:=" "native_noinline:="
  "native_const:=" "native_pure:=" "native_nodiscard:=" "native_lifetimebound:="
  "native_artificial:=" "native_noescape:=" "native_reinitializes:="
  "native_empty_bases:=" "native_restrict:=" "native_hidden:="
  "native_nonnull(x):=" "native_null_terminated_string_arg(x):=")
# The compiler includes wide's fragments once per target family. Doxygen scans
# input headers independently and cannot reconstruct their class context. Stage
# one expansion in the actual owning header, retaining signatures and comments;
# this is a representative overload, with internal dispatch constraints intact.
set(docs_source_root "${CMAKE_CURRENT_BINARY_DIR}/source")
# Doxygen's symbol exclusions leave class members in XML. Stage isa with its
# named property hooks removed, retaining generic get/set/has and the properties.
# Describe the generated property declarations here to keep the source compact.
set(docs_isa "${docs_source_root}/src/native/isa.h")
file(READ "${PROJECT_SOURCE_DIR}/src/native/isa.h" docs_isa_text)
string(REGEX REPLACE
  "    constexpr (bool get_[a-z0-9_]+\\(\\) const noexcept|void set_[a-z0-9_]+\\(bool value\\) noexcept) \\{[^\r\n]*\\}[\r\n]*"
  "" docs_isa_text "${docs_isa_text}")
string(REGEX REPLACE
  "(__declspec\\(property\\(get=get_([a-z0-9_]+),put=set_[a-z0-9_]+\\)\\) bool [a-z0-9_]+;)"
  "\\1 ///< Read or update the \\2 feature bit through flags."
  docs_isa_text "${docs_isa_text}")
file(MAKE_DIRECTORY "${docs_source_root}/src/native")
file(WRITE "${docs_isa}" "${docs_isa_text}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/native/isa.h")
list(APPEND DOXYGEN_EXCLUDE "${PROJECT_SOURCE_DIR}/src/native/isa.h")
set(docs_wide "${docs_source_root}/src/native/wide.h")
file(READ "${PROJECT_SOURCE_DIR}/src/native/wide.h" docs_wide_text)
foreach(fragment IN ITEMS wide_members wide_operations)
  set(fragment_path "${PROJECT_SOURCE_DIR}/src/native/${fragment}.h")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${fragment_path}")
  file(READ "${fragment_path}" fragment_text)
  set(include_line "#include \"native/${fragment}.h\"")
  string(FIND "${docs_wide_text}" "${include_line}" include_offset)
  if(include_offset EQUAL -1)
    message(FATAL_ERROR "Doxygen wide staging could not find ${include_line}")
  endif()
  string(LENGTH "${include_line}" include_length)
  math(EXPR suffix_offset "${include_offset} + ${include_length}")
  string(SUBSTRING "${docs_wide_text}" 0 ${include_offset} wide_prefix)
  string(SUBSTRING "${docs_wide_text}" ${suffix_offset} -1 wide_suffix)
  string(REPLACE "${include_line}" "" wide_suffix "${wide_suffix}")
  set(docs_wide_text "${wide_prefix}${fragment_text}${wide_suffix}")
  list(APPEND DOXYGEN_EXCLUDE "${fragment_path}")
endforeach()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/native/wide.h")
file(MAKE_DIRECTORY "${docs_source_root}/src/native")
# Parenthesize the compound assignment constraints for Doxygen's requires parser.
# This changes grouping only, not the represented participation condition.
string(REPLACE
  "requires ::native::detail::wide_family_is<NATIVE_WIDE_FAMILY,T,B> && NATIVE_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (NATIVE_WIDE_DETAIL::wide_batch_assignable"
  "requires (::native::detail::wide_family_is<NATIVE_WIDE_FAMILY,T,B> && NATIVE_WIDE_DETAIL::wide_arguments<wide<T, N>, B> &&
    (NATIVE_WIDE_DETAIL::wide_batch_assignable"
  docs_wide_text "${docs_wide_text}")
foreach(operation IN ITEMS + - *)
  string(REPLACE "{ a ${operation}= b; }))" "{ a ${operation}= b; })))"
    docs_wide_text "${docs_wide_text}")
endforeach()
# Make the representative-overload scope explicit in the rendered group.
string(REPLACE "This does not repartition a pack or choose a different architecture."
  "This does not repartition a pack or choose a different architecture. The reference shows one representative target-family overload per signature; internal family constraints select the matching implementation for scalar, AVX2, AVX-512 or NEON elements."
  docs_wide_text "${docs_wide_text}")
file(WRITE "${docs_wide}" "${docs_wide_text}")
list(APPEND DOXYGEN_EXCLUDE "${PROJECT_SOURCE_DIR}/src/native/wide.h")
# Preserve the module entry while avoiding a second parse of the unexpanded
# original owning header through the module adapter.
file(READ "${PROJECT_SOURCE_DIR}/src/native.wide.ccm" docs_wide_module)
string(REPLACE "#include \"native/wide.h\"" "" docs_wide_module "${docs_wide_module}")
set(docs_wide_adapter "${docs_source_root}/src/native.wide.ccm")
file(WRITE "${docs_wide_adapter}" "${docs_wide_module}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/native.wide.ccm")
list(APPEND DOXYGEN_EXCLUDE "${PROJECT_SOURCE_DIR}/src/native.wide.ccm")

set(DOXYGEN_ALIASES "license=@code{.spdx}" "endlicense=@endcode"
  "CPUID=@c CPUID" "PAUSE=@c PAUSE" "MONITOR=@c MONITOR" "MWAIT=@c MWAIT"
  "UMONITOR=@c UMONITOR" "UMWAIT=@c UMWAIT")
# Keep relative Markdown links intact in a build-tree rendering copy. The
# original pages, including the license, remain byte-for-byte untouched.
# Paired backticks in the historical license denote quotation marks; escape
# them for Doxygen without a shell filter (whose quoting differs on Windows).
set(docs_markdown_root "${CMAKE_CURRENT_BINARY_DIR}/markdown")
set(docs_pages README.md LICENSE.md CODE_OF_CONDUCT.md src/README.md tests/api/README.md
  doc/building.md tests/omnibus/README.md tests/bf16_profile/README.md tests/neon_fp16/README.md tests/neon_bf16/README.md tests/avx512_fp16/README.md
  tests/exp_policy_refinement/README.md tests/wide_construction/README.md)
file(GLOB_RECURSE docs_guides CONFIGURE_DEPENDS RELATIVE "${PROJECT_SOURCE_DIR}"
  "${PROJECT_SOURCE_DIR}/docs/*.md")
list(APPEND docs_pages ${docs_guides})
set(docs_markdown_inputs)
foreach(page IN LISTS docs_pages)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/${page}")
  file(READ "${PROJECT_SOURCE_DIR}/${page}" docs_markdown)
  if(page STREQUAL "LICENSE.md")
    string(REPLACE "``" "\\`\\`" docs_markdown "${docs_markdown}")
  endif()
  get_filename_component(page_directory "${docs_markdown_root}/${page}" DIRECTORY)
  file(MAKE_DIRECTORY "${page_directory}")
  file(WRITE "${docs_markdown_root}/${page}" "${docs_markdown}")
  list(APPEND docs_markdown_inputs "${docs_markdown_root}/${page}")
endforeach()
# Render nested README files as pages, not implicit directory descriptions.
# Directory descriptions otherwise leak build paths into link tooltips.
set(DOXYGEN_IMPLICIT_DIR_DOCS NO)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${docs_markdown_root}/README.md")
set(DOXYGEN_STRIP_FROM_PATH "${docs_source_root}" "${docs_markdown_root}" "${PROJECT_SOURCE_DIR}")
set(DOXYGEN_HAVE_DOT NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
# Static menus avoid Doxygen 1.18 links to absent short-list/tilde sections.
# API pages, alphabetical indices and the navigation tree remain available.
set(DOXYGEN_HTML_DYNAMIC_MENUS NO)
set(DOXYGEN_HTML_COLORSTYLE AUTO_LIGHT)
set(DOXYGEN_OBFUSCATE_EMAILS NO)
set(DOXYGEN_SOURCE_BROWSER YES)
set(DOXYGEN_INLINE_SOURCES NO)
set(DOXYGEN_FULL_PATH_NAMES YES)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)
set(DOXYGEN_WARN_IF_DOC_ERROR YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
set(DOXYGEN_WARN_IF_UNDOC_ENUM_VAL NO)
set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")
set(DOXYGEN_WARN_LOGFILE "${PROJECT_BINARY_DIR}/docs/warnings.log")
set(DOXYGEN_PROJECT_BRIEF "Vectors, masks and wide register packs for C++26")
doxygen_add_docs(native_docs
  "${PROJECT_SOURCE_DIR}/src"
  "${docs_isa}" "${docs_wide}" "${docs_wide_adapter}"
  ${docs_markdown_inputs}
  WORKING_DIRECTORY "${docs_markdown_root}"
  COMMENT "Build native API documentation and examples")

# Module summaries need aliases for Doxygen 1.18 crawler/navigation destinations.
add_custom_command(TARGET native_docs POST_BUILD
  COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/fix_module_anchors.py"
    "${PROJECT_BINARY_DIR}/docs/html"
  VERBATIM)
