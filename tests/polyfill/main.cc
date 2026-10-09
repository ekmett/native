// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#if NATIVE_POLYFILL_HEADERS
#include <native/simd.h>
#include <native/integer.h>
#include <native/packing.h>
#include <native/wide.h>
#else
import native;
#endif

namespace custom_fixture {
  struct encoded { std::uint32_t value; };
}
template<> struct native::simd_traits<custom_fixture::encoded> {
  using storage_type=std::uint32_t;
};
template<class Raw,class Self>
struct native::simd_customization<custom_fixture::encoded,Raw,Self> {
  using value_type=custom_fixture::encoded;
  using native_type=typename Raw::native_type;
  static constexpr std::size_t lanes=Raw::lanes;
  Raw value{};
  constexpr simd_customization() noexcept=default;
  constexpr simd_customization(Raw raw) noexcept : value(raw) {}
  constexpr simd_customization(value_type x) noexcept : value(x.value+1000u) {}
  constexpr native_type to_native() const noexcept { return value.to_native(); }
  template<std::size_t Alignment=1>
  static constexpr Self load_memory(value_type const * p) noexcept {
    std::array<std::uint32_t,lanes> raw{};
    for(std::size_t i=0;i<lanes;++i) raw[i]=p[i].value+1000u;
    return Self(Raw::load(raw.data()));
  }
  template<std::size_t Alignment=1>
  constexpr void store_memory(value_type * p) const noexcept {
    std::array<std::uint32_t,lanes> raw{}; value.store(raw.data());
    for(std::size_t i=0;i<lanes;++i) p[i].value=raw[i]-1000u;
  }
  friend constexpr Self operator+(Self a,Self b) noexcept {
    return Self(a.value+b.value-Raw(1000u));
  }
};

namespace polyfill_test {
  using namespace native;
#if defined(__aarch64__) || defined(_M_ARM64)
  constexpr auto hardware=neon;
  constexpr std::size_t native_lanes=4;
#elif defined(__wasm__)
  constexpr auto hardware=feature_closure(wasm_feature::simd128);
  constexpr std::size_t native_lanes=4;
#else
  constexpr auto hardware=avx2;
  constexpr std::size_t native_lanes=8;
#endif
  constexpr auto permitted=hardware|polyfill;
  template<class T,std::size_t N,isa<> A> concept complete=requires { sizeof(simd<T,N,A>); };
  static_assert(!complete<float,16,hardware>);
#if defined(__aarch64__) || defined(_M_ARM64)
  static_assert(simd<std::uint8_t,17,permitted>::register_lanes==16);
#elif defined(__x86_64__) || defined(_M_X64)
  constexpr auto sse_permitted=feature_closure(x86_feature::sse2)|polyfill;
  static_assert(simd<float,17,sse_permitted>::register_lanes==1);
  static_assert(sizeof(simd<float,17,sse_permitted>)==17*sizeof(float));
#endif
  using decomposed=simd<float,16,permitted>;
  using scalar_emulated=simd<float,16,polyfill>;
  static_assert(decomposed::register_lanes==native_lanes);
  static_assert(decomposed::register_count==16/native_lanes);
  static_assert(sizeof(decomposed)==64);
  static_assert(sizeof(typename simd<float,17,permitted>::mask)==sizeof(std::uint64_t));
  static_assert(sizeof(typename simd<float,17,polyfill>::mask)==sizeof(std::uint64_t));
  static_assert(simd<float,17,permitted>::mask::architecture==permitted);
  static_assert(scalar_emulated::register_lanes==1 && scalar_emulated::register_count==16);
  static_assert(sizeof(scalar_emulated)==64);
  static_assert(std::same_as<typename simd<float,native_lanes,permitted>::native_type,
    typename simd<float,native_lanes,hardware>::native_type>);
  static_assert(!std::same_as<simd<float,native_lanes,permitted>,simd<float,native_lanes,hardware>>);

