// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.pclmul;
import native.x86.vpclmul;
#include "checks.h"
int main() { return pclmul_fixture::run(); }
