// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/x86/f16c.h>
import native.x86.features;
#include "checks.h"
int main() { return f16c_fixture::run(); }
