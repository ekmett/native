// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/x86/pclmul.h>
#include <native/x86/vpclmul.h>
import native.x86.features;
#include "checks.h"
int main() { return pclmul_fixture::run(); }
