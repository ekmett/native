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
#include <native/simd/math/exp.h>
#include <native/simd/math/bits.h>
#else
import native;
import native.math;
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
  template<isa<> A> bool math_graph(unsigned seed=0) {
    constexpr auto raw=[] { auto result=A; result.allow_polyfill=false; return result; }();
    using V=simd<float,17,A>; using C=simd<float,1,raw>;
    std::array<float,17> input{},output{};
    for(std::size_t i=0;i<17;++i) input[i]=float(i+seed)/16.f-0.5f;
    auto x=V::load(input.data()); auto positive=x+V(2.f);
    auto check=[&](V result,auto operation,bool shifted=false) {
      result.store(output.data());
      for(std::size_t i=0;i<17;++i) {
        std::array<float,1> expected{}; operation(C(shifted?input[i]+2.f:input[i])).store(expected.data());
        if(std::bit_cast<std::uint32_t>(expected[0])!=std::bit_cast<std::uint32_t>(output[i])) return false;
      }
      return true;
    };
#define POLYFILL_CHECK_MATH(NAME) if(!check(native::NAME(x),[](C v) { return native::NAME(v); })) return false;
    POLYFILL_CHECK_MATH(exp)
    POLYFILL_CHECK_MATH(exp2)
    POLYFILL_CHECK_MATH(expm1)
    POLYFILL_CHECK_MATH(damping_gain)
    POLYFILL_CHECK_MATH(log1p)
    POLYFILL_CHECK_MATH(tanh)
    if(!check(native::sin(x),[](C v) { return ::math::sin(v); }) ||
       !check(native::cos(x),[](C v) { return ::math::cos(v); })) return false;
#undef POLYFILL_CHECK_MATH
    if(!check(native::log(positive),[](C v) { return native::log(v); },true) ||
       !check(native::log2(positive),[](C v) { return native::log2(v); },true) ||
       !check(native::atan2(x,V(2.f)),[](C v) { return native::atan2(v,C(2.f)); })) return false;
    auto pair=native::sincos(x);
    if(!check(pair.first,[](C v) { return ::math::sin(v); }) || !check(pair.second,[](C v) { return ::math::cos(v); })) return false;
    std::array<V,3> batch{x,x,x};
    auto exponentials=native::exp(batch,std::false_type{},std::integral_constant<unsigned,6>{});
    for(auto value:exponentials) if(!check(value,[](C v) { return native::exp(v); })) return false;
    auto paired=native::sincos(batch);
    for(auto value:paired.first) if(!check(value,[](C v) { return ::math::sin(v); })) return false;
    native::wide<V,3> wide_input{batch}; auto wide_result=native::log1p(wide_input);
    for(auto value:wide_result.registers) if(!check(value,[](C v) { return native::log1p(v); })) return false;
    std::array<V,0> empty{};
    if(!native::exp(empty).empty() || !native::sincos(empty).first.empty()) return false;
    constexpr std::array special{0x7f800000u,0xff800000u,0x7fc12345u,0x80000000u,0u,1u,0x80000001u,0xbf800000u};
    for(std::size_t i=0;i<17;++i) input[i]=std::bit_cast<float>(special[i%special.size()]);
    auto exceptional=V::load(input.data());
    if(!check(native::exp(exceptional),[](C v) { return native::exp(v); }) ||
       !check(native::log(exceptional),[](C v) { return native::log(v); }) ||
       !check(native::tanh(exceptional),[](C v) { return native::tanh(v); })) return false;
    std::array<std::uint32_t,17> representations{}; representations.fill(0x80000001u);
    native::flush_to_zero(V::load_bits(representations.data())).store_bits(representations.data());
    for(auto word:representations) if(word!=0x80000000u) return false;
    return true;
  }
  template<isa<> A> constexpr bool helper_graph(unsigned seed=0) {
    using F=simd<float,17,A>; using U=simd<std::uint32_t,17,A>;
    using M=simd<mask32,17,A>;
    constexpr std::uint64_t bits=0x15555;
    auto mask=M::from_bitset(bits);
    auto compact=to_predicate(mask);
    if(to_bool(compact).to_bitset()!=bits || to_vector_mask<mask32>(compact).to_bitset()!=bits ||
       mask_cast<mask16>(mask).to_bitset()!=bits) return false;
    std::array<std::uint32_t,17> words{}; mask_bits<std::uint32_t>(compact).store(words.data());
    for(std::size_t i=0;i<17;++i) if(words[i]!=(((bits>>i)&1)?~0u:0u)) return false;
    mask_bits(mask).store(words.data());
    for(std::size_t i=0;i<17;++i) if(words[i]!=(((bits>>i)&1)?~0u:0u)) return false;
    std::array<float,17> input{},output{};
    for(std::size_t i=0;i<17;++i) input[i]=float(i+seed)+.75f;
    auto value=F::load(input.data());
    masked_add(compact,F(-9.f),value,F(2.f)).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=(((bits>>i)&1)?input[i]+2:-9.f)) return false;
    masked_mul_zero(mask,value,F(2.f)).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=(((bits>>i)&1)?input[i]*2:0.f)) return false;
    convert<std::uint32_t>(value).store(words.data());
    for(std::size_t i=0;i<17;++i) if(words[i]!=i+seed) return false;
    convert<float>(U::load(words.data())).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=float(i+seed)) return false;
    if(value.template get<16>()!=input[16] || value.template set<8>(-7.f).template get<8>()!=-7.f) return false;
    broadcast(value,imm<9>).store(output.data());
    for(auto lane:output) if(lane!=input[9]) return false;
    normal_pow2(F(3.f)).store(output.data());
    for(auto lane:output) if(lane!=8.f) return false;
    masked_scaleb(compact,F(-9.f),value,F(2.75f)).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=(((bits>>i)&1)?input[i]*4:-9.f)) return false;
    scaleb(value,F(-1.f)).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=input[i]*.5f) return false;
    float sum=0; for(auto lane:input) sum+=lane;
    if(reduce_add(value)!=sum) return false;
    auto shifts=U(1u)<<imm<31>; shifts.store(words.data());
    for(auto word:words) if(word!=0x80000000u) return false;
    (shifts>>imm<31>).store(words.data());
    for(auto word:words) if(word!=1) return false;
    std::array<std::uint32_t,17> counts{};
    for(std::size_t i=0;i<17;++i) counts[i]=std::uint32_t(i+23);
    counts.back()=0xffffffffu;
    (U(1u)<<U::load(counts.data())).store(words.data());
    for(std::size_t i=0;i<17;++i) if(words[i]!=(counts[i]<32?1u<<counts[i]:0)) return false;
    auto packed=compress(F::mask_type::from_bitset(bits),value,-99.f);
    if(packed.count!=9) return false;
    packed.value.store(output.data());
    for(std::size_t i=0;i<9;++i) if(output[i]!=input[2*i]) return false;
    for(std::size_t i=9;i<17;++i) if(output[i]!=-99.f) return false;
    expand(F::mask_type::from_bitset(bits),packed.value,F(-9.f)).store(output.data());
    for(std::size_t i=0;i<17;++i) if(output[i]!=(((bits>>i)&1)?input[i]:-9.f)) return false;
    output.fill(-7.f);
    if(compress_store(output.data(),3,F::mask_type::from_bitset(bits),value)!=3 ||
       compress_store(static_cast<float *>(nullptr),0,F::mask_type::from_bitset(bits),value)!=0) return false;
    for(std::size_t i=0;i<17;++i) if(output[i]!=(i<3?input[2*i]:-7.f)) return false;
    auto selected=bit_select(U(0x80000000u),F(-0.f),F(1.f)); selected.store_bits(words.data());
    for(auto word:words) if(word!=0xbf800000u) return false;
    return true;
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
  template<class T,isa<> A> constexpr bool extra_storage() {
    using V=simd<T,17,A>;
    using W=typename V::word_type;
    std::array<W,17> input{},output{};
    for(std::size_t i=0;i<17;++i) input[i]=W(~W{}-W(i));
    auto value=V::load_bits(input.data()); value.store_bits(output.data());
    if(input!=output) return false;
    V::from_native(value.to_native()).store_bits(output.data());
    return input==output;
  }
  template<class T,isa<> A> constexpr bool extra_arithmetic() {
    using V=simd<T,17,A>;
    std::array<T,17> output{};
    auto result=fma(V(T(2.f)),V(T(3.f)),V(T(1.f)));
    result.store(output.data());
    for(auto x:output) if(float(x)!=7.f) return false;
    sqrt(V(T(4.f))).store(output.data());
    for(auto x:output) if(float(x)!=2.f) return false;
    floor(V(T(2.5f))).store(output.data());
    for(auto x:output) if(float(x)!=2.f) return false;
    abs(V(T(-2.f))).store(output.data());
    for(auto x:output) if(float(x)!=2.f) return false;
    return (V(T(1.f))<V(T(2.f))).to_bitset()==0x1ffff;
  }
  static_assert(extra_storage<double,isa<>(polyfill)>() && extra_arithmetic<double,isa<>(polyfill)>());
