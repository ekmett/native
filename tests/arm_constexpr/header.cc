// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/arm/crc.h>
#include "crc_checks.h"
template<auto Arch> concept crc_family = requires { native::crc32<Arch>(std::uint32_t{},std::uint8_t{1}); };
static_assert(crc_family<native::isa<native::arm>{}>);
static_assert(!crc_family<native::isa<native::x86>{}> && !crc_family<native::isa<native::wasm>{}>);
int main() { return 0; }
