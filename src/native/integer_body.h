// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0


namespace native {
  /// Repartition the bits of an integer register without conversion or a spill.
  /// Both shapes need equal physical storage sizes supported by this backend's target.
  /// Storage-only element views are allowed; lane arithmetic is not required.
  template <simd_integer_element To, simd_integer_element From, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (sizeof(From) * N % sizeof(To) == 0 && requires {
      typename simd<From,N,Arch>::native_type;
      typename simd<To,sizeof(From)*N/sizeof(To),Arch>::native_type;
    } && sizeof(typename simd<From,N,Arch>::native_type) ==
      sizeof(typename simd<To,sizeof(From)*N/sizeof(To),Arch>::native_type) &&
      (sizeof(typename simd<From,N,Arch>::native_type) <= 16
#if NATIVE_HAS_AVX2
        || sizeof(typename simd<From,N,Arch>::native_type) == 32
#endif
#if NATIVE_HAS_AVX512F
        || sizeof(typename simd<From,N,Arch>::native_type) == 64
#endif
      ))
  native_nodiscard native_inline constexpr native_const auto reinterpret_bits(simd<From,N,Arch> value) noexcept
      -> simd<To,sizeof(From)*N/sizeof(To),Arch> {
    using result = simd<To,sizeof(From)*N/sizeof(To),Arch>;
    // Keep native vectors in this target scope: the standard-library wrapper
    // can otherwise impose its baseline vector return ABI on SysV hosts.
    return result::from_native(__builtin_bit_cast(typename result::native_type,value.to_native()));
  }

