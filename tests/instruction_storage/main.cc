// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <type_traits>
import native;

template<class V> bool check() {
  using T=typename V::value_type;
  constexpr auto n=V::lanes;
  static_assert(std::same_as<native::mask<V>,typename V::mask_type>);
  static_assert(std::is_trivially_copyable_v<V>);
  std::array<T,n> input;
  for(std::size_t i=0;i<n;++i) {
    if constexpr(std::same_as<T,native::fp16> || std::same_as<T,native::bf16>)
      input[i]=T::from_bits(std::uint16_t(0x7c01+i));
    else input[i]=T(i+1);
  }
  std::array<T,n+2> result{};
  auto value=V::load(input.data());
  value.store(result.data()+1);
  for(std::size_t i=0;i<n;++i)
    if(std::bit_cast<std::array<std::byte,sizeof(T)>>(input[i]) !=
       std::bit_cast<std::array<std::byte,sizeof(T)>>(result[i+1])) return false;
  if(std::bit_cast<std::array<std::byte,sizeof(T)>>(result.front()) != std::array<std::byte,sizeof(T)>{} ||
     std::bit_cast<std::array<std::byte,sizeof(T)>>(result.back()) != std::array<std::byte,sizeof(T)>{}) return false;
  for(std::size_t count=0;count<=n;++count) {
    auto part=V::load_partial(count?input.data():nullptr,count);
    std::array<T,n> output{};
    part.store(output.data());
    for(std::size_t i=0;i<n;++i) {
      auto expected=i<count?input[i]:T{};
      if(std::bit_cast<std::array<std::byte,sizeof(T)>>(output[i]) !=
         std::bit_cast<std::array<std::byte,sizeof(T)>>(expected)) return false;
    }
    part.store_partial(count?output.data():nullptr,count);
  }
  auto zero=V{};
  std::array<T,n> output;
  zero.store(output.data());
  for(auto item:output)
    if(std::bit_cast<std::array<std::byte,sizeof(T)>>(item) != std::array<std::byte,sizeof(T)>{}) return false;
  if constexpr(std::same_as<T,native::fp16> || std::same_as<T,native::bf16>) {
    auto roundtrip=V::from_bits(value.bits());
    std::array<std::uint16_t,n> words;
    roundtrip.store_bits(words.data());
    for(std::size_t i=0;i<n;++i) if(words[i]!=input[i].to_bits()) return false;
  }
  return true;
}
int main() {
#if defined(__aarch64__) || defined(_M_ARM64)
  constexpr auto a=native::neon;
  bool ok=check<native::simd<std::uint8_t,8,a>>() && check<native::simd<std::int8_t,8,a>>() &&
    check<native::simd<std::int16_t,4,a>>() && check<native::simd<std::uint16_t,4,a>>() &&
    check<native::simd<native::fp16,4,a>>() && check<native::simd<native::fp16,8,a>>() &&
    check<native::simd<native::bf16,4,a>>() && check<native::simd<native::bf16,8,a>>() &&
    check<native::simd<double,2,a>>();
#elif defined(__x86_64__) || defined(_M_X64)
  constexpr auto a=native::feature_closure(native::x86_feature::sse2);
  bool ok=check<native::simd<std::uint8_t,16,a>>() && check<native::simd<std::int16_t,8,a>>() &&
    check<native::simd<std::uint16_t,4,a>>() && check<native::simd<std::uint64_t,2,a>>() &&
    check<native::simd<float,4,a>>() && check<native::simd<double,2,a>>() &&
    check<native::simd<native::fp16,4,a>>() && check<native::simd<native::fp16,8,a>>();
  using mask=native::mask<native::simd<std::uint8_t,16,a>>;
  ok=ok && mask::from_bits(~std::uint64_t{}).bits()==0xffff && all(mask(true)) && none(mask{});
#else
  bool ok=true;
#endif
  if(!ok) { std::puts("instruction register storage failed"); return 1; }
  std::puts("instruction register representations, logical bounds and empty tails passed");
}
