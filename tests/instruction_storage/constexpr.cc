// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>
#if NATIVE_CONSTEXPR_HEADERS
#include <native/vec.h>
#include <native/wide.h>
#else
import native.simd;
#endif

static_assert([] {
  native::wide<int,2> a{{1,2}},b{{3,4}};
  native::wide<bool,2> masks{{true,false}};
  auto r=native::select(masks,a,b);
  native::wide<int,0> empty{};
  return r.registers[0]==1 && r.registers[1]==4 &&
    native::select(native::wide<bool,0>{},empty,empty).registers.empty();
}());
static_assert([] {
  using V=native::simd<std::int32_t,1,native::scalar>;
  native::wide<V,2> a{{V(1),V(2)}},b{{V(3),V(4)}};
  native::wide<typename V::mask_type,2> masks{{V::mask_type(true),V::mask_type(false)}};
  auto r=native::select(masks,a,b);
  return r.registers[0].to_native()==1 && r.registers[1].to_native()==4;
}());

template<class T> consteval bool scalar_integer() {
  using V=native::simd<T,1,native::scalar>;
  using U=std::make_unsigned_t<T>;
  constexpr T high=std::numeric_limits<T>::max();
  constexpr T low=std::numeric_limits<T>::min();
  if(std::bit_cast<U>((V(high)+V(1)).to_native())!=U(U(high)+U(1))) return false;
  if(std::bit_cast<U>((V(low)-V(1)).to_native())!=U(U(low)-U(1))) return false;
  if((V(3)*V(7)).to_native()!=T(21)) return false;
  if((-V(0)).to_native()!=T(0) || (V(0)|V(high)).to_native()!=high) return false;
  if(!all(V(low)<=V(high)) || any(V(high)<V(low))) return false;
  return true;
}
static_assert(scalar_integer<std::int8_t>() && scalar_integer<std::uint8_t>());
static_assert(scalar_integer<std::int16_t>() && scalar_integer<std::uint16_t>());
static_assert(scalar_integer<std::int32_t>() && scalar_integer<std::uint32_t>());
static_assert(scalar_integer<std::int64_t>() && scalar_integer<std::uint64_t>());

static_assert([] {
  using V=native::simd<float,1,native::scalar>;
  V x(6.f); x+=V(2.f); x-=V(1.f); x*=V(3.f); x/=V(7.f);
  return x.to_native()==3.f && (-x).to_native()==-3.f &&
    all(x==V(3.f)) && none(x!=V(3.f)) && all(x<V(4.f)) && all(x>=V(2.f)) &&
    select(x>V(0.f),x,V(-1.f)).to_native()==3.f &&
    V::from_bits(0x80000000u).bits().to_native()==0x80000000u;
}());

template<class M> consteval bool scalar_mask() {
  auto yes=M::from_bitset(~std::uint64_t{}), no=M(false);
  auto x=yes; x&=yes; x|=no; x^=yes;
  return all(yes) && none(no) && none(x) && all(!no) &&
    all(yes==~no) && none(yes!=yes) && select(yes,yes,no).to_bitset()==1;
}
static_assert(scalar_mask<native::simd<native::mask8,1,native::scalar>>());
static_assert(scalar_mask<native::simd<native::mask16,1,native::scalar>>());
static_assert(scalar_mask<native::simd<native::mask32,1,native::scalar>>());
static_assert(scalar_mask<native::simd<native::mask64,1,native::scalar>>());

#if defined(__aarch64__) || defined(_M_ARM64)
constexpr auto arithmetic_arch=native::neon;
constexpr auto storage_arch=native::neon;
#elif defined(__x86_64__) || defined(_M_X64)
constexpr auto arithmetic_arch=native::avx2;
constexpr auto storage_arch=native::feature_closure(native::x86_feature::sse2);
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64)
// Keep the existing Clang lane-list constructors and their physical padding usable.
static_assert([] {
  native::simd<float,2,arithmetic_arch> a(1.f,-2.f);
  native::simd<float,3,arithmetic_arch> b(1.f,-2.f,3.f);
  native::simd<float,4,arithmetic_arch> c(1.f,-2.f,3.f,4.f);
  return a.value[0]==1.f && a.value[1]==-2.f && a.value[2]==0.f && a.value[3]==0.f &&
    b.value[2]==3.f && b.value[3]==0.f && c.value[0]==1.f && c.value[3]==4.f;
}());