  template<class V,class U> concept scalar_operand=requires(V value,U scalar) {
    value+scalar; scalar+value; value*scalar; value<scalar; value+=scalar;
  };
  template<class V,class M> concept selectable=requires(V value,M mask) { select(mask,value,value); };
  using integer_emulated=simd<std::uint32_t,17,polyfill>;
  using floating_emulated=simd<float,17,polyfill>;
  static_assert(scalar_operand<integer_emulated,std::int64_t>);
  static_assert(!scalar_operand<integer_emulated,float> && !scalar_operand<integer_emulated,double> &&
    !scalar_operand<integer_emulated,bool>);
  static_assert(!std::constructible_from<integer_emulated,float> && !std::constructible_from<integer_emulated,double>);
  static_assert(selectable<floating_emulated,floating_emulated::mask_type> &&
    selectable<floating_emulated,floating_emulated::vector_mask_type>);
  static_assert(!selectable<floating_emulated,predicate<1,polyfill>> &&
    !selectable<floating_emulated,simd<mask64,17,polyfill>> &&
    !selectable<floating_emulated,simd<float,17,permitted>::mask_type>);

  template<class V> constexpr bool floating(float bias=0.f) {
    std::array<float,V::lanes> input{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=float(i)-3+bias;
    auto a=V::load(input.data());
    auto b=V(2.f);
    auto c=(a+b)*b;
    std::array<float,V::lanes> output{}; c.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=(input[i]+2)*2) return false;
    std::uint64_t less_bits=0,less_equal_bits=0;
    for(std::size_t i=0;i<V::lanes;++i) {
      less_bits|=std::uint64_t(input[i]<2) << i;
      less_equal_bits|=std::uint64_t(input[i]<=2) << i;
    }
    if((a<=b).to_bitset()!=less_equal_bits || (a!=a).to_bitset()!=0) return false;
    auto rounded=floor(a+V(.5f)); rounded.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=input[i]) return false;
    auto comparison=a<b;
    auto chosen=select(comparison,a,b);
    chosen.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=(input[i]<2?input[i]:2)) return false;
    if(comparison.to_bitset()!=less_bits) return false;
    auto fused=fma(a,b,b); fused.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=input[i]*2+2) return false;
    auto root=sqrt(V(4.f)); root.store(output.data());
    for(auto x:output) if(x!=2.f) return false;
    return true;
  }
  template<class V> constexpr bool memory(unsigned seed=0) {
    using T=typename V::value_type;
    std::array<T,V::lanes+2> input{},output{};
    for(std::size_t i=0;i<input.size();++i) input[i]=T(i+1+seed);
    for(std::size_t count=0;count<=V::lanes;++count) {
      auto value=V::load_partial(count?input.data():nullptr,count,T(99));
      output.fill(T(77)); value.store(output.data()+1);
      if(output.front()!=T(77) || output.back()!=T(77)) return false;
      for(std::size_t i=0;i<V::lanes;++i)
        if(output[i+1]!=(i<count?input[i]:T(99))) return false;
      output.fill(T(77)); value.store_partial(count?output.data()+1:nullptr,count);
      for(std::size_t i=0;i<output.size();++i)
        if(output[i]!=(i>0 && i<=count?input[i-1]:T(77))) return false;
    }
    return true;
  }
  template<class V> bool unaligned_memory(unsigned seed) {
    using T=typename V::value_type;
    using W=typename V::word_type;
    std::array<T,V::lanes> input{},output{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=T(i+1+seed);
    alignas(T) std::array<std::byte,sizeof(input)+2> source{},destination{};
    std::memcpy(source.data()+1,input.data(),sizeof(input));
    auto p=reinterpret_cast<T const *>(source.data()+1);
    auto q=reinterpret_cast<T *>(destination.data()+1);
    for(std::size_t count=0;count<=V::lanes;++count) {
      auto value=V::load_partial(count?p:nullptr,count,T(99));
      destination.fill(std::byte{0x5a}); value.storeu(q);
      std::memcpy(output.data(),destination.data()+1,sizeof(output));
      if(destination.front()!=std::byte{0x5a} || destination.back()!=std::byte{0x5a}) return false;
      for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=(i<count?input[i]:T(99))) return false;
      destination.fill(std::byte{0x5a}); value.store_partial(count?q:nullptr,count);
      if(destination.front()!=std::byte{0x5a}) return false;
      for(std::size_t i=1+count*sizeof(T);i<destination.size();++i)
        if(destination[i]!=std::byte{0x5a}) return false;
      std::memcpy(output.data(),destination.data()+1,count*sizeof(T));
      for(std::size_t i=0;i<count;++i) if(output[i]!=input[i]) return false;
    }
    auto value=V::loadu(p); value.storeu(q);
    std::memcpy(output.data(),destination.data()+1,sizeof(output));
    if(output!=input) return false;
    std::array<W,V::lanes> words{},stored{};
    for(std::size_t i=0;i<V::lanes;++i) words[i]=W(0x800001u+i+seed);
    std::memcpy(source.data()+1,words.data(),sizeof(words));
    V::load_bits(reinterpret_cast<W const *>(source.data()+1)).store_bits(reinterpret_cast<W *>(destination.data()+1));
    std::memcpy(stored.data(),destination.data()+1,sizeof(stored));
    return stored==words;
  }
  template<class V> constexpr bool representations() {
    std::array<std::uint32_t,V::lanes> input{},output{};
    constexpr std::array patterns{0u,0x80000000u,1u,0x7f800000u,0x7fa12345u,0x7fc54321u};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=patterns[i%patterns.size()];
    auto value=V::load_bits(input.data()); value.store_bits(output.data());
    if(input!=output) return false;
    auto again=V::from_native(value.to_native()); again.store_bits(output.data());
    return input==output;
  }
  template<isa<> A> constexpr bool integers(unsigned seed=0) {
    using V=simd<std::uint32_t,17,A>;
    std::array<std::uint32_t,V::lanes> input{},output{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=0xfffffff0u+std::uint32_t(i)+seed;
    auto a=V::load(input.data());
    auto result=((a+V(19u))*V(3u)).template right<2>(); result.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i)
      if(output[i]!=std::uint32_t(std::uint32_t(input[i]+19u)*3u)>>2) return false;
    auto population=popcount(a); population.store(output.data());
    for(std::size_t i=0;i<V::lanes;++i) if(output[i]!=std::popcount(input[i])) return false;
    return memory<V>(seed);
  }
  template<class T,std::size_t N,isa<> A> constexpr bool reductions(unsigned seed=0) {
    using V=simd<T,N,A>;
    std::array<T,N> input{};
    std::uint64_t expected=0;
    for(std::size_t i=0;i<N;++i) {
      input[i]=T(std::numeric_limits<T>::max()-T(i+seed));
      expected+=input[i];
    }
    if(reduce_add_widened(V::load(input.data()))!=expected) return false;
    // Broadcast fills physical padding too; reduction includes logical lanes only.
    return reduce_add_widened(V(std::numeric_limits<T>::max()))==
      std::uint64_t(N)*std::numeric_limits<T>::max();
  }
  template<isa<> A> constexpr bool integer_reductions(unsigned seed=0) {
    return reductions<std::uint8_t,17,A>(seed) && reductions<std::uint16_t,17,A>(seed) &&
      reductions<std::uint32_t,17,A>(seed) && reductions<std::uint32_t,64,A>(seed);
  }
  template<class V> concept exact_reducible=requires(V value) { reduce_add_widened(value); };
  static_assert(!exact_reducible<simd<std::int32_t,17,polyfill>> &&
    !exact_reducible<simd<std::uint64_t,17,polyfill>>);

  template<isa<> A> constexpr bool packing() {
    using V=simd<std::uint32_t,16,A>;
    std::array<std::uint32_t,V::lanes> input{};
    for(std::size_t i=0;i<V::lanes;++i) input[i]=0x10000u+std::uint32_t(i);
    auto a=V::load(input.data());
    auto narrow=narrow_concat<std::uint16_t>(a,a+V(16u));
    std::array<std::uint16_t,32> output{}; narrow.store(output.data());
    for(std::size_t i=0;i<output.size();++i) if(output[i]!=i) return false;
    auto widened=pairwise_add_widened(narrow);
    std::array<std::uint32_t,16> sums{}; widened.store(sums.data());
    for(std::size_t i=0;i<sums.size();++i) if(sums[i]!=4*i+1) return false;
    auto words=reinterpret_bits<std::uint8_t>(simd<std::uint32_t,4,A>(0x01010101u));
    std::array<std::uint8_t,16> bytes{}; words.store(bytes.data());
    for(auto value:bytes) if(value!=1) return false;
    return true;
  }
  template<isa<> A> constexpr bool masks() {
    using M=simd<mask32,17,A>;
    auto a=M::from_bitset(0x1ffff);
    auto b=M::from_bitset(0x10001);
    if(!all(a) || !any(b) || !none(a&~a)) return false;
    if((a^b).to_bitset()!=0xfffe) return false;
    using B=simd<bool,17,A>;
    if(!all(B::from_bitset(0x1ffff))) return false;
    return B::from_bitset(0xffffffffffffffffull).to_bitset()==0x1ffff;
  }
  constexpr bool normalized() {
    using B=simd<bool,17,polyfill>;
    B::native_type bytes{}; bytes.fill(255);
    auto booleans=B::from_native(bytes);
    if(booleans.to_native()[0]!=1 || !all(booleans)) return false;
    using M=simd<mask32,17,polyfill>;
    M::native_type words{}; words.fill(7);
    auto masks=M::from_native(words);
    return masks.to_native()[0]==0xffffffffu && all(masks);
  }
  template<isa<> A> constexpr bool custom() {
    using V=simd<custom_fixture::encoded,17,A>;
    using Raw=simd<std::uint32_t,17,A>;
    std::array<custom_fixture::encoded,V::lanes> input{},output{};
    for(std::size_t i=0;i<V::lanes;++i) input[i].value=std::uint32_t(i+1);
    auto a=load_simd<V>(input);
    auto b=a+V(custom_fixture::encoded{4});
    store_simd(output.data(),b);
    for(std::size_t i=0;i<V::lanes;++i) if(output[i].value!=input[i].value+4) return false;
    auto raw=load_simd<Raw>(input.data());
    std::array<std::uint32_t,V::lanes> words{}; raw.store(words.data());
    for(std::size_t i=0;i<V::lanes;++i) if(words[i]!=input[i].value+1000) return false;
    store_simd(output.data(),raw);
    for(std::size_t i=0;i<V::lanes;++i) if(output[i].value!=input[i].value) return false;
    return true;
  }
  template<class V> constexpr bool batching() {
    native::wide<V,3> a(V(2.f));
    auto b=a+a;
    for(auto const & value:b.registers) {
      std::array<float,V::lanes> output{}; value.store(output.data());
      for(auto x:output) if(x!=4.f) return false;
    }
    native::wide<V,0> empty{};
    return (empty+empty).registers.empty();
  }
  static_assert(custom<permitted>() && custom<isa<>(polyfill)>());
  static_assert(batching<decomposed>() && batching<scalar_emulated>());
  static_assert(normalized());
  static_assert(floating<decomposed>() && floating<scalar_emulated>());
  static_assert(memory<simd<float,17,permitted>>() && memory<scalar_emulated>());
  static_assert(representations<decomposed>() && representations<scalar_emulated>());
  static_assert(integers<permitted>() && integers<isa<>(polyfill)>());
  static_assert(integer_reductions<permitted>() && integer_reductions<isa<>(polyfill)>());
  static_assert(packing<permitted>() && packing<isa<>(polyfill)>());
  static_assert(masks<permitted>() && masks<isa<>(polyfill)>());
}

