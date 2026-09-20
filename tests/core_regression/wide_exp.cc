#include "support/failure.h"
#include "support/profile.h"
#include "support/fp_environment.h"
#include "reference.h"
#include <bit>
#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
using test_backend::native::fp32x1;
static_assert(std::tuple_size_v<decltype(test_backend::native::exp(std::array<fp32x1,0>{}))> == 0);
bool equal(unsigned a,unsigned b) { return a==b || ((a&0x7fffffffu)>0x7f800000u && (b&0x7fffffffu)>0x7f800000u); }
template<class V> void check(std::vector<unsigned> const & input) {
  constexpr std::size_t count=96/V::lanes;
  for(std::size_t i=0;i<input.size();i+=96) {
    native::wide<V,count> x;
    for(std::size_t k=0;k<count;++k) {
      std::array<unsigned,V::lanes> words{};
      for(std::size_t j=0;j<V::lanes;++j) if(i+k*V::lanes+j<input.size()) words[j]=input[i+k*V::lanes+j];
      x.registers[k]=V::load_bits(words.data());
    }
    auto before=exp_before::exp(x);
    auto after=test_backend::native::exp(x.registers);
    for(std::size_t k=0;k<count;++k) {
      std::array<unsigned,V::lanes>a,b; before.registers[k].store_bits(a.data());after[k].store_bits(b.data());
      for(std::size_t j=0;j<V::lanes;++j) {
        if(!equal(a[j],b[j])) {
          std::cerr<<"mismatch lanes="<<V::lanes<<" input="<<std::hex<<input[std::min(i+k*V::lanes+j,input.size()-1)]<<" old="<<a[j]<<" new="<<b[j]<<'\n';native::test::fail(std::runtime_error("exp mismatch"));
        }
      }
    }
  }
}
int main() {
  std::vector<unsigned> words{0,0x80000000u,0x7f800000u,0xff800000u,0x7fc00000u,0x7f800001u,0xff800001u,0xffffffffu};
  for(unsigned e=0;e<256;++e)for(unsigned f:{0u,1u,0x003fffffu,0x007ffffeu,0x007fffffu})for(unsigned s:{0u,0x80000000u})words.push_back(s|(e<<23)|f);
  for(unsigned c:{0xc2aeac50u,0xc2d00000u,0x42b17218u})for(int d=-4096;d<=4096;++d)words.push_back(c+unsigned(d));
  for(unsigned b=0xc2aeac40u;b<0xc2d00010u;++b) words.push_back(b);
  unsigned random=0x379bae12u; for(unsigned i=0;i<1000000;++i){random^=random<<13;random^=random>>17;random^=random<<5;words.push_back(random);}
  auto saved=native::test::read_fp_state();
  for(auto mode:{native::test::fp_mode::gradual,native::test::fp_mode::flush}) {
    native::test::fp_scope scope(mode);check<fp32x1>(words);
#if defined(__AVX2__) || defined(__ARM_NEON)
    check<test_backend::native::fp32x4>(words);
#endif
#if defined(__AVX2__)
    check<test_backend::native::fp32x8>(words);
#endif
#if defined(__AVX512F__)
    check<test_backend::native::fp32x16>(words);
#endif
    if(!scope.controls_match())native::test::fail(std::runtime_error("controls changed"));
  }
  if(native::test::read_fp_state()!=saved)native::test::fail(std::runtime_error("state not restored"));
  std::cout<<"exact raw baseline comparisons passed; input words="<<words.size()<<" per width per mode\n";
}

/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Checks direct wide exp against its independent historical graph.
 */
