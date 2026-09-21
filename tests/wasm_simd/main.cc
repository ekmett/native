// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <cstdio>
#include <type_traits>
import native;
constexpr auto a=native::feature_closure(native::wasm_feature::simd128);
template<class T> using V=native::simd<T,16/sizeof(T),a>;
template<class T> __attribute__((target("simd128"))) constexpr bool arithmetic(T three=T(3),T two=T(2)) {
  V<T> x(three),y(two);
  auto sum=x+y;auto difference=x-y;
  std::array<T,V<T>::lanes> values{};sum.store(values.data());
  for(auto v:values)if(v!=T(5))return false;
  if(!all(sum>difference)||any(sum==difference))return false;
  auto chosen=select(x>y,sum,difference);chosen.store(values.data());
  for(auto v:values)if(v!=T(5))return false;
  if constexpr(sizeof(T)>1) {auto p=x*y;p.store(values.data());for(auto v:values)if(v!=T(6))return false;}
  if constexpr(std::is_floating_point_v<T>) {auto q=x/y;q.store(values.data());for(auto v:values)if(v!=T(1.5))return false;}
  else {auto shifted=(x<<unsigned(sizeof(T)*8+1));shifted.store(values.data());for(auto v:values)if(v!=T(6))return false;}
  return true;
}
static_assert(arithmetic<std::int8_t>() && arithmetic<std::uint8_t>());
static_assert(arithmetic<std::int16_t>() && arithmetic<std::uint16_t>());
static_assert(arithmetic<std::int32_t>() && arithmetic<std::uint32_t>());
static_assert(arithmetic<std::int64_t>() && arithmetic<std::uint64_t>());
static_assert(arithmetic<float>() && arithmetic<double>());
volatile int input_three=3;
volatile int input_two=2;
__attribute__((target("simd128"),noinline)) bool runtime() {
  return arithmetic<std::int8_t>(std::int8_t(input_three),std::int8_t(input_two)) && arithmetic<std::uint8_t>(std::uint8_t(input_three),std::uint8_t(input_two)) &&
    arithmetic<std::int16_t>(std::int16_t(input_three),std::int16_t(input_two)) && arithmetic<std::uint16_t>(std::uint16_t(input_three),std::uint16_t(input_two)) &&
    arithmetic<std::int32_t>(std::int32_t(input_three),std::int32_t(input_two)) && arithmetic<std::uint32_t>(std::uint32_t(input_three),std::uint32_t(input_two)) &&
    arithmetic<std::int64_t>(std::int64_t(input_three),std::int64_t(input_two)) && arithmetic<std::uint64_t>(std::uint64_t(input_three),std::uint64_t(input_two)) &&
    arithmetic<float>(float(input_three),float(input_two)) && arithmetic<double>(double(input_three),double(input_two));
}
int main(){if(!runtime())return 1;std::puts("Wasm SIMD128 arithmetic and constexpr contracts passed");}
