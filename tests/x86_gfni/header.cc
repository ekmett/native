// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/x86/gfni.h>
import native.x86.features;
#include "checks.h"

int main(int argc, char**) { return gfni_fixture::run(argc); }
