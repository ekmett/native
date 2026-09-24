// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstdint>
#include <limits>
#include <native/attributes.h>
import native.x86.memory;

constexpr auto full = native::target_features<native::x86>("avx512f,avx512vl");
constexpr auto avx = native::target_features<native::x86>("avx2");
constexpr auto weak = native::target_features<native::x86>("sse4.2");
using indices = native::simd<std::int32_t,4,full>;
using values = native::simd<float,4,full>;
using mask = native::predicate<4,full>;

template<int Scale, class I>
concept gather_shape = requires(float const * p, I i) {
  { native::vgatherdps<Scale,4>(p,i) } -> std::same_as<native::simd<float,4,I::architecture>>;
};
static_assert(gather_shape<4,indices>);
static_assert(!gather_shape<0,indices> && !gather_shape<3,indices> && !gather_shape<16,indices>);
static_assert(!gather_shape<4,native::simd<std::uint32_t,4,full>>);
static_assert(!gather_shape<4,native::simd<std::int64_t,2,full>>);
static_assert(!gather_shape<4,native::simd<std::int32_t,8,full>>);

template<class M>
concept gather_mask = requires(float const * p, indices i, values v, M m) {
  { native::mask_vgatherdps<4>(v,m,p,i) } -> std::same_as<values>;
};
static_assert(gather_mask<mask> && gather_mask<native::simd<std::int32_t,4,full>>);
static_assert(!gather_mask<unsigned> && !gather_mask<native::predicate<8,full>>);
static_assert(!gather_mask<native::predicate<4,avx>>);

consteval bool weak_semantics() {
  using index_type = native::simd<std::int32_t,4,weak>;
  using value_type = native::simd<float,4,weak>;
  using predicate_type = native::predicate<4,weak>;
  float memory[]{10,20,30,40};
  index_type i(std::array<std::int32_t,4>{3,1,2,0});
  std::array<float,4> result{};
  native::vgatherdps<4,4>(memory,i).store(result.data());
  if (result != std::array<float,4>{40,20,30,10}) return false;
  native::mask_vgatherdps<4>(value_type{},predicate_type::from_bitset(5),memory,i).store(result.data());
  if (result != std::array<float,4>{40,0,30,0}) return false;
  native::mask_vscatterdps<4>(memory,predicate_type::from_bitset(7),index_type{},
    value_type(std::array<float,4>{1,2,3,4}));
  return memory[0] == 3;
}
static_assert(weak_semantics());

#if NATIVE_MEMORY_REJECT == 9
// The public wrapper cannot inline into a baseline caller.
native_noinline auto rejection(float const * p, indices i) {
  return native::vgatherdps<4,4>(p,i);
}
#elif NATIVE_MEMORY_REJECT == 10
native_noinline auto rejection(float const * p, native::simd<std::int32_t,4,weak> i) {
  return native::vgatherdps<4,4>(p,i);
}
#elif NATIVE_MEMORY_REJECT == 11
native_noinline auto rejection(float const * p, native::simd<std::int32_t,4,avx> i) {
  return native::mask_vgatherdps<4>(native::simd<float,4,avx>{},native::predicate<4,avx>{},p,i);
}
#elif NATIVE_MEMORY_REJECT == 12
native_noinline void rejection(float * p, native::simd<std::int32_t,4,avx> i, native::simd<float,4,avx> v) {
  native::mask_vscatterdps<4>(p,native::predicate<4,avx>{},i,v);
}
#elif NATIVE_MEMORY_REJECT == 13
constexpr auto rejected_constant = [] {
  float memory[]{1,2,3,4};
  return native::vgatherdps<1,4>(memory,indices(std::array<std::int32_t,4>{1,0,0,0}));
}();
#elif NATIVE_MEMORY_REJECT == 14
constexpr auto rejected_constant = [] {
  float memory[]{1,2,3,4};
  return native::vgatherdps<4,4>(memory,indices(std::array<std::int32_t,4>{4,0,0,0}));
}();
#elif NATIVE_MEMORY_REJECT == 15
constexpr auto rejected_constant = [] {
  std::uint32_t memory[]{1,2,3,4};
  native::simd<std::int64_t,2,full> i(std::array<std::int64_t,2>{std::numeric_limits<std::int64_t>::max(),0});
  return native::vpgatherqd<8,4>(memory,i);
}();
#elif NATIVE_MEMORY_REJECT == 16
constexpr auto rejected_constant = [] {
  float memory[]{1,2,3,4};
  native::mask_vscatterdps<1>(memory,mask::from_bitset(1),indices(std::array<std::int32_t,4>{1,0,0,0}),values{});
  return memory[0];
}();
#elif NATIVE_MEMORY_REJECT == 17
constexpr auto only_f = native::target_features<native::x86>("avx512f");
native_noinline auto rejection(float const * p, native::simd<std::int32_t,4,only_f> i) {
  return native::mask_vgatherdps<4>(native::simd<float,4,only_f>{},native::predicate<4,only_f>{},p,i);
}
#elif NATIVE_MEMORY_REJECT == 18
constexpr auto only_f = native::target_features<native::x86>("avx512f");
native_noinline void rejection(float * p, native::simd<std::int32_t,4,only_f> i, native::simd<float,4,only_f> v) {
  native::mask_vscatterdps<4>(p,native::predicate<4,only_f>{},i,v);
}
#else
native_noinline native_target("avx512f,avx512vl")
auto rejection(float const * p, indices i, values v) {
#if NATIVE_MEMORY_REJECT == 1
  return native::vgatherdps<0,4>(p,i);
#elif NATIVE_MEMORY_REJECT == 2
  return native::vgatherdps<3,4>(p,i);
#elif NATIVE_MEMORY_REJECT == 3
  return native::vgatherdps<4,2>(p,i);
#elif NATIVE_MEMORY_REJECT == 4
  return native::vgatherdps<4,4>(p,native::simd<std::uint32_t,4,full>{});
#elif NATIVE_MEMORY_REJECT == 5
  return native::vgatherdps<4,4>(p,native::simd<std::int64_t,2,full>{});
#elif NATIVE_MEMORY_REJECT == 6
  return native::mask_vgatherdps<4>(v,0u,p,i);
#elif NATIVE_MEMORY_REJECT == 7
  return native::mask_vgatherdps<4>(v,native::predicate<8,full>{},p,i);
#elif NATIVE_MEMORY_REJECT == 8
  return native::mask_vgatherdps<4>(v,native::predicate<4,avx>{},p,i);
#else
  return native::mask_vgatherdps<4>(v,mask::from_bitset(5),p,i);
#endif
}
#endif
