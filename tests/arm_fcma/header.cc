// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/arm/fcma.h>
import native.arm.features;
#include "checks.h"

int main(int argc, char**) { return fcma_fixture::run(argc); }
