// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
#include <native/arm/aes.h>
#include <native/arm/pmull.h>
#include <native/arm/sha.h>
import native.arm.features;
#include "checks.h"
int main(int argc, char**) { return crypto_fixture::run(argc); }