int main(int argc,char **) {
  using namespace polyfill_test;
  // Runtime input prevents this check from collapsing to its constexpr result.
  auto seed=unsigned(argc-1);
  bool scalar_ok=custom<isa<>(polyfill)>() && batching<scalar_emulated>() && normalized() &&
    floating<scalar_emulated>(float(seed)) && memory<scalar_emulated>(seed) &&
    unaligned_memory<scalar_emulated>(seed) && unaligned_memory<integer_emulated>(seed) &&
    representations<scalar_emulated>() && integers<isa<>(polyfill)>(seed) && integer_reductions<isa<>(polyfill)>(seed) &&
    packing<isa<>(polyfill)>() && masks<isa<>(polyfill)>();
#if NATIVE_POLYFILL_SCALAR_ONLY
  return !scalar_ok;
#else
  bool native_ok=custom<permitted>() && batching<decomposed>() &&
    floating<decomposed>(float(seed)) && memory<simd<float,17,permitted>>(seed) &&
    unaligned_memory<simd<float,17,permitted>>(seed) && unaligned_memory<simd<std::uint32_t,17,permitted>>(seed) &&
    representations<decomposed>() && integers<permitted>(seed) && integer_reductions<permitted>(seed) &&
    packing<permitted>() && masks<permitted>();
  return !(scalar_ok && native_ok);
#endif
}