template<class T,std::size_t N,native::isa<> A> consteval bool integer_lanes() {
  using V=native::simd<T,N,A>;
  static_assert(sizeof(V)==sizeof(typename V::native_type));
  static_assert(std::is_trivially_copyable_v<V>);
  std::array<T,N> source;
  for(std::size_t i=0;i<N;++i) source[i]=T(i+1);
  auto v=V(source);
  if(std::bit_cast<std::array<T,N>>(v.to_native())!=source) return false;
  std::array<T,N+2> actual{};
  V::load(source.data()).store(actual.data()+1);
  if(actual.front()!=0 || actual.back()!=0) return false;
  for(std::size_t i=0;i<N;++i) if(actual[i+1]!=source[i]) return false;
  V(7).store(actual.data()+1);
  for(std::size_t i=0;i<N;++i) if(actual[i+1]!=7) return false;
  return true;
}
static_assert(integer_lanes<std::uint8_t,16,arithmetic_arch>());
static_assert(integer_lanes<std::int16_t,8,arithmetic_arch>());
static_assert(integer_lanes<std::uint32_t,4,arithmetic_arch>());
static_assert(integer_lanes<std::int64_t,2,arithmetic_arch>());
static_assert([] {
  native::simd<std::uint64_t,2,arithmetic_arch> v(std::uint64_t{0x8000000000000001ull},std::uint64_t{0xffffffffffffffffull});
  auto a=std::bit_cast<std::array<std::uint64_t,2>>(v.to_native());
  return a[0]==0x8000000000000001ull && a[1]==0xffffffffffffffffull;
}());

#if !NATIVE_CONSTEXPR_HEADERS
template<std::size_t N> consteval bool compact_mask() {
  using P=native::predicate<N,storage_arch>;
  constexpr auto active=[] { if constexpr(N==64) return ~std::uint64_t{}; else return (std::uint64_t{1}<<N)-1; }();
  auto all_lanes=P::from_bitset(~std::uint64_t{});
  auto a=P::from_bitset(0x5555555555555555ull), b=P::from_bitset(0xaaaaaaaaaaaaaaaaull);
  auto x=a; x&=b; x|=a; x^=b;
  return all_lanes.to_bitset()==active && all(all_lanes) && none(!all_lanes) &&
    none(a&b) && all(a|b) && all(a^b) && all(a==~b) && none(a!=a) &&
    x.to_bitset()==active && select(a,b,a).to_bitset()==0 && any(a);
}
static_assert(compact_mask<7>() && compact_mask<64>());

