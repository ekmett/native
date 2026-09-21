// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <type_traits>
import native;

template<class T,std::size_t N,native::isa A>
concept complete_simd = requires { sizeof(native::simd<T,N,A>); };

template<class V> bool check() {
  using T=typename V::value_type;
  constexpr auto n=V::lanes;
  static_assert(std::same_as<native::mask<V>,typename V::mask_type>);
  static_assert(std::is_trivially_copyable_v<V>);
  static_assert(alignof(V)>=alignof(typename V::native_type));
  using M=native::mask<V>;
  auto p=M::from_bitset(0x55), q=M::from_bitset(0x33);
  if(!all(p==p) || any(p!=p) || !none(p&!p)) return false;
  if(select(p,q,!q).to_bitset()!=((p&q)|(!p&!q)).to_bitset()) return false;
  auto compound=p; compound&=q;
  if(compound.to_bitset()!=(p&q).to_bitset()) return false;
  compound|=p; compound^=q;
  if(compound.to_bitset()!=(((p&q)|p)^q).to_bitset()) return false;
  // Importing an instruction register copies its bits, including unused bytes.
  using R=typename V::native_type;
  std::array<std::byte,sizeof(R)> raw_bytes; raw_bytes.fill(std::byte{0xa5});
  auto imported=V::from_native(std::bit_cast<R>(raw_bytes));
  if(std::bit_cast<decltype(raw_bytes)>(imported.to_native())!=raw_bytes) return false;
  if constexpr(std::same_as<T,native::fp16> || std::same_as<T,native::bf16>) {
    auto words=imported.bits();
    if(std::bit_cast<decltype(raw_bytes)>(words.to_native())!=raw_bytes) return false;
    if(std::bit_cast<decltype(raw_bytes)>(V::from_bits(words).to_native())!=raw_bytes) return false;
  }

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
  static_assert(!complete_simd<std::uint16_t,(std::size_t{1}<<63)+4,a>);
  bool ok=check<native::simd<std::uint8_t,8,a>>() && check<native::simd<std::int8_t,8,a>>() &&
    check<native::simd<std::int16_t,4,a>>() && check<native::simd<std::uint16_t,4,a>>() &&
    check<native::simd<native::fp16,4,a>>() && check<native::simd<native::fp16,8,a>>() &&
    check<native::simd<native::bf16,4,a>>() && check<native::simd<native::bf16,8,a>>() &&
    check<native::simd<double,2,a>>();
#elif defined(__x86_64__) || defined(_M_X64)
  constexpr auto a=native::feature_closure(native::x86_feature::sse2);
  static_assert(!complete_simd<std::uint16_t,(std::size_t{1}<<63)+4,a>);
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