#if !NATIVE_POLYFILL_HEADERS
  static_assert(extra_storage<fp16,isa<>(polyfill)>() && extra_storage<bf16,isa<>(polyfill)>());
  static_assert(extra_storage<fp16,permitted>() && extra_storage<bf16,permitted>());
  static_assert(extra_arithmetic<fp16,isa<>(polyfill)>() && extra_arithmetic<fp16,permitted>());
#if NATIVE_HOST_NEON
  static_assert(extra_arithmetic<fp16,neon_fp16|polyfill>());
  constexpr bool native_half_helpers() {
    constexpr auto A=neon_fp16|polyfill; using H=simd<fp16,8,A>;
    std::array<fp16,8> output{};
    auto value=floor(H(fp16(2.5f))); value+=fp16(1.f); value.store(output.data());
    for(auto lane:output) if(float(lane)!=3.f) return false;
    masked_mul_zero(predicate<8,A>::from_bitset(0x55),value,H(fp16(2.f))).store(output.data());
    for(std::size_t i=0;i<8;++i) if(float(output[i])!=((i&1)?0.f:6.f)) return false;
    return true;
  }
  static_assert(native_half_helpers());
#endif
  template<class V> concept elementwise_add=requires(V a) { a+a; };
  static_assert(!elementwise_add<simd<bf16,17,polyfill>>);
  constexpr bool storage_only_operations() {
#if NATIVE_HOST_NEON
    constexpr auto A=neon|polyfill;
#elif NATIVE_HOST_X86
    constexpr auto A=feature_closure(x86_feature::sse2)|polyfill;
#else
    return true;
#endif
#if NATIVE_HOST_NEON || NATIVE_HOST_X86
    using D=simd<double,2,A>;
    using I=simd<std::uint16_t,4,A>;
    using H=simd<fp16,4,A>;
    static_assert(std::same_as<D::native_type,simd<double,2,detail::hardware_isa<A>>::native_type>);
    static_assert(sizeof(D)==sizeof(simd<double,2,detail::hardware_isa<A>>));
    static_assert(!elementwise_add<simd<double,2,detail::hardware_isa<A>>>);
    static_assert(scalar_operand<I,std::int64_t> && !scalar_operand<I,float>);
    std::array<double,2> doubles{};
    auto incremented=D(2.); incremented+=3.;
    incremented.store(doubles.data());
    for(auto value:doubles) if(value!=5.) return false;
    std::array<std::uint64_t,2> double_bits{0x7ff8123456789abcull,0x8000000000000000ull},restored_bits{};
    D::load_bits(double_bits.data()).store_bits(restored_bits.data());
    if(double_bits!=restored_bits) return false;
    (D(2.)+D(3.)).store(doubles.data());
    for(auto value:doubles) if(value!=5.) return false;
    if((D(1.)<D(2.)).to_bitset()!=3) return false;
    std::array<std::uint16_t,4> integers{};
    (I(std::uint16_t{65530})+I(std::uint16_t{9})).store(integers.data());
    for(auto value:integers) if(value!=3) return false;
    auto wrap=I(std::uint16_t{65530}); wrap+=9;
    wrap.store(integers.data()); for(auto value:integers) if(value!=3) return false;
    std::array<fp16,4> halves{};
    sqrt(H(fp16(4.f))).store(halves.data());
    for(auto value:halves) if(float(value)!=2.f) return false;
    using B=simd<bf16,17,polyfill>; using W=simd<std::uint64_t,17,polyfill>;
    std::array<std::uint16_t,17> bfloat_bits{};
    convert<bf16>(W(0x8080000000000001ull)).store_bits(bfloat_bits.data());
    for(auto bits:bfloat_bits) if(bits!=0x5f01) return false;
    return true;
#endif
  }
  static_assert(storage_only_operations());
  bool half_environment() {
#if NATIVE_HOST_NEON
    std::uint64_t saved;
    __asm__ volatile("mrs %0, fpcr":"=r"(saved));
    auto write=[](std::uint64_t control) { __asm__ volatile("msr fpcr, %0"::"r"(control):"memory"); };
    constexpr std::uint64_t mode_mask=3ull<<22;
#elif NATIVE_HOST_X86
    std::uint32_t saved;
    __asm__ volatile("stmxcsr %0":"=m"(saved));
    auto write=[](std::uint32_t control) { __asm__ volatile("ldmxcsr %0"::"m"(control):"memory"); };
    constexpr std::uint32_t mode_mask=3u<<13;
#else
    return true;
#endif
#if NATIVE_HOST_NEON || NATIVE_HOST_X86
    using V=simd<fp16,17,polyfill>;
    std::array<std::uint16_t,17> one{},small{},output{}; one.fill(0x3c00); small.fill(0x1000);
    bool valid=true;
    for(unsigned mode=0;mode<4;++mode) {
#if NATIVE_HOST_NEON
      write((saved&~mode_mask)|(std::uint64_t(mode)<<22));
      auto expected=mode==1?0x3c01:0x3c00;
#else
      write((saved&~mode_mask)|(mode<<13));
      auto expected=mode==2?0x3c01:0x3c00;
#endif
      (V::load_bits(one.data())+V::load_bits(small.data())).store_bits(output.data());
      for(auto x:output) if(x!=expected) valid=false;
    }
#if NATIVE_HOST_NEON
    one.fill(1); small.fill(0);
    write(saved|(1ull<<19));
    (V::load_bits(one.data())+V::load_bits(small.data())).store_bits(output.data());
    for(auto x:output) if(x!=0) valid=false;
#endif
#if NATIVE_HOST_NEON
    using F=simd<float,17,polyfill>;
    std::array<std::uint32_t,17> nan_bits{},scaled_bits{};
    for(auto nan:std::array{0x7f812345u,0xffc12345u}) {
      nan_bits.fill(nan); write(saved|(1ull<<25));
      scaleb(F::load_bits(nan_bits.data()),F(0.f)).store_bits(scaled_bits.data());
      for(auto bits:scaled_bits) if(bits!=0x7fc00000u) valid=false;
    }
    // AH retains binary32 inputs under FZ; FIZ independently flushes them.
    nan_bits.fill(0x80000001u); write((saved|(1ull<<24)|2)&~((1ull<<25)|1));
    std::uint64_t observed; __asm__ volatile("mrs %0, fpcr":"=r"(observed)::"memory");
    scaleb(F(1.f),F::load_bits(nan_bits.data())).store_bits(scaled_bits.data());
    auto expected=(observed&2)?0x3f000000u:0x3f800000u;
    for(auto bits:scaled_bits) if(bits!=expected) valid=false;
    // Alternative handling leaves NaN representation signs untouched under FNEG.
    one.fill(0x7e12); write(saved|2);
    __asm__ volatile("mrs %0, fpcr":"=r"(observed)::"memory");
    (-V::load_bits(one.data())).store_bits(output.data());
    for(auto bits:output) if(bits!=((observed&2)?0x7e12:0xfe12)) valid=false;

#endif
    write(saved);
    return valid;
#endif
  }
