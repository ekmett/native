// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated under honest backend targets; baseline permission acquires no SIMD target.
#if NATIVE_HOST_WASM
namespace native::detail::NATIVE_BACKEND {
  template<class V> constexpr auto wasm_polyfill_lanes(V value) noexcept {
    std::array<typename V::value_type,V::lanes> lanes{}; value.store(lanes.data()); return lanes;
  }
  template<class V,class Native,class Scalar,class... W>
  constexpr V wasm_polyfill_map(Native native_operation,Scalar scalar_operation,V value,W... other) noexcept {
    if constexpr(V::architecture.has(wasm_feature::simd128) && requires { typename V::chunk_type; requires V::register_lanes*sizeof(typename V::value_type)==16; }) {
      auto inputs=std::tuple{value.to_native(),other.to_native()...}; typename V::native_type result{};
      for(std::size_t i=0;i<V::register_count;++i) std::apply([&](auto const &... parts) {
        result[i]=native_operation(V::chunk_type::from_native(parts[i])...).to_native();
      },inputs);
      return V::from_native(result);
    } else {
      auto inputs=std::tuple{wasm_polyfill_lanes(value),wasm_polyfill_lanes(other)...};
      std::array<typename V::value_type,V::lanes> result{};
      for(std::size_t i=0;i<V::lanes;++i) std::apply([&](auto const &... lanes) { result[i]=scalar_operation(lanes[i]...); },inputs);
      return V::load(result.data());
    }
  }
  template<class T> constexpr T wasm_polyfill_saturate(std::int64_t value) noexcept {
    return T(std::clamp(value,std::int64_t(std::numeric_limits<T>::min()),std::int64_t(std::numeric_limits<T>::max())));
  }
  template<class T> using wasm_polyfill_wide_unsigned=std::conditional_t<sizeof(T)==1,std::uint16_t,
    std::conditional_t<sizeof(T)==2,std::uint32_t,std::uint64_t>>;
  template<class T> using wasm_polyfill_wide=std::conditional_t<std::is_signed_v<T>,
    std::make_signed_t<wasm_polyfill_wide_unsigned<T>>,wasm_polyfill_wide_unsigned<T>>;
  template<bool High,class T,std::size_t N,isa<> A>
  constexpr auto wasm_polyfill_extend(simd<T,N,A> value) noexcept {
    using W=wasm_polyfill_wide<T>; using R=simd<W,N/2,A>;
    auto source=wasm_polyfill_lanes(value); std::array<W,N/2> output{};
    if constexpr(A.has(wasm_feature::simd128)) {
      using C=simd<T,16/sizeof(T),hardware_isa<A>>; constexpr auto count=C::lanes/2;
      for(std::size_t i=0;i<N/2;i+=count) {
        std::array<T,C::lanes> part{};
        for(std::size_t j=0;j<std::min(count,N/2-i);++j) part[j]=source[i+j+(High?N/2:0)];
        std::array<W,count> widened{}; extend_low(C::load(part.data())).store(widened.data());
        for(std::size_t j=0;j<std::min(count,N/2-i);++j) output[i+j]=widened[j];
      }
    } else for(std::size_t i=0;i<N/2;++i) output[i]=W(source[i+(High?N/2:0)]);
    return R::load(output.data());
  }
  template<class T> constexpr T wasm_polyfill_read(T const * p) noexcept {
    if consteval { return *p; }
    else { T result; std::memcpy(&result,reinterpret_cast<unsigned char const *>(p),sizeof(T)); return result; }
  }
}
namespace native {
  /// Shuffle logical lanes from a followed by b, in the requested compile-time order.
  template<std::size_t... I,class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (sizeof...(I)==N && ((I<2*N) && ...))
  constexpr simd<T,N,A> shuffle(simd<T,N,A> a,simd<T,N,A> b) noexcept {
    auto x=detail::NATIVE_BACKEND::wasm_polyfill_lanes(a),y=detail::NATIVE_BACKEND::wasm_polyfill_lanes(b);
    std::array<T,N> output{(I<N?x[I]:y[I-N])...}; return simd<T,N,A>::load(output.data());
  }
  /// Select bytes by indices 0..15; all larger indices produce zero.
  template<isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<std::uint8_t,16,A>
  constexpr simd<std::uint8_t,16,A> swizzle(simd<std::uint8_t,16,A> value,simd<std::uint8_t,16,A> indices) noexcept {
    auto x=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value),index=detail::NATIVE_BACKEND::wasm_polyfill_lanes(indices);
    std::array<std::uint8_t,16> output{};
    for(std::size_t i=0;i<16;++i) output[i]=index[i]<16?x[index[i]]:0;
    return simd<std::uint8_t,16,A>::load(output.data());
  }
