// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <type_traits>
import native.arm;
using namespace native;
namespace {
  constexpr auto software=isa<arm>{}|polyfill;
  template<class T,std::size_t N,auto A>
  constexpr simd<T,N,A> sample(unsigned seed) noexcept {
    if constexpr(std::same_as<T,fp16> || std::same_as<T,bf16>) {
      std::array<std::uint16_t,N> bits{};
      for(std::size_t i=0;i<N;++i)
        bits[i]=(std::same_as<T,fp16>?0x3c00:0x3f80)+((seed+i)%7)*16;
      return simd<T,N,A>::load_bits(bits.data());
    } else {
      std::array<T,N> lanes{};
      for(std::size_t i=0;i<N;++i) lanes[i]=static_cast<T>(seed+3*i+1);
      return simd<T,N,A>::load(lanes.data());
    }
  }
  template<class T,std::size_t N,auto A,auto B>
  bool equal(simd<T,N,A> a,simd<T,N,B> b) noexcept {
    std::array<T,N> av{},bv{};a.store(av.data());b.store(bv.data());
    return std::bit_cast<std::array<std::uint8_t,sizeof(T)*N>>(av)==
      std::bit_cast<std::array<std::uint8_t,sizeof(T)*N>>(bv);
  }
  __attribute__((target("dotprod"), noinline))
  bool check_dotprod(unsigned seed) noexcept {
    constexpr auto A=arm_feature::dotprod|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(sdot<A>(sample<std::int32_t,2,A>(seed+0), sample<std::int8_t,8,A>(seed+1), sample<std::int8_t,8,A>(seed+2)),
      sdot<software>(sample<std::int32_t,2,software>(seed+0), sample<std::int8_t,8,software>(seed+1), sample<std::int8_t,8,software>(seed+2)));
    ok&=equal(sdot_lane<A, 1>(sample<std::int32_t,2,A>(seed+0), sample<std::int8_t,8,A>(seed+1), sample<std::int8_t,16,A>(seed+2)),
      sdot_lane<software, 1>(sample<std::int32_t,2,software>(seed+0), sample<std::int8_t,8,software>(seed+1), sample<std::int8_t,16,software>(seed+2)));
    ok&=equal(udot<A>(sample<std::uint32_t,2,A>(seed+0), sample<std::uint8_t,8,A>(seed+1), sample<std::uint8_t,8,A>(seed+2)),
      udot<software>(sample<std::uint32_t,2,software>(seed+0), sample<std::uint8_t,8,software>(seed+1), sample<std::uint8_t,8,software>(seed+2)));
    return ok;
  }
  __attribute__((target("i8mm"), noinline))
  bool check_i8mm(unsigned seed) noexcept {
    constexpr auto A=arm_feature::i8mm|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(usdot<A>(sample<std::int32_t,2,A>(seed+0), sample<std::uint8_t,8,A>(seed+1), sample<std::int8_t,8,A>(seed+2)),
      usdot<software>(sample<std::int32_t,2,software>(seed+0), sample<std::uint8_t,8,software>(seed+1), sample<std::int8_t,8,software>(seed+2)));
    ok&=equal(sudot_lane<A, 1>(sample<std::int32_t,2,A>(seed+0), sample<std::int8_t,8,A>(seed+1), sample<std::uint8_t,16,A>(seed+2)),
      sudot_lane<software, 1>(sample<std::int32_t,2,software>(seed+0), sample<std::int8_t,8,software>(seed+1), sample<std::uint8_t,16,software>(seed+2)));
    return ok;
  }
  __attribute__((target("rdm"), noinline))
  bool check_rdm(unsigned seed) noexcept {
    constexpr auto A=arm_feature::rdm|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(sqrdmlah<A>(sample<std::int32_t,2,A>(seed+0), sample<std::int32_t,2,A>(seed+1), sample<std::int32_t,2,A>(seed+2)),
      sqrdmlah<software>(sample<std::int32_t,2,software>(seed+0), sample<std::int32_t,2,software>(seed+1), sample<std::int32_t,2,software>(seed+2)));
    ok&=equal(sqrdmlsh_lane<A, 1>(sample<std::int32_t,2,A>(seed+0), sample<std::int32_t,2,A>(seed+1), sample<std::int32_t,4,A>(seed+2)),
      sqrdmlsh_lane<software, 1>(sample<std::int32_t,2,software>(seed+0), sample<std::int32_t,2,software>(seed+1), sample<std::int32_t,4,software>(seed+2)));
    return ok;
  }
  __attribute__((target("complxnum,fullfp16"), noinline))
  bool check_fcma(unsigned seed) noexcept {
    constexpr auto A=arm_feature::complxnum|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(fcadd<A, 90>(sample<float,2,A>(seed+0), sample<float,2,A>(seed+1)),
      fcadd<software, 90>(sample<float,2,software>(seed+0), sample<float,2,software>(seed+1)));
    ok&=equal(fcmla_lane<A, 90, 1>(sample<float,2,A>(seed+0), sample<float,2,A>(seed+1), sample<float,4,A>(seed+2)),
      fcmla_lane<software, 90, 1>(sample<float,2,software>(seed+0), sample<float,2,software>(seed+1), sample<float,4,software>(seed+2)));
    ok&=equal(fcmla_lane<A, 0, 0>(sample<float,4,A>(seed+0), sample<float,4,A>(seed+1), sample<float,2,A>(seed+2)),
      fcmla_lane<software, 0, 0>(sample<float,4,software>(seed+0), sample<float,4,software>(seed+1), sample<float,2,software>(seed+2)));
    ok&=equal(fcmla<A, 90>(sample<double,2,A>(seed+0), sample<double,2,A>(seed+1), sample<double,2,A>(seed+2)),
      fcmla<software, 90>(sample<double,2,software>(seed+0), sample<double,2,software>(seed+1), sample<double,2,software>(seed+2)));
    return ok;
  }
  __attribute__((target("fp16fml"), noinline))
  bool check_fp16fml(unsigned seed) noexcept {
    constexpr auto A=arm_feature::fp16fml|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(fmlal<A>(sample<float,2,A>(seed+0), sample<fp16,4,A>(seed+1), sample<fp16,4,A>(seed+2)),
      fmlal<software>(sample<float,2,software>(seed+0), sample<fp16,4,software>(seed+1), sample<fp16,4,software>(seed+2)));
    ok&=equal(fmlsl_lane<A, 1>(sample<float,2,A>(seed+0), sample<fp16,4,A>(seed+1), sample<fp16,8,A>(seed+2)),
      fmlsl_lane<software, 1>(sample<float,2,software>(seed+0), sample<fp16,4,software>(seed+1), sample<fp16,8,software>(seed+2)));
    return ok;
  }
  __attribute__((target("bf16"), noinline))
  bool check_bf16(unsigned seed) noexcept {
    constexpr auto A=arm_feature::neon_bf16|polyfill;
    static_assert(!A.has(arm_feature::neon));
    bool ok=true;
    ok&=equal(bfdot<A>(sample<float,2,A>(seed+0), sample<bf16,4,A>(seed+1), sample<bf16,4,A>(seed+2)),
      bfdot<software>(sample<float,2,software>(seed+0), sample<bf16,4,software>(seed+1), sample<bf16,4,software>(seed+2)));
    ok&=equal(bfdot_lane<A, 3>(sample<float,2,A>(seed+0), sample<bf16,4,A>(seed+1), sample<bf16,8,A>(seed+2)),
      bfdot_lane<software, 3>(sample<float,2,software>(seed+0), sample<bf16,4,software>(seed+1), sample<bf16,8,software>(seed+2)));
    ok&=equal(bfdot<A>(sample<float,4,A>(seed+0), sample<bf16,8,A>(seed+1), sample<bf16,8,A>(seed+2)),
      bfdot<software>(sample<float,4,software>(seed+0), sample<bf16,8,software>(seed+1), sample<bf16,8,software>(seed+2)));
    ok&=equal(bfmmla<A>(sample<float,4,A>(seed+0), sample<bf16,8,A>(seed+1), sample<bf16,8,A>(seed+2)),
      bfmmla<software>(sample<float,4,software>(seed+0), sample<bf16,8,software>(seed+1), sample<bf16,8,software>(seed+2)));
    ok&=equal(bfmlalb_lane<A, 3>(sample<float,4,A>(seed+0), sample<bf16,8,A>(seed+1), sample<bf16,4,A>(seed+2)),
      bfmlalb_lane<software, 3>(sample<float,4,software>(seed+0), sample<bf16,8,software>(seed+1), sample<bf16,4,software>(seed+2)));
    return ok;
  }
}
int main(int argc,char **) {
  auto cpu=observe_arm_capabilities();
  constexpr auto required=[] {
    isa<arm> result{};
    for(auto f : {arm_feature::dotprod,arm_feature::i8mm,arm_feature::rdm,
        arm_feature::complxnum,arm_feature::fp16fml,arm_feature::neon_bf16}) result.set(f,true);
    return result;
  }();
  if(!classify_isa(cpu,required).admitted()) return 77;
  bool ok=check_dotprod(argc)&&check_i8mm(argc)&&check_rdm(argc)&&
    check_fcma(argc)&&check_fp16fml(argc)&&check_bf16(argc);
  if(!ok) std::puts("feature-only carrier result mismatch");
  return !ok;
}
