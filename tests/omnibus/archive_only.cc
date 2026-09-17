// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__) || defined(__FMA__)
#error Linking the archive must not add native ISA options to a baseline target
#endif
int main() { return 0; }