#define NATIVE_WASM_POLYFILL_SAT(NAME,OP) \
  /** Saturate byte and halfword lane arithmetic at the element limits. */ \
  template<simd_integer_element T,std::size_t N,isa<> A> \
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (sizeof(T)<=2) \
  constexpr simd<T,N,A> NAME(simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return NAME(x,y); }, \
      [](T x,T y) { return detail::NATIVE_BACKEND::wasm_polyfill_saturate<T>(std::int64_t(x) OP std::int64_t(y)); },a,b); \
  }
  NATIVE_WASM_POLYFILL_SAT(add_sat,+)
  NATIVE_WASM_POLYFILL_SAT(sub_sat,-)
#undef NATIVE_WASM_POLYFILL_SAT
  /// Compute the unsigned lane average rounded upward.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (sizeof(T)<=2 && std::is_unsigned_v<T>)
  constexpr simd<T,N,A> average_round(simd<T,N,A> a,simd<T,N,A> b) noexcept {
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return average_round(x,y); },
      [](T x,T y) { return T((unsigned(x)+unsigned(y)+1)/2); },a,b);
  }
  /// Negate negative signed integer lanes, retaining the minimum value's representation.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && std::is_signed_v<T>
  constexpr simd<T,N,A> abs(simd<T,N,A> value) noexcept {
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x) { return abs(x); },[](T x) {
      using U=std::make_unsigned_t<T>; return x<0?std::bit_cast<T>(U(U(0)-U(x))):x;
    },value);
  }
#define NATIVE_WASM_POLYFILL_INTEGER_MINMAX(NAME,OP) \
  /** Choose the corresponding integer lane using its signedness. */ \
  template<simd_integer_element T,std::size_t N,isa<> A> \
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> \
  constexpr simd<T,N,A> NAME(simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return NAME(x,y); },[](T x,T y) { return x OP y?x:y; },a,b); \
  }
  NATIVE_WASM_POLYFILL_INTEGER_MINMAX(min,<)
  NATIVE_WASM_POLYFILL_INTEGER_MINMAX(max,>)
#undef NATIVE_WASM_POLYFILL_INTEGER_MINMAX
#define NATIVE_WASM_POLYFILL_FLOAT_MINMAX(NAME,MAXIMUM) \
  /** Propagate floating NaNs and choose signed zeros using WebAssembly rules. */ \
  template<std::floating_point T,std::size_t N,isa<> A> \
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> \
  constexpr simd<T,N,A> NAME(simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return NAME(x,y); }, \
      [](T x,T y) { return detail::polyfill_wasm_minmax<MAXIMUM>(x,y); },a,b); \
  }
  NATIVE_WASM_POLYFILL_FLOAT_MINMAX(min,false)
  NATIVE_WASM_POLYFILL_FLOAT_MINMAX(max,true)