template<class T,std::size_t N,native::isa<> A> consteval bool instruction_lanes() {
  using V=native::simd<T,N,A>;
  std::array<T,N> source;
  for(std::size_t i=0;i<N;++i) source[i]=T(float(i+1));
  V value(source);
  std::array<T,N+2> actual{};
  value.store(actual.data()+1);
  if(actual.front()!=T{} || actual.back()!=T{}) return false;
  for(std::size_t i=0;i<N;++i) if(actual[i+1]!=source[i]) return false;
  for(std::size_t count=0;count<=N;++count) {
    auto partial=V::load_partial(count?source.data():nullptr,count,T(7.f));
    partial.store(actual.data()+1);
    for(std::size_t i=0;i<N;++i) if(actual[i+1]!=(i<count?source[i]:T(7.f))) return false;
    actual.fill(T{});
    partial.store_partial(count?actual.data()+1:nullptr,count);
    if(actual.front()!=T{} || actual.back()!=T{}) return false;
    for(std::size_t i=0;i<N;++i) if(actual[i+1]!=(i<count?source[i]:T{})) return false;
  }
  auto words=std::bit_cast<std::array<T,sizeof(V)/sizeof(T)>>(value.to_native());
  for(std::size_t i=N;i<words.size();++i) if(words[i]!=T{}) return false;
  return true;
}
static_assert(instruction_lanes<std::uint8_t,8,storage_arch>());
static_assert(instruction_lanes<std::uint16_t,4,storage_arch>());
static_assert(instruction_lanes<double,2,storage_arch>());
static_assert(instruction_lanes<native::fp16,4,storage_arch>());
#if defined(__aarch64__) || defined(_M_ARM64)
static_assert(instruction_lanes<native::bf16,4,storage_arch>());
#else
static_assert(integer_lanes<std::uint32_t,8,native::avx2>());
static_assert(integer_lanes<std::uint64_t,8,native::avx512>());
static_assert(instruction_lanes<std::uint8_t,64,native::feature_closure(native::x86_feature::avx512f)>());
#endif
static_assert([] {
  using V=native::simd<native::fp16,4,storage_arch>;
  constexpr std::array<std::uint16_t,4> bits{0x8000,0x7c01,0x7e55,0x0001};
  auto value=V::load_bits(bits.data());
  std::array<std::uint16_t,4> result{};
  V::from_bits(value.bits()).store_bits(result.data());
  return bits==result;
}());
#endif
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64)
template<class T,std::size_t N> consteval bool integer_arithmetic() {
  using V=native::simd<T,N,arithmetic_arch>;
  using U=std::make_unsigned_t<T>;
  V a(std::numeric_limits<T>::max()), b(1);
  std::array<T,N> result{};
  (a+b).store(result.data());
  for(auto x:result) if(std::bit_cast<U>(x)!=U(U(std::numeric_limits<T>::max())+U(1))) return false;
  (V(19)*V(23)).store(result.data());
  for(auto x:result) if(std::bit_cast<U>(x)!=U(19*23)) return false;
  if(!all((a&b)==b) || !all(V(3)<V(7))) return false;
  (V(16)>>native::imm<2>).store(result.data());
  for(auto x:result) if(x!=4) return false;
  auto m=V::mask_type::from_bitset(5);
  if(m.to_bitset()!=(5&((std::uint64_t(1)<<N)-1))) return false;
  if(!all(m|~m) || !none(m&~m)) return false;
  std::array<T,N> source{}; for(std::size_t i=0;i<N;++i) source[i]=T(i+1);
  for(std::size_t n=0;n<=N;++n) {
    auto value=V::load_partial(n?source.data():nullptr,n,T(42));
    value.store(result.data());
    for(std::size_t i=0;i<N;++i) if(result[i]!=(i<n?source[i]:T(42))) return false;
    result.fill(T(99)); value.store_partial(n?result.data():nullptr,n);
    for(std::size_t i=0;i<N;++i) if(result[i]!=(i<n?source[i]:T(99))) return false;
  }
  return true;
}
static_assert(integer_arithmetic<std::int8_t,16>() && integer_arithmetic<std::uint8_t,16>());
static_assert(integer_arithmetic<std::int16_t,8>() && integer_arithmetic<std::uint16_t,8>());
static_assert(integer_arithmetic<std::int32_t,4>() && integer_arithmetic<std::uint32_t,4>());
static_assert(integer_arithmetic<std::int64_t,2>() && integer_arithmetic<std::uint64_t,2>());
static_assert(integer_arithmetic<std::int32_t,2>() && integer_arithmetic<std::uint32_t,3>());

template<std::size_t N> consteval bool float_storage() {
  using V=native::simd<float,N,arithmetic_arch>;
  std::array<std::uint32_t,N> words{};
  for(std::size_t i=0;i<N;++i) words[i]=0x7f800001u+std::uint32_t(i);
  std::array<std::uint32_t,N> result{};
  V::from_native(V::load_bits(words.data()).to_native()).store_bits(result.data());
  if(words!=result) return false;
  std::array<float,N> values{}; for(std::size_t i=0;i<N;++i) values[i]=float(i+1);
  std::array<float,N> actual{};
  V(values).store(actual.data()); if(actual!=values) return false;
  V(3.f).store(actual.data()); for(auto x:actual) if(x!=3.f) return false;
  for(std::size_t n=0;n<=N;++n) {
    auto value=V::load_partial(n?values.data():nullptr,n,7.f);
    value.store(actual.data());
    for(std::size_t i=0;i<N;++i) if(actual[i]!=(i<n?values[i]:7.f)) return false;
    actual.fill(99.f); value.store_partial(n?actual.data():nullptr,n);
    for(std::size_t i=0;i<N;++i) if(actual[i]!=(i<n?values[i]:99.f)) return false;
  }
  return true;
}
static_assert(float_storage<1>() && float_storage<2>() && float_storage<3>() && float_storage<4>());

