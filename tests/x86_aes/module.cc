// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"

import native.x86.aes;
#include "checks.h"

int main() {
  return aes_fixture::run();
}
