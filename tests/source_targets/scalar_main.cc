// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Stand-alone developer link proof: scalar target scopes must not introduce
// compiler-rt dispatch dependencies. The full fixture calls this from macros.cc.
int scalar_result(int);
int main() { return scalar_result(4)==5?0:1; }