  /// Sum adjacent unsigned lanes into lanes twice as wide. No sum can overflow.
  /// The result contains half as many lanes; full-register inputs keep their width.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::is_unsigned_v<T> && sizeof(T) <= 4 && N > 1 && N % 2 == 0 &&
      detail::integer_arithmetic<T,N,Arch>)
  native_nodiscard native_inline constexpr native_const auto pairwise_add_widened(simd<T,N,Arch> value) noexcept {
    using U = std::conditional_t<sizeof(T)==1,std::uint16_t,
              std::conditional_t<sizeof(T)==2,std::uint32_t,std::uint64_t>>;
    using result = simd<U,N/2,Arch>;
    if consteval {
      std::array<T,N> source{};
      value.store(source.data());
      std::array<U,N/2> lanes{};
      for(std::size_t i=0;i<N/2;++i) lanes[i]=U(source[2*i])+source[2*i+1];
      return result(lanes);
    }
    if constexpr (sizeof(T)==4 && N==2) {
      // A logical two-lane input has four physical lanes. Only the live pair
      // contributes to the scalar result, regardless of padding bits.
      auto lanes = value.to_native();
      return result(std::uint64_t(lanes[0]) + lanes[1]);
    } else
#if NATIVE_HAS_ARM_NEON
    if constexpr (sizeof(T)==1)
      return result::from_native(vreinterpretq_u8_u16(vpaddlq_u8(value.to_native())));
    else if constexpr (sizeof(T)==2)
      return result::from_native(vreinterpretq_u8_u32(vpaddlq_u16(vreinterpretq_u16_u8(value.to_native()))));
    else return result::from_native(vreinterpretq_u8_u64(vpaddlq_u32(vreinterpretq_u32_u8(value.to_native()))));
#elif NATIVE_HAS_WASM_SIMD128
    if constexpr(sizeof(T)==1) return result::from_native(wasm_u16x8_extadd_pairwise_u8x16(value.to_native()));
    else if constexpr(sizeof(T)==2) return result::from_native(wasm_u32x4_extadd_pairwise_u16x8(value.to_native()));
    else {
      auto even=wasm_i32x4_shuffle(value.to_native(),value.to_native(),0,2,0,0);
      auto odd=wasm_i32x4_shuffle(value.to_native(),value.to_native(),1,3,0,0);
      return result::from_native(wasm_i64x2_add(wasm_u64x2_extend_low_u32x4(even),wasm_u64x2_extend_low_u32x4(odd)));
    }
#else
    // Two unsigned byte lanes sum to at most 510, well inside PMADDUBSW's
    // signed saturation bound. Wider lanes use exact register shifts/adds.
#if NATIVE_HAS_AVX2
    if constexpr (sizeof(T)==1 && sizeof(T)*N==16)
      return result::from_native(_mm_maddubs_epi16(value.to_native(),_mm_set1_epi8(1)));
    else if constexpr (sizeof(T)==1 && sizeof(T)*N==32)
      return result::from_native(_mm256_maddubs_epi16(value.to_native(),_mm256_set1_epi8(1)));
    else
#endif
#if NATIVE_HAS_AVX512BW
    if constexpr (sizeof(T)==1 && sizeof(T)*N==64)
      return result::from_native(_mm512_maddubs_epi16(value.to_native(),_mm512_set1_epi8(1)));
    else
#endif
    {
      auto words = reinterpret_bits<U>(value);
      return (words & result(U(std::numeric_limits<T>::max()))) + words.template right<8*sizeof(T)>();
    }
#endif
  }

  /// Count set bits independently in each unsigned integer lane.
  /// Byte populations use CNT on NEON and register nibble tables on x86.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::is_unsigned_v<T> && detail::integer_arithmetic<T,N,Arch>)
  native_nodiscard native_inline constexpr native_const simd<T,N,Arch> popcount(simd<T,N,Arch> value) noexcept {
    using result = simd<T,N,Arch>;
    if consteval {
      std::array<T,N> lanes{};
      value.store(lanes.data());
      for(auto &lane:lanes) lane=T(std::popcount(lane));
      return result(lanes);
    }
    if constexpr (N==1) return result(T(std::popcount(value.to_native())));
    else if constexpr (sizeof(T)==4 && (N==2 || N==3))
      return result::from_storage(popcount(value.to_storage()));
#if NATIVE_HAS_AVX512F && !NATIVE_HAS_AVX512BW
    else if constexpr (sizeof(T)*N==64) {
      // F/DQ supports arithmetic on these lanes, while its byte/word
      // repartitions provide storage only.
      // Accumulate bit populations inside each original lane instead.
      constexpr T all = std::numeric_limits<T>::max();
      value = value - (value.template right<1>() & result(T(all/3)));
      value = (value & result(T(all/5))) + (value.template right<2>() & result(T(all/5)));
      value = (value + value.template right<4>()) & result(T(all/17));
      value = value + value.template right<8>();
      value = value + value.template right<16>();
      if constexpr (sizeof(T)==8) value = value + value.template right<32>();
      return value & result(T(127));
    }
#endif
    else if constexpr (sizeof(T)>1)
      return pairwise_add_widened(popcount(reinterpret_bits<
        std::conditional_t<sizeof(T)==2,std::uint8_t,
          std::conditional_t<sizeof(T)==4,std::uint16_t,std::uint32_t>>>(value)));
    else {
#if NATIVE_HAS_WASM_SIMD128
      return result::from_native(wasm_i8x16_popcnt(value.to_native()));
#endif
#if NATIVE_HAS_ARM_NEON
      return result::from_native(vcntq_u8(value.to_native()));
#endif
#if NATIVE_HAS_AVX2
      if constexpr (N==16) {
        auto table = _mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4);
        auto mask = _mm_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm_add_epi8(
          _mm_shuffle_epi8(table,_mm_and_si128(data,mask)),
          _mm_shuffle_epi8(table,_mm_and_si128(_mm_srli_epi16(data,4),mask))));
      } else if constexpr (N==32) {
        auto table = _mm256_broadcastsi128_si256(_mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4));
        auto mask = _mm256_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm256_add_epi8(
          _mm256_shuffle_epi8(table,_mm256_and_si256(data,mask)),
          _mm256_shuffle_epi8(table,_mm256_and_si256(_mm256_srli_epi16(data,4),mask))));
      }