#if !NATIVE_CONSTEXPR_HEADERS
template<class T,std::size_t N,native::isa<> A> consteval bool half_storage() {
  using V=native::simd<T,N,A>;
  std::array<std::uint16_t,N> words{};
  for(std::size_t i=0;i<N;++i) words[i]=std::uint16_t(0x7c01u+i);
  std::array<std::uint16_t,N> result{};
  V::from_native(V::load_bits(words.data()).to_native()).store_bits(result.data());
  if(result!=words) return false;
  std::array<T,N> values{}; for(std::size_t i=0;i<N;++i) values[i]=T::from_bits(words[i]);
  V(values).store_bits(result.data()); if(result!=words) return false;
  V::load_bits(words.data()).store(values.data());
  for(std::size_t i=0;i<N;++i) if(values[i].to_bits()!=words[i]) return false;
  for(std::size_t n=0;n<=N;++n) {
    auto value=V::load_partial(n?values.data():nullptr,n,T::from_bits(0x8000));
    value.store_bits(result.data());
    for(std::size_t i=0;i<N;++i) if(result[i]!=(i<n?words[i]:0x8000)) return false;
    std::array<T,N> output{}; value.store_partial(n?output.data():nullptr,n);
    for(std::size_t i=0;i<N;++i) if(output[i].to_bits()!=(i<n?words[i]:0)) return false;
  }
  return true;
}
#if defined(__aarch64__) || defined(_M_ARM64)
static_assert(half_storage<native::fp16,8,native::neon_fp16>());
static_assert(half_storage<native::bf16,8,native::neon_bf16>());
#else
static_assert(float_storage<8>());
static_assert(half_storage<native::fp16,8,native::avx512_fp16>());
static_assert(half_storage<native::fp16,32,native::avx512_fp16>());
static_assert(half_storage<native::bf16,8,native::avx512_bf16>());
static_assert(half_storage<native::bf16,32,native::avx512_bf16>());
#endif
#endif
#endif

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64)
static_assert([] {
  native::simd<float,4,arithmetic_arch> x(1.f,2.f,3.f,4.f);
  x.xy=x.yx;
  auto y=x.zyx;
  return x.x==2.f && x.y==1.f && y.x==3.f && y.y==1.f && y.z==2.f;
}());
static_assert([] {
  native::simd<std::int32_t,3,arithmetic_arch> x(1,2,3);
  x.yz=x.xy;
  auto y=x.zyxx;
  return x.x==1 && x.y==1 && x.z==2 && y.w==1 && x.value[3]==0;
}());

template<class T,std::size_t N,native::isa<> A> consteval bool compact_lanes() {
  using V=native::simd<T,N,A>;
  std::array<T,N> input{};
  for(std::size_t i=0;i<N;++i) {
    if constexpr(std::same_as<T,float>) input[i]=std::bit_cast<float>(0x7f800001u+std::uint32_t(i));
    else input[i]=T(i+1);
  }
  auto source=V::load(input.data());
  for(unsigned bits=0;bits<(1u<<N);++bits) {
    auto mask=V::mask::from_bitset(bits);
    auto result=native::compress(mask,source,T{});
    if(result.count!=std::popcount(bits)) return false;
    auto restored=native::expand(mask,result.value,V(T{}));
    std::array<T,N> actual{}; restored.store(actual.data());
    for(std::size_t i=0;i<N;++i)
      if(std::bit_cast<std::uint32_t>(actual[i])!=std::bit_cast<std::uint32_t>((bits>>i)&1?input[i]:T{})) return false;
    std::array<T,N+2> output{};
    for(std::size_t capacity=0;capacity<=N+1;++capacity) {
      output.fill(T{});
      auto written=native::compress_store(capacity?output.data()+1:nullptr,capacity,mask,source);
      auto expected=result.count<capacity?result.count:capacity;
      if(written!=expected || output.front()!=T{} || output.back()!=T{}) return false;
      std::array<T,N> packed{}; result.value.store(packed.data());
      for(std::size_t i=0;i<N;++i)
        if(std::bit_cast<std::uint32_t>(output[i+1])!=std::bit_cast<std::uint32_t>(i<written?packed[i]:T{})) return false;
    }
  }
  return true;
}
static_assert(compact_lanes<float,1,native::scalar>() && compact_lanes<std::int32_t,1,native::scalar>());
static_assert(compact_lanes<float,2,arithmetic_arch>() && compact_lanes<std::int32_t,3,arithmetic_arch>());
static_assert(compact_lanes<float,4,arithmetic_arch>() && compact_lanes<std::uint32_t,4,arithmetic_arch>());
#endif
int main() {}
