// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <type_traits>
#if SIMD_TEST_IMPORT
import simd.wide;
#else
#include <simd/wide.h>
#endif
#include "traits.h"

struct initialized { int value=17; };
struct no_default { no_default()=delete; };
struct no_destructor { ~no_destructor()=delete; };
struct move_only {
  int value=11;
  constexpr move_only()=default;
  move_only(move_only const &)=delete;
  constexpr move_only(move_only &&)=default;
};
struct immobile {
  immobile()=delete;
  immobile(immobile const &)=delete;
  immobile(immobile &&)=delete;
};
struct events {
  int constructed=0,destroyed=0,caught=0;
  bool operator==(events const &) const=default;
};
events observed;
bool fail_construction=false;
struct tracked {
  tracked() noexcept(false) {
    ++observed.constructed;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    if(fail_construction) throw 1;
#endif
  }
  ~tracked() noexcept { ++observed.destroyed; }
};
template<class T> consteval bool check_traits() {
  return same_traits<T,0>() && same_traits<T,1>() && same_traits<T,3>();
}
static_assert(check_traits<int>() && check_traits<const int>() &&
  check_traits<initialized>() && check_traits<no_default>() &&
  check_traits<no_destructor>() && check_traits<move_only>() &&
  check_traits<immobile>() && check_traits<tracked>());

constexpr bool constants() {
  simd::wide<int,0> empty;
  simd::wide<int,3> zero{};
  simd::wide<initialized,3> member_default;
  simd::wide<no_default,0> deleted_element;
  simd::wide<move_only,0> movable;
  auto moved=static_cast<decltype(movable) &&>(movable);
  return empty.registers.empty() && deleted_element.registers.empty() &&
    moved.registers.empty() && zero.get<0>()==0 && zero.get<2>()==0 &&
    member_default.get<0>().value==17 && member_default.get<2>().value==17;
}
static_assert(constants());

template<class W,bool Value> events construction() {
  observed={};
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  try {
#endif
    if constexpr(Value) { W value{}; (void)value; }
    else { W value; (void)value; }
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  } catch(int) { ++observed.caught; }
#endif
  return observed;
}
template<std::size_t N,bool Value> bool effects() {
  auto before=construction<old_wide<tracked,N>,Value>();
  auto after=construction<simd::wide<tracked,N>,Value>();
  return before==after;
}
bool effects() {
  return effects<0,false>() && effects<0,true>() && effects<1,false>() &&
    effects<1,true>() && effects<3,false>() && effects<3,true>();
}
int native_construction();
int main(int argc,char ** argv) {
  if(argc==2) {
    // Some empty-array implementations promise noexcept yet construct a dummy
    // throwing T. Compare that existing termination contract in child processes.
    std::set_terminate([] {
      std::printf("terminate %d %d %d\n",observed.constructed,observed.destroyed,observed.caught);
      std::fflush(stdout);
      std::_Exit(86);
    });
    fail_construction=true;
    events result;
    if(std::strcmp(argv[1],"old-default")==0) result=construction<old_wide<tracked,0>,false>();
    else if(std::strcmp(argv[1],"new-default")==0) result=construction<simd::wide<tracked,0>,false>();
    else if(std::strcmp(argv[1],"old-value")==0) result=construction<old_wide<tracked,0>,true>();
    else if(std::strcmp(argv[1],"new-value")==0) result=construction<simd::wide<tracked,0>,true>();
    else return 4;
    std::printf("returned %d %d %d\n",result.constructed,result.destroyed,result.caught);
    return 0;
  }
  if(!constants() || !effects()) return 1;
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
  fail_construction=true;
  if(!effects<1,false>() || !effects<1,true>() || !effects<3,false>() || !effects<3,true>()) return 2;
  std::puts("custom default/value effects and nonempty throwing effects preserved");
#else
  std::puts("custom empty/nonempty default/value construction effects preserved; exceptions disabled");
#endif
  return native_construction();
}
