// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
inline constexpr auto cd512 = native::target_features<native::x86>("avx512f,avx512cd");
inline constexpr auto cdvl = native::target_features<native::x86>("avx512f,avx512cd,avx512vl");

template<native::isa<native::x86> A, class T, std::size_t N>
concept cd_shape = requires(native::simd<T,N,A> v, native::predicate<N,A> m) {
  { native::vpconflictd<A>(v) } noexcept -> std::same_as<decltype(v)>;
  { native::mask_vpconflictd<A>(v,m,v) } noexcept -> std::same_as<decltype(v)>;
  { native::maskz_vpconflictd<A>(m,v) } noexcept -> std::same_as<decltype(v)>;
  { native::vplzcntd<A>(v) } noexcept -> std::same_as<decltype(v)>;
};
static_assert(cd_shape<cdvl,std::uint32_t,4> && cd_shape<cdvl,std::uint32_t,8> && cd_shape<cd512,std::uint32_t,16>);
static_assert(!cd_shape<cdvl,std::int32_t,4> && !cd_shape<cdvl,std::uint32_t,2>);
static_assert(!cd_shape<native::isa<native::x86>{},std::uint32_t,4>);
template<class V> concept cd_raw = requires(V v) { native::vpconflictd<cdvl>(v); };
static_assert(!cd_raw<__m128i> && !cd_raw<__m256i> && !cd_raw<__m512i>);
template<class M> concept cd_mask = requires(native::simd<std::uint32_t,4,cdvl> v, M m) {
  native::mask_vpconflictd<cdvl>(v,m,v);
};
static_assert(!cd_mask<unsigned> && !cd_mask<native::predicate<8,cdvl>> && !cd_mask<native::predicate<4,cd512>>);

template<class T, std::size_t N>
constexpr std::array<std::array<T,N>,6> reference(
    std::array<T,N> const& input, std::array<T,N> const& source, std::uint64_t mask) {
  std::array<std::array<T,N>,6> result{};
  for(std::size_t lane=0;lane<N;++lane) {
    T conflicts=0, zeros=0;
    // Earlier lanes are considered even when their destination mask bit is zero.
    for(std::size_t earlier=0;earlier<lane;++earlier)
      if(input[earlier]==input[lane]) conflicts+=T{1}<<earlier;
    for(unsigned bit=sizeof(T)*8;bit && !((input[lane]>>(bit-1))&T{1});--bit) ++zeros;
    bool active=(mask>>lane)&1;
    result[0][lane]=conflicts;
    result[1][lane]=active ? conflicts : source[lane];
    result[2][lane]=active ? conflicts : 0;
    result[3][lane]=zeros;
    result[4][lane]=active ? zeros : source[lane];
    result[5][lane]=active ? zeros : 0;
  }
  return result;
}

// Keep both the native runtime calls and constant evaluation on the same public API.
template<native::isa<native::x86> A, class T, std::size_t N>
consteval auto calculate(std::array<T,N> input, std::array<T,N> source, std::uint64_t mask) {
  using V=native::simd<T,N,A>;
  auto v=V::load(input.data()), s=V::load(source.data());
  auto m=native::predicate<N,A>::from_bitset(mask);
  std::array<std::array<T,N>,6> result{};
  if constexpr(sizeof(T)==4) {
    native::vpconflictd<A>(v).store(result[0].data());
    native::mask_vpconflictd<A>(s,m,v).store(result[1].data());
    native::maskz_vpconflictd<A>(m,v).store(result[2].data());
    native::vplzcntd<A>(v).store(result[3].data());
    native::mask_vplzcntd<A>(s,m,v).store(result[4].data());
    native::maskz_vplzcntd<A>(m,v).store(result[5].data());
  } else {
    native::vpconflictq<A>(v).store(result[0].data());
    native::mask_vpconflictq<A>(s,m,v).store(result[1].data());
    native::maskz_vpconflictq<A>(m,v).store(result[2].data());
    native::vplzcntq<A>(v).store(result[3].data());
    native::mask_vplzcntq<A>(s,m,v).store(result[4].data());
    native::maskz_vplzcntq<A>(m,v).store(result[5].data());
  }
  return result;
}
template<native::isa<native::x86> A, class T, std::size_t N>
consteval bool constants() {
  std::array<T,N> input{},source{};
  for(std::size_t lane=0;lane<N;++lane) source[lane]=T(99+lane);
  for(unsigned pattern=0;pattern<sizeof(T)*8+4;++pattern) {
    for(std::size_t lane=0;lane<N;++lane)
      input[lane]=pattern==0 ? 0 : pattern==1 ? T(lane) : pattern==2 ? T(lane%3) :
        pattern==3 ? ~T{0} : T{1}<<((pattern-4+lane)%(sizeof(T)*8));
    for(auto mask : {std::uint64_t{0},~std::uint64_t{0},std::uint64_t{0xaaaa},std::uint64_t{1}<<(N-1)})
      if(calculate<A>(input,source,mask)!=reference(input,source,mask)) return false;
  }
  return true;
}
#define CHECK_CD(T,N,Storage) \
  static_assert(constants<native::target_features<native::x86>(Storage),T,N>()); \
  static_assert(constants<cdvl,T,N>())