#undef NATIVE_WASM_POLYFILL_FLOAT_MINMAX
#define NATIVE_WASM_POLYFILL_PMINMAX(NAME,OP) \
  /** Choose the first operand on equality or unordered comparison. */ \
  template<std::floating_point T,std::size_t N,isa<> A> \
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> \
  constexpr simd<T,N,A> NAME(simd<T,N,A> a,simd<T,N,A> b) noexcept { \
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return NAME(x,y); },[](T x,T y) { return y OP x?y:x; },a,b); \
  }
  NATIVE_WASM_POLYFILL_PMINMAX(pmin,<)
  NATIVE_WASM_POLYFILL_PMINMAX(pmax,>)
#undef NATIVE_WASM_POLYFILL_PMINMAX
  /// Widen the lower logical half, preserving source signedness.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (N%2==0 && sizeof(T)<=4)
  constexpr auto extend_low(simd<T,N,A> value) noexcept { return detail::NATIVE_BACKEND::wasm_polyfill_extend<false>(value); }
  /// Widen the upper logical half, preserving source signedness.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (N%2==0 && sizeof(T)<=4)
  constexpr auto extend_high(simd<T,N,A> value) noexcept { return detail::NATIVE_BACKEND::wasm_polyfill_extend<true>(value); }
  /// Multiply the widened lower logical half without losing product bits.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (N%2==0 && sizeof(T)<=4)
  constexpr auto multiply_widened_low(simd<T,N,A> a,simd<T,N,A> b) noexcept { return extend_low(a)*extend_low(b); }
  /// Multiply the widened upper logical half without losing product bits.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (N%2==0 && sizeof(T)<=4)
  constexpr auto multiply_widened_high(simd<T,N,A> a,simd<T,N,A> b) noexcept { return extend_high(a)*extend_high(b); }
  /// Concatenate signed source lanes and saturate at destination lane limits.
  template<simd_integer_element To,simd_integer_element From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<From,N,A> &&
      (sizeof(From)==2*sizeof(To) && sizeof(To)<=2 && std::is_signed_v<From> && N<=32)
  constexpr simd<To,2*N,A> narrow_sat(simd<From,N,A> a,simd<From,N,A> b) noexcept {
    auto x=detail::NATIVE_BACKEND::wasm_polyfill_lanes(a),y=detail::NATIVE_BACKEND::wasm_polyfill_lanes(b);
    std::array<To,2*N> output{};
    for(std::size_t i=0;i<N;++i) { output[i]=detail::NATIVE_BACKEND::wasm_polyfill_saturate<To>(x[i]); output[N+i]=detail::NATIVE_BACKEND::wasm_polyfill_saturate<To>(y[i]); }
    return simd<To,2*N,A>::load(output.data());
  }
  /// Multiply signed Q15 lanes, round upward at half units, and saturate.
  template<isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<std::int16_t,8,A>
  constexpr simd<std::int16_t,8,A> q15mulr_sat(simd<std::int16_t,8,A> a,simd<std::int16_t,8,A> b) noexcept {
    return detail::NATIVE_BACKEND::wasm_polyfill_map([](auto x,auto y) { return q15mulr_sat(x,y); },[](std::int16_t x,std::int16_t y) {
      return detail::NATIVE_BACKEND::wasm_polyfill_saturate<std::int16_t>((std::int64_t(x)*y+16384)>>15);
    },a,b);
  }
  /// Add adjacent signed halfword products modulo 2^32.
  template<isa<> A> requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<std::int16_t,8,A>
  constexpr simd<std::int32_t,4,A> dot(simd<std::int16_t,8,A> a,simd<std::int16_t,8,A> b) noexcept {
    auto x=detail::NATIVE_BACKEND::wasm_polyfill_lanes(a),y=detail::NATIVE_BACKEND::wasm_polyfill_lanes(b);
    std::array<std::int32_t,4> output{};
    for(std::size_t i=0;i<4;++i) output[i]=std::bit_cast<std::int32_t>(std::uint32_t(std::int64_t(x[2*i])*y[2*i]+std::int64_t(x[2*i+1])*y[2*i+1]));
    return simd<std::int32_t,4,A>::load(output.data());
  }
  /// Saturating conversion to four 32-bit lanes; NaNs and unused upper lanes become zero.
  template<simd_integer_element To,std::floating_point From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<From,N,A> && (sizeof(From)*N==16 && sizeof(To)==4)
  constexpr simd<To,4,A> trunc_sat(simd<From,N,A> value) noexcept {
    auto input=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value); std::array<To,4> output{};
    for(std::size_t i=0;i<N;++i) {
      auto x=input[i];
      if(x!=x) output[i]=0;
      else if(x<=double(std::numeric_limits<To>::min())) output[i]=std::numeric_limits<To>::min();
      else if(x>=double(std::numeric_limits<To>::max())) output[i]=std::numeric_limits<To>::max();
      else output[i]=To(x);
    }
    return simd<To,4,A>::load(output.data());
  }
  /// Convert one logical SIMD128 shape; binary64 consumes low lanes, binary32 demotion zeroes high lanes.
  template<std::floating_point To,detail::polyfill_wasm_number From,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<From,N,A> &&
      (sizeof(From)*N==16 && (sizeof(From)==4 || std::floating_point<From>) && !std::same_as<To,From>)
  constexpr simd<To,16/sizeof(To),A> convert(simd<From,N,A> value) noexcept {
    auto input=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value); std::array<To,16/sizeof(To)> output{};
    for(std::size_t i=0;i<std::min(N,output.size());++i) {
      if consteval {
        if constexpr(std::floating_point<From>) {
          using F=typename detail::polyfill_element_traits<From>::format; using G=typename detail::polyfill_element_traits<To>::format;
          output[i]=std::bit_cast<To>(detail::constexpr_float::convert_bits<G,F>(std::bit_cast<typename F::bits_type>(input[i])));
        } else output[i]=static_cast<To>(input[i]);
      } else { output[i]=static_cast<To>(input[i]); }
    }
    return simd<To,16/sizeof(To),A>::load(output.data());
  }
  /// Read exactly one unaligned element and broadcast it to the logical lanes.
  template<class V> requires NATIVE_ARCH_REQUIRES(V::architecture) && detail::polyfill_wasm_shape<typename V::value_type,V::lanes,V::architecture>
  constexpr V load_splat(typename V::value_type const * p) noexcept { return V(detail::NATIVE_BACKEND::wasm_polyfill_read(p)); }
  /// Read exactly one unaligned element into lane I, retaining the others.
  template<std::size_t I,class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (I<N)
  constexpr simd<T,N,A> load_lane(T const * p,simd<T,N,A> value) noexcept {
    auto lanes=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value); lanes[I]=detail::NATIVE_BACKEND::wasm_polyfill_read(p);
    return simd<T,N,A>::load(lanes.data());
  }
  /// Write exactly one unaligned logical element from lane I.
  template<std::size_t I,class T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (I<N)
  constexpr void store_lane(T * p,simd<T,N,A> value) noexcept {
    auto lanes=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value);
    if consteval { *p=lanes[I]; }
    else { std::memcpy(reinterpret_cast<unsigned char *>(p),lanes.data()+I,sizeof(T)); }
  }
  /// Read one unaligned 32- or 64-bit element and zero the remaining logical lanes.
  template<class V> requires NATIVE_ARCH_REQUIRES(V::architecture) &&
    detail::polyfill_wasm_shape<typename V::value_type,V::lanes,V::architecture> && (sizeof(typename V::value_type)>=4)
  constexpr V load_zero(typename V::value_type const * p) noexcept {
    std::array<typename V::value_type,V::lanes> output{}; output[0]=detail::NATIVE_BACKEND::wasm_polyfill_read(p); return V::load(output.data());
  }
  /// Read exactly eight unaligned source bytes and widen lanes with their signedness.
  template<simd_integer_element To,simd_integer_element From,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && !A.has(wasm_feature::simd128)) &&
      (sizeof(To)==2*sizeof(From) && sizeof(To)<=8 && std::is_signed_v<To> ==std::is_signed_v<From>)
  constexpr simd<To,16/sizeof(To),A> load_widened(From const * p) noexcept {
    std::array<From,8/sizeof(From)> input{};
    if consteval { for(std::size_t i=0;i<input.size();++i) input[i]=p[i]; }
    else { std::memcpy(input.data(),reinterpret_cast<unsigned char const *>(p),8); }
    std::array<To,input.size()> output{}; for(std::size_t i=0;i<input.size();++i) output[i]=To(input[i]);
    return simd<To,output.size(),A>::load(output.data());
  }
  /// Widen adjacent signed byte or halfword sums in logical lane order.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A> && (std::is_signed_v<T> && sizeof(T)<=2 && N%2==0)
  constexpr auto pairwise_add_widened(simd<T,N,A> value) noexcept {
    using W=detail::NATIVE_BACKEND::wasm_polyfill_wide<T>;
    auto input=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value); std::array<W,N/2> output{};
    for(std::size_t i=0;i<N/2;++i) output[i]=W(input[2*i])+W(input[2*i+1]);
    return simd<W,N/2,A>::load(output.data());
  }
  /// Gather each integer lane's most significant bit in logical order.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A>
  constexpr auto bitmask(simd<T,N,A> value) noexcept {
    using R=std::conditional_t<(N<=32),std::uint32_t,std::uint64_t>;
    auto input=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value); R bits{};
    for(std::size_t i=0;i<N;++i) bits|=R((std::make_unsigned_t<T>(input[i])>>(sizeof(T)*8-1))&1)<<i;
    return bits;
  }
  /// Test whether any logical integer lane is nonzero.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A>
  constexpr bool any(simd<T,N,A> value) noexcept {
    for(auto lane:detail::NATIVE_BACKEND::wasm_polyfill_lanes(value)) if(lane!=0) return true; return false;
  }
  /// Test whether all logical integer lanes are nonzero.
  template<simd_integer_element T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && detail::polyfill_wasm_shape<T,N,A>
  constexpr bool all(simd<T,N,A> value) noexcept {
    for(auto lane:detail::NATIVE_BACKEND::wasm_polyfill_lanes(value)) if(lane==0) return false; return true;
  }
  /// Construct 2^n for exact integral binary32 exponents in the normal range.
  template<isa<> A> requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && A.has(wasm_feature::simd128))
  constexpr simd<float,4,A> normal_pow2(simd<float,4,A> value) noexcept {
    auto lanes=detail::NATIVE_BACKEND::wasm_polyfill_lanes(value);
    for(auto & lane:lanes) lane=std::bit_cast<float>(std::uint32_t(std::int32_t(lane)+127)<<23);
    return simd<float,4,A>::load(lanes.data());
  }
  /// Supply fused binary32/binary64 arithmetic only when explicit permission requests it.
  template<std::floating_point T,std::size_t N,isa<> A>
    requires NATIVE_ARCH_REQUIRES(A) && (A.has(polyfill) && A.has(wasm_feature::simd128) && sizeof(T)*N==16)
  constexpr simd<T,N,A> fma(simd<T,N,A> a,simd<T,N,A> b,simd<T,N,A> c) noexcept {
    auto x=detail::NATIVE_BACKEND::wasm_polyfill_lanes(a),y=detail::NATIVE_BACKEND::wasm_polyfill_lanes(b),z=detail::NATIVE_BACKEND::wasm_polyfill_lanes(c);
    using F=typename detail::polyfill_element_traits<T>::format; using U=typename F::bits_type;
    for(std::size_t i=0;i<N;++i) {
      if consteval { x[i]=std::bit_cast<T>(detail::constexpr_float::fma_bits<F>(std::bit_cast<U>(x[i]),std::bit_cast<U>(y[i]),std::bit_cast<U>(z[i]))); }
      else { x[i]=std::fma(x[i],y[i],z[i]); }
    }
    return simd<T,N,A>::load(x.data());
  }
}
#endif
