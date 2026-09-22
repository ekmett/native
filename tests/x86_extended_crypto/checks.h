// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <cstdint>

namespace checks {
  using namespace native;
  template<class V> constexpr auto words(V x) {
    std::array<typename V::value_type, V::lanes> out{};
    x.store(out.data());
    return out;
  }
  inline constexpr std::array<std::uint64_t, 80> k512{
    0x428a2f98d728ae22,0x7137449123ef65cd,0xb5c0fbcfec4d3b2f,0xe9b5dba58189dbbc,
    0x3956c25bf348b538,0x59f111f1b605d019,0x923f82a4af194f9b,0xab1c5ed5da6d8118,
    0xd807aa98a3030242,0x12835b0145706fbe,0x243185be4ee4b28c,0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f,0x80deb1fe3b1696b1,0x9bdc06a725c71235,0xc19bf174cf692694,
    0xe49b69c19ef14ad2,0xefbe4786384f25e3,0x0fc19dc68b8cd5b5,0x240ca1cc77ac9c65,
    0x2de92c6f592b0275,0x4a7484aa6ea6e483,0x5cb0a9dcbd41fbd4,0x76f988da831153b5,
    0x983e5152ee66dfab,0xa831c66d2db43210,0xb00327c898fb213f,0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2,0xd5a79147930aa725,0x06ca6351e003826f,0x142929670a0e6e70,
    0x27b70a8546d22ffc,0x2e1b21385c26c926,0x4d2c6dfc5ac42aed,0x53380d139d95b3df,
    0x650a73548baf63de,0x766a0abb3c77b2a8,0x81c2c92e47edaee6,0x92722c851482353b,
    0xa2bfe8a14cf10364,0xa81a664bbc423001,0xc24b8b70d0f89791,0xc76c51a30654be30,
    0xd192e819d6ef5218,0xd69906245565a910,0xf40e35855771202a,0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8,0x1e376c085141ab53,0x2748774cdf8eeb99,0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63,0x4ed8aa4ae3418acb,0x5b9cca4f7763e373,0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc,0x78a5636f43172f60,0x84c87814a1f0ab72,0x8cc702081a6439ec,
    0x90befffa23631e28,0xa4506cebde82bde9,0xbef9a3f7b2c67915,0xc67178f2e372532b,
    0xca273eceea26619c,0xd186b8c721c0c207,0xeada7dd6cde0eb1e,0xf57d4f7fee6ed178,
    0x06f067aa72176fba,0x0a637dc5a2c898a6,0x113f9804bef90dae,0x1b710b35131c471b,
    0x28db77f523047d84,0x32caab7b40c72493,0x3c9ebe0a15c9bebc,0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6,0x597f299cfc657e2a,0x5fcb6fab3ad6faec,0x6c44198c4a475817};

  template<isa<x86> Arch> [[gnu::target("sha512")]] constexpr bool sha512_block(std::array<std::uint64_t,16> const & block,
      std::array<std::uint64_t,8> const & expected) {
    using V = simd<std::uint64_t, 4, Arch>;
    using W = simd<std::uint64_t, 2, Arch>;
    constexpr std::array<std::uint64_t, 8> initial{
      0x6a09e667f3bcc908,0xbb67ae8584caa73b,0x3c6ef372fe94f82b,0xa54ff53a5f1d36f1,
      0x510e527fade682d1,0x9b05688c2b3e6c1f,0x1f83d9abfb41bd6b,0x5be0cd19137e2179};
    std::array<std::uint64_t, 80> w{};
    for (unsigned i=0;i<16;++i) w[i]=block[i];
    // The message instructions generate all 64 expanded schedule words.
    for (unsigned i = 16; i < 80; i += 4) {
      auto first = sha512msg1<Arch>(V::load(w.data() + i - 16), W::load(w.data() + i - 12));
      auto x = words(first);
      for (unsigned j = 0; j < 4; ++j) x[j] += w[i + j - 7];
      sha512msg2<Arch>(V::load(x.data()), V::load(w.data() + i - 4)).store(w.data() + i);
    }
    auto a = V::load(std::array{initial[7],initial[6],initial[3],initial[2]}.data());
    auto b = V::load(std::array{initial[5],initial[4],initial[1],initial[0]}.data());
    for (unsigned i = 0; i < 80; i += 2) {
      auto next = sha512rnds2<Arch>(a,b,W::load(std::array{w[i]+k512[i],w[i+1]+k512[i+1]}.data()));
      a = b; b = next;
    }
    auto x = words(a), y = words(b);
    std::array result{y[3],y[2],x[3],x[2],y[1],y[0],x[1],x[0]};
    for (unsigned i = 0; i < 8; ++i) result[i] += initial[i];
    return result == expected;
  }
  template<isa<x86> Arch> [[gnu::target("sha512")]] constexpr bool sha512_abc() {
    std::array<std::uint64_t,16> block{};
    block[0]=0x6162638000000000; block[15]=24;
    return sha512_block<Arch>(block, std::array<std::uint64_t,8>{
      0xddaf35a193617aba,0xcc417349ae204131,0x12e6fa4e89a97ea2,0x0a9eeee64b55d39a,
      0x2192992a274fc1a8,0x36ba3c23a3feebbd,0x454d4423643ce80e,0x2a9ac94fa54ca49f});
  }

