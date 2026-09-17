# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

find_package(Doxygen 1.12 REQUIRED)
set(DOXYGEN_PROJECT_BRIEF "C++26 vectors, masks and wide register packs")
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_FILE_PATTERNS *.h *.cc *.ccm *.md)
set(DOXYGEN_EXTENSION_MAPPING "ccm=C++")
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
set(DOXYGEN_STRIP_FROM_PATH "${PROJECT_SOURCE_DIR}")
doxygen_add_docs(simd_docs "${PROJECT_SOURCE_DIR}/src/cxx" "${PROJECT_SOURCE_DIR}/src/shared" "${PROJECT_SOURCE_DIR}/README.md" "${PROJECT_SOURCE_DIR}/docs/modules.md" "${PROJECT_SOURCE_DIR}/doc/building.md"
  COMMENT "Build simd API documentation")
