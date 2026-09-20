// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>
#include <algorithm>
#include "../core_regression/support/profile.h"
#include "../core_regression/support/guarded_pages.h"
#include "../core_regression/support/fp_environment.h"
template<class V> concept has_compaction = requires(V value, typename V::mask mask) {
  native::compress(mask,value);native::expand(mask,value,value);
};
static_assert(!has_compaction<test_vec<std::uint16_t,1>>);
static_assert(!has_compaction<test_vec<std::uint64_t,1>>);
static std::uint64_t checks;
static void require(bool condition) {
  ++checks;
  if (!condition) { std::fprintf(stderr,"compaction failure at check %llu\n",(unsigned long long)checks); std::abort(); }
}
constexpr std::array<std::uint32_t,20> bank{
  0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,
  0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,
  0x7fc12345,0xffc12345,0x7f800001,0xff800001,0x12345678,0xffffffff};
template<class V> auto words(V value) {
  std::array<typename V::value_type,V::lanes> values;
  value.store(values.data());
  return std::bit_cast<std::array<std::uint32_t,V::lanes>>(values);
}
template<class V> V from_words(std::array<std::uint32_t,V::lanes> const & bits) {
  auto values=std::bit_cast<std::array<typename V::value_type,V::lanes>>(bits);
  return V::load(values.data());
}
template<class T,std::size_t N> void test_shape() {
  using V=test_vec<T,N>; using M=typename V::mask;
  static_assert(std::same_as<decltype(native::compress(M{},V{})),native::compaction_result<V>>);
  static_assert(std::same_as<decltype(native::expand(M{},V{},V{})),V>);
  static_assert(noexcept(native::compress(M{},V{})) && noexcept(native::expand(M{},V{},V{})));
  native::test::guarded_pages pages;
  auto * storage=::new (pages.end()-(N+1)*sizeof(T)) T[N+1];
  constexpr std::uint32_t sentinel=0xabcdef12;
  auto rotations=N<=8 ? bank.size() : std::size_t(5);
  for (std::size_t rotation=0;rotation<rotations;++rotation) {
    std::array<std::uint32_t,N> input{},prior{};
    for (std::size_t lane=0;lane<N;++lane) {
      input[lane]=bank[(rotation+lane)%bank.size()];
      prior[lane]=bank[(rotation+lane+7)%bank.size()];
    }
    V value=from_words<V>(input),merge=from_words<V>(prior);
    // Polluted physical padding must neither contribute nor survive in outputs.
    if constexpr(N==2 || N==3) {
      auto native=std::bit_cast<std::array<std::uint32_t,4>>(value);
      for (std::size_t lane=N;lane<4;++lane) native[lane]=0x7f800001;
      value=std::bit_cast<V>(native);
      merge=std::bit_cast<V>(native);
      prior=input;
    }
    for (std::uint32_t mask=0;mask<(std::uint32_t(1)<<N);++mask) {
      // Supplying high mask bits must not make padding into logical lanes.
      auto active=M::from_bitset(std::uint64_t(mask)|(~std::uint64_t(0)<<N));
      auto fill=bank[(rotation+13)%bank.size()];
      std::array<std::uint32_t,N> packed{},expanded=prior;
      packed.fill(fill); std::size_t count=0;
      for(std::size_t lane=0;lane<N;++lane) if(mask&(1u<<lane)) packed[count++]=input[lane];
      auto compact=native::compress(active,value,std::bit_cast<T>(fill));
      require(compact.count==count && words(compact.value)==packed);
      auto default_compact=native::compress(active,value);
      auto default_packed=packed;
      for(std::size_t lane=count;lane<N;++lane) default_packed[lane]=0;
      require(default_compact.count==count && words(default_compact.value)==default_packed);
      std::size_t rank=0;
      for(std::size_t lane=0;lane<N;++lane) if(mask&(1u<<lane)) expanded[lane]=packed[rank++];
      auto restored=native::expand(active,compact.value,merge);
      require(words(restored)==expanded);
      require(words(restored)==words(select(active,value,merge)));
      if constexpr(N==2 || N==3) {
        auto c=std::bit_cast<std::array<std::uint32_t,4>>(compact.value);
        auto e=std::bit_cast<std::array<std::uint32_t,4>>(restored);
        for(std::size_t lane=N;lane<4;++lane) require(c[lane]==0 && e[lane]==0);
      }
      require(native::compress_store(static_cast<T*>(nullptr),0,active,value)==0);
      if(!count) require(native::compress_store(static_cast<T*>(nullptr),N+5,active,value)==0);
      for(std::size_t capacity=1;capacity<=N+1;++capacity) {
        auto written=std::min(capacity,count);
        for(std::size_t lane=0;lane<N+1;++lane) storage[lane]=std::bit_cast<T>(sentinel);
        auto * destination=storage+N+1-written;
        require(native::compress_store(destination,capacity,active,value)==written);
        require(std::bit_cast<std::uint32_t>(destination[-1])==sentinel);
        for(std::size_t lane=0;lane<written;++lane)
          require(std::bit_cast<std::uint32_t>(destination[lane])==packed[lane]);
      }
    }
  }
}
template<class T> void test_type() {
  test_shape<T,1>();
#if NATIVE_TEST_PROFILE != 0
  test_shape<T,2>();test_shape<T,3>();test_shape<T,4>();
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
  test_shape<T,8>();
#endif
#if NATIVE_TEST_PROFILE == 512
  test_shape<T,16>();
#endif
#endif
}

