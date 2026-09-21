// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native.arm.aes;
import native.arm.pmull;
import native.arm.sha;
#include "checks.h"
int main(int argc, char**) { return crypto_fixture::run(argc); }