  template<unsigned Round, isa<x86> Arch>
  [[gnu::target("sm3")]] constexpr auto sm3_rounds(
      simd<std::uint32_t,4,Arch> a, simd<std::uint32_t,4,Arch> b,
      std::array<std::uint32_t,68> const & w) {
    using V = simd<std::uint32_t,4,Arch>;
    if constexpr (Round < 64) {
      auto message = V::load(std::array{w[Round],w[Round+1],w[Round+4],w[Round+5]}.data());
      auto next = sm3rnds2<Arch,Round>(a,b,message);
      return sm3_rounds<Round+2,Arch>(b,next,w);
    } else {
      auto x = words(a), y = words(b);
      return std::array{y[3],y[2],std::rotl(x[3],9),std::rotl(x[2],9),
        y[1],y[0],std::rotl(x[1],19),std::rotl(x[0],19)};
    }
  }
  template<isa<x86> Arch> [[gnu::target("sm3")]] constexpr bool sm3_block(std::array<std::uint32_t,16> const & block,
      std::array<std::uint32_t,8> const & expected) {
    using V = simd<std::uint32_t,4,Arch>;
    constexpr std::array<std::uint32_t,8> initial{
      0x7380166f,0x4914b2b9,0x172442d7,0xda8a0600,0xa96f30bc,0x163138aa,0xe38dee4d,0xb0fb0e4e};
    std::array<std::uint32_t,68> w{};
    for (unsigned i=0;i<16;++i) w[i]=block[i];
    for (unsigned i = 16; i < 68; i += 4) {
      auto first = sm3msg1<Arch>(V::load(w.data()+i-9),V::load(w.data()+i-3),V::load(w.data()+i-16));
      sm3msg2<Arch>(first,V::load(w.data()+i-13),V::load(w.data()+i-6)).store(w.data()+i);
    }
    auto a = V::load(std::array{std::rotr(initial[7],19),std::rotr(initial[6],19),
      std::rotr(initial[3],9),std::rotr(initial[2],9)}.data());
    auto b = V::load(std::array{initial[5],initial[4],initial[1],initial[0]}.data());
    auto result = sm3_rounds<0,Arch>(a,b,w);
    for (unsigned i = 0; i < 8; ++i) result[i] ^= initial[i];
    return result == expected;
  }
  template<isa<x86> Arch> [[gnu::target("sm3")]] constexpr bool sm3_abc() {
    std::array<std::uint32_t,16> block{};
    block[0]=0x61626380; block[15]=24;
    return sm3_block<Arch>(block, std::array<std::uint32_t,8>{0x66c7f0f4,0x62eeedd9,0xd1f2d46b,0xdc10e4e2,
      0x4167c487,0x5cf2f7a2,0x297da02b,0x8f4ba8e0});
  }

  template<isa<x86> Arch, unsigned N> [[gnu::target("sm4")]] constexpr bool sm4_example() {
    using V = simd<std::uint32_t,N,Arch>;
    constexpr std::array<std::uint32_t,4> input{0x01234567,0x89abcdef,0xfedcba98,0x76543210};
    constexpr std::array<std::uint32_t,4> fk{0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc};
    std::array<std::uint32_t,N> initial{}, key{};
    for (unsigned i=0;i<N;++i) { initial[i]=input[i%4]; key[i]=input[i%4]^fk[i%4]; }
    auto x=V::load(initial.data()), k=V::load(key.data());
    for (unsigned round=0;round<32;round+=4) {
      std::array<std::uint32_t,N> ck{};
      for (unsigned i=0;i<N;++i)
        for (unsigned byte=0;byte<4;++byte)
          ck[i] |= (((4*(round+i%4)+byte)*7)&255u) << (24-8*byte);
      k=sm4key4<Arch>(k,V::load(ck.data()));
      if (round==0) {
        auto keys=words(k);
        constexpr std::array<std::uint32_t,4> expected{0xf12186f9,0x41662b61,0x5a6ab19a,0x7ba92077};
        for (unsigned i=0;i<N;++i) if(keys[i]!=expected[i%4]) return false;
      }
      x=sm4rnds4<Arch>(x,k);
    }
    auto result=words(x);
    constexpr std::array<std::uint32_t,4> expected{0x536e4246,0x86b3e94f,0xd206965e,0x681edf34};
    for (unsigned i=0;i<N;++i) if(result[i]!=expected[i%4]) return false;
    return true;
  }
}