// Stable external wrappers make the actual register lowering easy to inspect.
#if NATIVE_TEST_PROFILE == 512
constexpr std::size_t native_lanes=16;
#elif NATIVE_TEST_PROFILE == 256
constexpr std::size_t native_lanes=8;
#elif NATIVE_TEST_PROFILE == 128
constexpr std::size_t native_lanes=4;
#else
constexpr std::size_t native_lanes=1;
#endif
using native_v=test_vec<std::uint32_t,native_lanes>;
using native_m=typename native_v::mask;
extern "C" __attribute__((noinline)) std::size_t compaction_codegen(std::uint32_t * out,std::uint32_t const * input,std::uint32_t mask) {
  auto result=native::compress(native_m::from_bitset(mask),native_v::load(input));
  result.value.store(out);return result.count;
}
extern "C" __attribute__((noinline)) void expansion_codegen(std::uint32_t * out,std::uint32_t const * input,std::uint32_t mask) {
  native::expand(native_m::from_bitset(mask),native_v::load(input),native_v(0xabcdef12u)).store(out);
}
extern "C" __attribute__((noinline)) std::size_t compaction_store_codegen(std::uint32_t * out,std::uint32_t const * input,std::uint32_t mask,std::size_t capacity) {
  return native::compress_store(out,capacity,native_m::from_bitset(mask),native_v::load(input));
}
template<bool Native,unsigned Operation> __attribute__((noinline)) std::uint64_t batch(std::uint32_t * out,std::uint32_t const * input,std::uint32_t const * masks,std::size_t blocks) {
  std::uint64_t checksum=0;
  for(std::size_t block=0;block<blocks;++block) {
    auto mask=masks[block];std::size_t count=0;
    if constexpr(Native) {
      auto value=native_v::load(input+block*native_lanes);
      auto active=native_m::from_bitset(mask);
      if constexpr(Operation==0) {
        auto result=native::compress(active,value);
        result.value.store(out+block*native_lanes);count=result.count;
      } else if constexpr(Operation==1) {
        native::expand(active,value,native_v(0u)).store(out+block*native_lanes);
        count=std::popcount(mask);
      } else count=native::compress_store(out+block*native_lanes,native_lanes,active,value);
    } else if constexpr(Operation==1) {
      for(std::size_t lane=0;lane<native_lanes;++lane)
        out[block*native_lanes+lane]=(mask&(1u<<lane)) ? input[block*native_lanes+count++] : 0;
    } else {
      for(std::size_t lane=0;lane<native_lanes;++lane) if(mask&(1u<<lane))
        out[block*native_lanes+count++]=input[block*native_lanes+lane];
      if constexpr(Operation==0)
        for(std::size_t lane=count;lane<native_lanes;++lane) out[block*native_lanes+lane]=0;
    }
    checksum+=count;
  }
  return checksum;
}
static volatile std::uint64_t benchmark_sink;
static void benchmark() {
  constexpr std::size_t blocks=4096,repeats=64;
  std::vector<std::uint32_t> input(blocks*native_lanes),native(input.size()),reference(input.size()),masks(blocks);
  for(std::size_t i=0;i<input.size();++i) input[i]=std::uint32_t(i*0x9e3779b9u);
  constexpr auto all=(1u<<native_lanes)-1;
  for(unsigned operation=0;operation!=3;++operation)
  for(char const * density: {"empty","full","alternating","random"}) {
    std::fill(native.begin(),native.end(),0);std::fill(reference.begin(),reference.end(),0);
    auto run=[&](bool use_native) {
      if(operation==0) return use_native ? batch<true,0>(native.data(),input.data(),masks.data(),blocks) : batch<false,0>(reference.data(),input.data(),masks.data(),blocks);
      if(operation==1) return use_native ? batch<true,1>(native.data(),input.data(),masks.data(),blocks) : batch<false,1>(reference.data(),input.data(),masks.data(),blocks);
      return use_native ? batch<true,2>(native.data(),input.data(),masks.data(),blocks) : batch<false,2>(reference.data(),input.data(),masks.data(),blocks);
    };
    std::uint32_t random=12345;
    for(auto & mask:masks) { random=random*1664525u+1013904223u;
      mask=std::strcmp(density,"empty")==0 ? 0 : std::strcmp(density,"full")==0 ? all :
        std::strcmp(density,"alternating")==0 ? (0x5555u&all) : (random>>8)&all; }
    require(run(true)==run(false));
    require(native==reference);
    std::array<double,7> a{},b{};
    auto time=[&](bool use_native) {
      auto start=std::chrono::steady_clock::now();std::uint64_t sum=0;
      for(std::size_t repeat=0;repeat<repeats;++repeat)
      {
        sum+=run(use_native);
        std::atomic_signal_fence(std::memory_order_seq_cst);
      }
      benchmark_sink=sum;
      return std::chrono::duration<double,std::nano>(std::chrono::steady_clock::now()-start).count()/(blocks*repeats);
    };
    for(std::size_t pair=0;pair<a.size();++pair) {
      if(pair&1) { b[pair]=time(false);a[pair]=time(true); }
      else { a[pair]=time(true);b[pair]=time(false); }
      std::printf("pair profile=%d operation=%u lanes=%zu masks=%s index=%zu native_ns=%.3f reference_ns=%.3f\n",NATIVE_TEST_PROFILE,operation,native_lanes,density,pair,a[pair],b[pair]);
    }
    std::sort(a.begin(),a.end());std::sort(b.begin(),b.end());
    std::printf("median profile=%d operation=%u lanes=%zu masks=%s native_ns=%.3f reference_ns=%.3f\n",NATIVE_TEST_PROFILE,operation,native_lanes,density,a[3],b[3]);
  }
}
int compaction_entry(int argc,char ** argv) {
  if(argc==2 && std::strcmp(argv[1],"--bench")==0) { benchmark(); return 0; }
  auto before=native::test::read_fp_state();
  test_type<float>();test_type<std::int32_t>();test_type<std::uint32_t>();
  require(native::test::read_fp_state()==before);
  std::printf("profile=%d compaction checks=%llu\n",NATIVE_TEST_PROFILE,(unsigned long long)checks);
  return 0;
}