#endif
#if NATIVE_HAS_AVX512BW
      if constexpr (N==64) {
        auto table = _mm512_broadcast_i32x4(_mm_setr_epi8(0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4));
        auto mask = _mm512_set1_epi8(15);
        auto data = value.to_native();
        return result::from_native(_mm512_add_epi8(
          _mm512_shuffle_epi8(table,_mm512_and_si512(data,mask)),
          _mm512_shuffle_epi8(table,_mm512_and_si512(_mm512_srli_epi16(data,4),mask))));
      }
#endif
    }
  }

  /// Sum unsigned 8-, 16-, or 32-bit lanes exactly into a 64-bit scalar.
  /// Reduction widens before adding: it never wraps at the input lane width.
  template <simd_integer_element T, std::size_t N, ::native::isa<> Arch>
    requires NATIVE_ARCH_REQUIRES(Arch) && (std::is_unsigned_v<T> && sizeof(T)<=4 &&
      detail::integer_arithmetic<T,N,Arch>)
  native_nodiscard native_inline constexpr native_const std::uint64_t reduce_add_widened(simd<T,N,Arch> value) noexcept {
    if consteval {
      std::array<T,N> lanes{};
      value.store(lanes.data());
      std::uint64_t sum=0;
      for(auto lane:lanes) sum+=lane;
      return sum;
    }
    if constexpr (N==1) return value.to_native();
    else if constexpr (sizeof(T)==4 && (N==2 || N==3)) {
      auto lanes = value.to_native();
      auto sum = std::uint64_t(lanes[0]) + lanes[1];
      if constexpr (N==3) sum += lanes[2];
      return sum;
    }
    else {
#if NATIVE_HAS_WASM_SIMD128
      auto sums=[&] {
        if constexpr(sizeof(T)==1) return pairwise_add_widened(pairwise_add_widened(pairwise_add_widened(value)));
        else if constexpr(sizeof(T)==2) return pairwise_add_widened(pairwise_add_widened(value));
        else return pairwise_add_widened(value);
      }();
      return sums.template get<0>()+sums.template get<1>();
#endif
#if NATIVE_HAS_ARM_NEON
      if constexpr (sizeof(T)==1) return vaddlvq_u8(value.to_native());
      else if constexpr (sizeof(T)==2) return vaddlvq_u16(vreinterpretq_u16_u8(value.to_native()));
      else return vaddlvq_u32(vreinterpretq_u32_u8(value.to_native()));
#endif
#if NATIVE_HAS_AVX2
      auto sums = [&] {
        if constexpr (sizeof(T)==1) {
          if constexpr (N==16) return _mm_sad_epu8(value.to_native(),_mm_setzero_si128());
          else if constexpr (N==32) return _mm256_sad_epu8(value.to_native(),_mm256_setzero_si256());
#if NATIVE_HAS_AVX512BW
          else return _mm512_sad_epu8(value.to_native(),_mm512_setzero_si512());
#endif
        } else if constexpr (sizeof(T)==2)
          return pairwise_add_widened(pairwise_add_widened(value)).to_native();
        else return pairwise_add_widened(value).to_native();
      }();
      if constexpr (sizeof(T)*N==16)
        return std::uint64_t(_mm_cvtsi128_si64(_mm_add_epi64(sums,_mm_srli_si128(sums,8))));
      else if constexpr (sizeof(T)*N==32) {
        auto halves = _mm_add_epi64(_mm256_castsi256_si128(sums),_mm256_extracti128_si256(sums,1));
        return std::uint64_t(_mm_cvtsi128_si64(_mm_add_epi64(halves,_mm_srli_si128(halves,8))));
      }
#if NATIVE_HAS_AVX512F
      else return std::uint64_t(_mm512_reduce_add_epi64(sums));
#endif
#endif
    }
  }
}
