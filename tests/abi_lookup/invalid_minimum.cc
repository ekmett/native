// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
constexpr auto invalid=native::target_entry{native::avx2,native::detail::invalid_features};
using policies=native::isa_list<invalid,native::avx2>;
constexpr auto chosen=native::abi_lookup<native::avx2,policies>::index;