CHECK_CD(std::uint32_t,4,"sse2");
CHECK_CD(std::uint32_t,8,"avx");
CHECK_CD(std::uint32_t,16,"avx512f");
CHECK_CD(std::uint64_t,2,"sse2");
CHECK_CD(std::uint64_t,4,"avx");
CHECK_CD(std::uint64_t,8,"avx512f");
#undef CHECK_CD
static_assert(constants<cd512,std::uint32_t,16>());
static_assert(constants<cd512,std::uint64_t,8>());

#define CD_RUNTIME(Name,Arch,Features) \
  template<class T,std::size_t N> native_noinline native_target(Features) \
  void Name(T* result,T const* input,T const* source,std::uint64_t mask) noexcept { \
    using V=native::simd<T,N,Arch>; \
    auto v=V::load(input),s=V::load(source); \
    auto m=native::predicate<N,Arch>::from_bitset(mask); \
    if constexpr(sizeof(T)==4) { \
      native::vpconflictd<Arch>(v).store(result); \
      native::mask_vpconflictd<Arch>(s,m,v).store(result+N); \
      native::maskz_vpconflictd<Arch>(m,v).store(result+2*N); \
      native::vplzcntd<Arch>(v).store(result+3*N); \
      native::mask_vplzcntd<Arch>(s,m,v).store(result+4*N); \
      native::maskz_vplzcntd<Arch>(m,v).store(result+5*N); \
    } else { \
      native::vpconflictq<Arch>(v).store(result); \
      native::mask_vpconflictq<Arch>(s,m,v).store(result+N); \
      native::maskz_vpconflictq<Arch>(m,v).store(result+2*N); \
      native::vplzcntq<Arch>(v).store(result+3*N); \
      native::mask_vplzcntq<Arch>(s,m,v).store(result+4*N); \
      native::maskz_vplzcntq<Arch>(m,v).store(result+5*N); \
    } \
  }
CD_RUNTIME(invoke512,cd512,"avx512f,avx512cd")
CD_RUNTIME(invokevl,cdvl,"avx512f,avx512cd,avx512vl")
#undef CD_RUNTIME

inline std::uint64_t random_bits(std::uint64_t& state) {
  state^=state<<13; state^=state>>7; state^=state<<17; return state;
}
template<class T,std::size_t N> bool check_case(std::array<T,N> const& input,
    std::array<T,N> const& source,std::uint64_t mask) {
  std::array<T,6*N> actual{};
  if constexpr(sizeof(T)*N==64) invoke512<T,N>(actual.data(),input.data(),source.data(),mask);
  else invokevl<T,N>(actual.data(),input.data(),source.data(),mask);
  auto expected=reference(input,source,mask);
  for(std::size_t form=0;form<6;++form) for(std::size_t lane=0;lane<N;++lane)
    if(actual[form*N+lane]!=expected[form][lane]) {
      std::printf("AVX512CD mismatch width=%zu form=%zu lane=%zu mask=%llx\n",sizeof(T)*N*8,form,lane,
        static_cast<unsigned long long>(mask)); return false;
    }
  return true;
}
template<class T,std::size_t N> bool check_vectors(std::uint64_t& state) {
  std::array<T,N> input{},source{};
  for(auto& lane:source) lane=T(random_bits(state));
  for(unsigned pattern=0;pattern<sizeof(T)*8+4;++pattern) {
    for(std::size_t lane=0;lane<N;++lane)
      input[lane]=pattern==0 ? 0 : pattern==1 ? T(lane) : pattern==2 ? T(lane%3) :
        pattern==3 ? ~T{0} : T{1}<<((pattern-4+lane)%(sizeof(T)*8));
    if(!check_case(input,source,0) || !check_case(input,source,~std::uint64_t{0}) ||
       !check_case(input,source,0xaaaa) || !check_case(input,source,0x5555)) return false;
    for(unsigned bit=0;bit<16;++bit)
      if(!check_case(input,source,std::uint64_t{1}<<bit) ||
         !check_case(input,source,~(std::uint64_t{1}<<bit))) return false;
  }
  // Small-domain inputs force duplicates across every 128-bit boundary.
  for(unsigned trial=0;trial<2048;++trial) {
    for(auto& lane:input) lane=T(random_bits(state)&(trial&1 ? 7 : ~std::uint64_t{0}));
    for(auto& lane:source) lane=T(random_bits(state));
    if(!check_case(input,source,random_bits(state))) return false;
  }
  return true;
}
