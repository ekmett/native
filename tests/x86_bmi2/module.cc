// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.x86.bmi2;
#include "checks.h"

int main(int argc, char**) { return bmi2_fixture::run(argc); }
