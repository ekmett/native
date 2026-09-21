// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Repeated in matching test target scopes; the driver remains baseline-safe.
  template<class V> requires(V::architecture==NATIVE_FLOAT_ARCH) NATIVE_FLOAT_TARGET constexpr auto bits(V value) {
    words<V::lanes> result{};value.store_bits(result.data());return result;
  }
  template<class V> requires(V::architecture==NATIVE_FLOAT_ARCH) NATIVE_FLOAT_TARGET constexpr auto calculate(input<V::lanes> const& in) {
    auto a=V::load_bits(in.a.data()),b=V::load_bits(in.b.data()),c=V::load_bits(in.c.data());
    return output<V::lanes>{{bits(a+b),bits(a-b),bits(a*b),bits(a/b),bits(fma(a,b,c)),
      bits(sqrt(abs(a))),bits(floor(a)),bits(ceil(a)),bits(trunc(a)),bits(round_even(a)),
      bits(-a),bits(select(a<b,a,b))},
      {(a<b).to_bitset(),(a>b).to_bitset(),(a==b).to_bitset(),
       (a!=b).to_bitset(),(a<=b).to_bitset(),(a>=b).to_bitset()}};
  }

  template<class V> requires(V::architecture==NATIVE_FLOAT_ARCH) NATIVE_FLOAT_TARGET constexpr auto math_calculate(words<V::lanes> const& in) {
    auto a=V::load_bits(in.data());
    auto pair=::math::sincos(a);
    auto packed=::math::exp(std::array{a,a});
    return std::array{bits(native::exp(a)),bits(::math::exp(a)),bits(::math::sin(a)),bits(::math::cos(a)),
      bits(pair.first),bits(pair.second),bits(packed[1]),bits(::math::flush_to_zero(a)),
      bits(::math::abs(a)),bits(::math::sqrt(abs(a))),bits(::math::floor(a)),bits(::math::ceil(a)),
      bits(::math::trunc(a)),bits(::math::round_even(a))};
  }