#endif
  static_assert(custom<permitted>() && custom<isa<>(polyfill)>());
  static_assert(batching<decomposed>() && batching<scalar_emulated>());
  static_assert(normalized());
  static_assert(native::exp(simd<float,3,polyfill>(0.f)).template get<2>()==1.f);
  static_assert(native::sincos(simd<float,3,polyfill>(0.f)).first.template get<1>()==0.f);
  static_assert(helper_graph<permitted>() && helper_graph<isa<>(polyfill)>());
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
  bool extra_ok=extra_storage<double,isa<>(polyfill)>() && extra_arithmetic<double,isa<>(polyfill)>();
#if !NATIVE_POLYFILL_HEADERS
  extra_ok=extra_ok && extra_storage<fp16,isa<>(polyfill)>() && extra_storage<bf16,isa<>(polyfill)>() &&
    extra_arithmetic<fp16,isa<>(polyfill)>() && storage_only_operations() && half_environment();
#endif
  bool scalar_ok=extra_ok && math_graph<isa<>(polyfill)>(seed) && helper_graph<isa<>(polyfill)>(seed) && custom<isa<>(polyfill)>() && batching<scalar_emulated>() && normalized() &&
    floating<scalar_emulated>(float(seed)) && memory<scalar_emulated>(seed) &&
    unaligned_memory<scalar_emulated>(seed) && unaligned_memory<integer_emulated>(seed) &&
    representations<scalar_emulated>() && integers<isa<>(polyfill)>(seed) && integer_reductions<isa<>(polyfill)>(seed) &&
    packing<isa<>(polyfill)>() && masks<isa<>(polyfill)>();
#if NATIVE_POLYFILL_SCALAR_ONLY
  return !scalar_ok;
#else
  bool native_ok=math_graph<permitted>(seed) && helper_graph<permitted>(seed) && custom<permitted>() && batching<decomposed>() &&
    floating<decomposed>(float(seed)) && memory<simd<float,17,permitted>>(seed) &&
    unaligned_memory<simd<float,17,permitted>>(seed) && unaligned_memory<simd<std::uint32_t,17,permitted>>(seed) &&
    representations<decomposed>() && integers<permitted>(seed) && integer_reductions<permitted>(seed) &&
    packing<permitted>() && masks<permitted>();
  return !(scalar_ok && native_ok);
#endif
}
