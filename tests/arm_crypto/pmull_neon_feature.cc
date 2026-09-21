// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/pmull.h>
auto invalid(poly8x8_t a, poly8x8_t b) { return native::pmull<native::scalar>(a, b); }
