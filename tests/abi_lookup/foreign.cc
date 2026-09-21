// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
using namespace native;
struct cpu { isa<arm> present{},observed{}; };
#if NATIVE_FOREIGN_CASE == 0
constexpr auto rejected=isa<x86>{}&isa<arm>{};
#elif NATIVE_FOREIGN_CASE == 1
constexpr isa<arm> rejected=isa<x86>{};
#elif NATIVE_FOREIGN_CASE == 2
constexpr auto rejected=target_entry{avx2,isa<arm>{}};
#elif NATIVE_FOREIGN_CASE == 3
constexpr auto rejected=target<avx2,neon>;
#elif NATIVE_FOREIGN_CASE == 4
constexpr auto rejected=abi_lookup<avx2,isa_list<neon>>::index;
#elif NATIVE_FOREIGN_CASE == 5
constexpr auto rejected=classify_isa(cpu{},isa<x86>{});
#elif NATIVE_FOREIGN_CASE == 6
constexpr auto rejected=with_isa(isa_list<avx2>{},cpu{},[]<auto>() {});
#elif NATIVE_FOREIGN_CASE == 7
constexpr auto rejected=with_isa(isa_list<neon>{},cpu{},[]<auto>() {},isa<x86>{});
#endif
