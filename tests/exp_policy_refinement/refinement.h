// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include "../../src/native/abi_refinement.h"
namespace refinement_test {
  template<class... Lists> using refinement=native::detail::abi_refinement<Lists...>;
}
