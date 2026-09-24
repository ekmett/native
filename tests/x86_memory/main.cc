// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <immintrin.h>
#include <native/attributes.h>
#if NATIVE_TEST_INTERFACE == 2
import native;
#else
import native.x86.memory;
#endif

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

struct guarded_page {
  void * allocation = nullptr;
  std::size_t size = 0;
  guarded_page() {
#if defined(_WIN32)
    SYSTEM_INFO info{};
    GetSystemInfo(&info);
    size = info.dwPageSize;
    allocation = VirtualAlloc(nullptr, size * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD previous;
    if (allocation && !VirtualProtect(static_cast<char *>(allocation) + size, size, PAGE_NOACCESS, &previous)) {
      VirtualFree(allocation, 0, MEM_RELEASE);
      allocation = nullptr;
    }
#else
    size = static_cast<std::size_t>(sysconf(_SC_PAGESIZE));
    allocation = mmap(nullptr, size * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (allocation == MAP_FAILED) allocation = nullptr;
    if (allocation && mprotect(static_cast<char *>(allocation) + size, size, PROT_NONE)) {
      munmap(allocation, size * 2);
      allocation = nullptr;
    }
#endif
  }
  ~guarded_page() {
    if (!allocation) return;
#if defined(_WIN32)
    VirtualFree(allocation, 0, MEM_RELEASE);
#else
    munmap(allocation, size * 2);
#endif
  }
  template<class T> T * last() { return reinterpret_cast<T *>(static_cast<char *>(allocation) + size) - 1; }
};

inline constexpr auto profile_0 = native::target_features<native::x86>("avx2");
inline constexpr auto profile_1 = native::target_features<native::x86>("avx512f");
inline constexpr auto profile_2 = native::target_features<native::x86>("avx512f,avx512vl");

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_0(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherdps<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_0<1,float>());
static_assert(check_0<2,float>());
static_assert(check_0<4,float>());
static_assert(check_0<8,float>());

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_1(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdps<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_1<1,float>());
static_assert(check_1<2,float>());
static_assert(check_1<4,float>());
static_assert(check_1<8,float>());

template<class T>
native_noinline native_target("avx2")
bool guard_1(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_2(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdps<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_2<1,float>());
static_assert(check_2<2,float>());
static_assert(check_2<4,float>());
static_assert(check_2<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_2(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_3(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdps<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_3<1,float>());
static_assert(check_3<2,float>());
static_assert(check_3<4,float>());
static_assert(check_3<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_3(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdps<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_4(int seed = 0) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherdps<Scale,8>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_4<1,float>());
static_assert(check_4<2,float>());
static_assert(check_4<4,float>());
static_assert(check_4<8,float>());

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_5(int seed = 0) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,8> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdps<Scale>(value_type(source), native::simd<std::int32_t,8,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_5<1,float>());
static_assert(check_5<2,float>());
static_assert(check_5<4,float>());
static_assert(check_5<8,float>());

template<class T>
native_noinline native_target("avx2")
bool guard_5(T * base) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,8> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::simd<std::int32_t,8,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::simd<std::int32_t,8,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_6(int seed = 0) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdps<Scale>(value_type(source), native::predicate<8,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_6<1,float>());
static_assert(check_6<2,float>());
static_assert(check_6<4,float>());
static_assert(check_6<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_6(T * base) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<8,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<8,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_7(int seed = 0) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdps<Scale>(memory.data()+40, native::predicate<8,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_7<1,float>());
static_assert(check_7<2,float>());
static_assert(check_7<4,float>());
static_assert(check_7<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_7(T * base) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdps<sizeof(T)>(base, native::predicate<8,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = float>
native_noinline native_target("avx512f")
constexpr bool check_8(int seed = 0) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<T,96> memory{};
  std::array<T,16> source{};
  std::array<std::int32_t,16> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdps<Scale>(value_type(source), native::predicate<16,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 16 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_8<1,float>());
static_assert(check_8<2,float>());
static_assert(check_8<4,float>());
static_assert(check_8<8,float>());

template<class T>
native_noinline native_target("avx512f")
bool guard_8(T * base) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<std::int32_t,16> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,16> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<16,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdps<sizeof(T)>(value_type(source), native::predicate<16,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<16; ++i) {
    if (result[i] != (i >= 16 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 16 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f")
constexpr bool check_9(int seed = 0) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<T,96> memory{};
  std::array<T,16> source{};
  std::array<std::int32_t,16> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<16; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdps<Scale>(memory.data()+40, native::predicate<16,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_9<1,float>());
static_assert(check_9<2,float>());
static_assert(check_9<4,float>());
static_assert(check_9<8,float>());

template<class T>
native_noinline native_target("avx512f")
bool guard_9(T * base) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<std::int32_t,16> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,16> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdps<sizeof(T)>(base, native::predicate<16,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<16,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_10(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherdd<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_10<1,std::int32_t>());
static_assert(check_10<2,std::int32_t>());
static_assert(check_10<4,std::int32_t>());
static_assert(check_10<8,std::int32_t>());
static_assert(check_10<1,std::uint32_t>());
static_assert(check_10<2,std::uint32_t>());
static_assert(check_10<4,std::uint32_t>());
static_assert(check_10<8,std::uint32_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_11(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdd<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_11<1,std::int32_t>());
static_assert(check_11<2,std::int32_t>());
static_assert(check_11<4,std::int32_t>());
static_assert(check_11<8,std::int32_t>());
static_assert(check_11<1,std::uint32_t>());
static_assert(check_11<2,std::uint32_t>());
static_assert(check_11<4,std::uint32_t>());
static_assert(check_11<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_11(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_12(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdd<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_12<1,std::int32_t>());
static_assert(check_12<2,std::int32_t>());
static_assert(check_12<4,std::int32_t>());
static_assert(check_12<8,std::int32_t>());
static_assert(check_12<1,std::uint32_t>());
static_assert(check_12<2,std::uint32_t>());
static_assert(check_12<4,std::uint32_t>());
static_assert(check_12<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_12(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_13(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdd<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_13<1,std::int32_t>());
static_assert(check_13<2,std::int32_t>());
static_assert(check_13<4,std::int32_t>());
static_assert(check_13<8,std::int32_t>());
static_assert(check_13<1,std::uint32_t>());
static_assert(check_13<2,std::uint32_t>());
static_assert(check_13<4,std::uint32_t>());
static_assert(check_13<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_13(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdd<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_14(int seed = 0) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherdd<Scale,8>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_14<1,std::int32_t>());
static_assert(check_14<2,std::int32_t>());
static_assert(check_14<4,std::int32_t>());
static_assert(check_14<8,std::int32_t>());
static_assert(check_14<1,std::uint32_t>());
static_assert(check_14<2,std::uint32_t>());
static_assert(check_14<4,std::uint32_t>());
static_assert(check_14<8,std::uint32_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_15(int seed = 0) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,8> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdd<Scale>(value_type(source), native::simd<std::int32_t,8,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_15<1,std::int32_t>());
static_assert(check_15<2,std::int32_t>());
static_assert(check_15<4,std::int32_t>());
static_assert(check_15<8,std::int32_t>());
static_assert(check_15<1,std::uint32_t>());
static_assert(check_15<2,std::uint32_t>());
static_assert(check_15<4,std::uint32_t>());
static_assert(check_15<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_15(T * base) {
  using value_type = native::simd<T,8,profile_0>;
  using index_type = native::simd<std::int32_t,8,profile_0>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,8> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::simd<std::int32_t,8,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::simd<std::int32_t,8,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_16(int seed = 0) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdd<Scale>(value_type(source), native::predicate<8,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_16<1,std::int32_t>());
static_assert(check_16<2,std::int32_t>());
static_assert(check_16<4,std::int32_t>());
static_assert(check_16<8,std::int32_t>());
static_assert(check_16<1,std::uint32_t>());
static_assert(check_16<2,std::uint32_t>());
static_assert(check_16<4,std::uint32_t>());
static_assert(check_16<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_16(T * base) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<8,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<8,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_17(int seed = 0) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdd<Scale>(memory.data()+40, native::predicate<8,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_17<1,std::int32_t>());
static_assert(check_17<2,std::int32_t>());
static_assert(check_17<4,std::int32_t>());
static_assert(check_17<8,std::int32_t>());
static_assert(check_17<1,std::uint32_t>());
static_assert(check_17<2,std::uint32_t>());
static_assert(check_17<4,std::uint32_t>());
static_assert(check_17<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_17(T * base) {
  using value_type = native::simd<T,8,profile_2>;
  using index_type = native::simd<std::int32_t,8,profile_2>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdd<sizeof(T)>(base, native::predicate<8,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_18(int seed = 0) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<T,96> memory{};
  std::array<T,16> source{};
  std::array<std::int32_t,16> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdd<Scale>(value_type(source), native::predicate<16,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 16 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_18<1,std::int32_t>());
static_assert(check_18<2,std::int32_t>());
static_assert(check_18<4,std::int32_t>());
static_assert(check_18<8,std::int32_t>());
static_assert(check_18<1,std::uint32_t>());
static_assert(check_18<2,std::uint32_t>());
static_assert(check_18<4,std::uint32_t>());
static_assert(check_18<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_18(T * base) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<std::int32_t,16> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,16> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<16,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdd<sizeof(T)>(value_type(source), native::predicate<16,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<16; ++i) {
    if (result[i] != (i >= 16 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 16 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_19(int seed = 0) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<T,96> memory{};
  std::array<T,16> source{};
  std::array<std::int32_t,16> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<16; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdd<Scale>(memory.data()+40, native::predicate<16,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_19<1,std::int32_t>());
static_assert(check_19<2,std::int32_t>());
static_assert(check_19<4,std::int32_t>());
static_assert(check_19<8,std::int32_t>());
static_assert(check_19<1,std::uint32_t>());
static_assert(check_19<2,std::uint32_t>());
static_assert(check_19<4,std::uint32_t>());
static_assert(check_19<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_19(T * base) {
  using value_type = native::simd<T,16,profile_1>;
  using index_type = native::simd<std::int32_t,16,profile_1>;
  std::array<std::int32_t,16> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,16> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdd<sizeof(T)>(base, native::predicate<16,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<16,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_20(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherdpd<Scale,2>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_20<1,double>());
static_assert(check_20<2,double>());
static_assert(check_20<4,double>());
static_assert(check_20<8,double>());

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_21(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,2> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdpd<Scale>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_21<1,double>());
static_assert(check_21<2,double>());
static_assert(check_21<4,double>());
static_assert(check_21<8,double>());

template<class T>
native_noinline native_target("avx2")
bool guard_21(T * base) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,2> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_22(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdpd<Scale>(value_type(source), native::predicate<2,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_22<1,double>());
static_assert(check_22<2,double>());
static_assert(check_22<4,double>());
static_assert(check_22<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_22(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_23(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdpd<Scale>(memory.data()+40, native::predicate<2,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_23<1,double>());
static_assert(check_23<2,double>());
static_assert(check_23<4,double>());
static_assert(check_23<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_23(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdpd<sizeof(T)>(base, native::predicate<2,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<2,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_24(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherdpd<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_24<1,double>());
static_assert(check_24<2,double>());
static_assert(check_24<4,double>());
static_assert(check_24<8,double>());

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_25(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdpd<Scale>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_25<1,double>());
static_assert(check_25<2,double>());
static_assert(check_25<4,double>());
static_assert(check_25<8,double>());

template<class T>
native_noinline native_target("avx2")
bool guard_25(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_26(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdpd<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_26<1,double>());
static_assert(check_26<2,double>());
static_assert(check_26<4,double>());
static_assert(check_26<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_26(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_27(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdpd<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_27<1,double>());
static_assert(check_27<2,double>());
static_assert(check_27<4,double>());
static_assert(check_27<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_27(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdpd<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx512f")
constexpr bool check_28(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherdpd<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_28<1,double>());
static_assert(check_28<2,double>());
static_assert(check_28<4,double>());
static_assert(check_28<8,double>());

template<class T>
native_noinline native_target("avx512f")
bool guard_28(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherdpd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f")
constexpr bool check_29(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterdpd<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_29<1,double>());
static_assert(check_29<2,double>());
static_assert(check_29<4,double>());
static_assert(check_29<8,double>());

template<class T>
native_noinline native_target("avx512f")
bool guard_29(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterdpd<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterdpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_30(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherdq<Scale,2>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_30<1,std::int64_t>());
static_assert(check_30<2,std::int64_t>());
static_assert(check_30<4,std::int64_t>());
static_assert(check_30<8,std::int64_t>());
static_assert(check_30<1,std::uint64_t>());
static_assert(check_30<2,std::uint64_t>());
static_assert(check_30<4,std::uint64_t>());
static_assert(check_30<8,std::uint64_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_31(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,2> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdq<Scale>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_31<1,std::int64_t>());
static_assert(check_31<2,std::int64_t>());
static_assert(check_31<4,std::int64_t>());
static_assert(check_31<8,std::int64_t>());
static_assert(check_31<1,std::uint64_t>());
static_assert(check_31<2,std::uint64_t>());
static_assert(check_31<4,std::uint64_t>());
static_assert(check_31<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_31(T * base) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,2> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_32(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdq<Scale>(value_type(source), native::predicate<2,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_32<1,std::int64_t>());
static_assert(check_32<2,std::int64_t>());
static_assert(check_32<4,std::int64_t>());
static_assert(check_32<8,std::int64_t>());
static_assert(check_32<1,std::uint64_t>());
static_assert(check_32<2,std::uint64_t>());
static_assert(check_32<4,std::uint64_t>());
static_assert(check_32<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_32(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_33(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (std::size_t i=2; i<offsets.size(); ++i) offsets[i] = std::numeric_limits<std::int32_t>::max();
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdq<Scale>(memory.data()+40, native::predicate<2,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_33<1,std::int64_t>());
static_assert(check_33<2,std::int64_t>());
static_assert(check_33<4,std::int64_t>());
static_assert(check_33<8,std::int64_t>());
static_assert(check_33<1,std::uint64_t>());
static_assert(check_33<2,std::uint64_t>());
static_assert(check_33<4,std::uint64_t>());
static_assert(check_33<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_33(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdq<sizeof(T)>(base, native::predicate<2,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<2,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_34(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherdq<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_34<1,std::int64_t>());
static_assert(check_34<2,std::int64_t>());
static_assert(check_34<4,std::int64_t>());
static_assert(check_34<8,std::int64_t>());
static_assert(check_34<1,std::uint64_t>());
static_assert(check_34<2,std::uint64_t>());
static_assert(check_34<4,std::uint64_t>());
static_assert(check_34<8,std::uint64_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_35(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdq<Scale>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_35<1,std::int64_t>());
static_assert(check_35<2,std::int64_t>());
static_assert(check_35<4,std::int64_t>());
static_assert(check_35<8,std::int64_t>());
static_assert(check_35<1,std::uint64_t>());
static_assert(check_35<2,std::uint64_t>());
static_assert(check_35<4,std::uint64_t>());
static_assert(check_35<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_35(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int32_t,4,profile_0>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_36(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdq<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_36<1,std::int64_t>());
static_assert(check_36<2,std::int64_t>());
static_assert(check_36<4,std::int64_t>());
static_assert(check_36<8,std::int64_t>());
static_assert(check_36<1,std::uint64_t>());
static_assert(check_36<2,std::uint64_t>());
static_assert(check_36<4,std::uint64_t>());
static_assert(check_36<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_36(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_37(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int32_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdq<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_37<1,std::int64_t>());
static_assert(check_37<2,std::int64_t>());
static_assert(check_37<4,std::int64_t>());
static_assert(check_37<8,std::int64_t>());
static_assert(check_37<1,std::uint64_t>());
static_assert(check_37<2,std::uint64_t>());
static_assert(check_37<4,std::uint64_t>());
static_assert(check_37<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_37(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int32_t,4,profile_2>;
  std::array<std::int32_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdq<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_38(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherdq<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_38<1,std::int64_t>());
static_assert(check_38<2,std::int64_t>());
static_assert(check_38<4,std::int64_t>());
static_assert(check_38<8,std::int64_t>());
static_assert(check_38<1,std::uint64_t>());
static_assert(check_38<2,std::uint64_t>());
static_assert(check_38<4,std::uint64_t>());
static_assert(check_38<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_38(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherdq<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_39(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int32_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int32_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterdq<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_39<1,std::int64_t>());
static_assert(check_39<2,std::int64_t>());
static_assert(check_39<4,std::int64_t>());
static_assert(check_39<8,std::int64_t>());
static_assert(check_39<1,std::uint64_t>());
static_assert(check_39<2,std::uint64_t>());
static_assert(check_39<4,std::uint64_t>());
static_assert(check_39<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_39(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int32_t,8,profile_1>;
  std::array<std::int32_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterdq<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterdq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_40(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherqps<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_40<1,float>());
static_assert(check_40<2,float>());
static_assert(check_40<4,float>());
static_assert(check_40<8,float>());

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_41(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqps<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_41<1,float>());
static_assert(check_41<2,float>());
static_assert(check_41<4,float>());
static_assert(check_41<8,float>());

template<class T>
native_noinline native_target("avx2")
bool guard_41(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_42(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqps<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_42<1,float>());
static_assert(check_42<2,float>());
static_assert(check_42<4,float>());
static_assert(check_42<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_42(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_43(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqps<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_43<1,float>());
static_assert(check_43<2,float>());
static_assert(check_43<4,float>());
static_assert(check_43<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_43(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqps<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_44(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherqps<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_44<1,float>());
static_assert(check_44<2,float>());
static_assert(check_44<4,float>());
static_assert(check_44<8,float>());

template<int Scale, class T = float>
native_noinline native_target("avx2")
constexpr bool check_45(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqps<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_45<1,float>());
static_assert(check_45<2,float>());
static_assert(check_45<4,float>());
static_assert(check_45<8,float>());

template<class T>
native_noinline native_target("avx2")
bool guard_45(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_46(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqps<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_46<1,float>());
static_assert(check_46<2,float>());
static_assert(check_46<4,float>());
static_assert(check_46<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_46(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_47(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqps<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_47<1,float>());
static_assert(check_47<2,float>());
static_assert(check_47<4,float>());
static_assert(check_47<8,float>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_47(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqps<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = float>
native_noinline native_target("avx512f")
constexpr bool check_48(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqps<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_48<1,float>());
static_assert(check_48<2,float>());
static_assert(check_48<4,float>());
static_assert(check_48<8,float>());

template<class T>
native_noinline native_target("avx512f")
bool guard_48(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqps<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = float>
native_noinline native_target("avx512f")
constexpr bool check_49(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqps<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_49<1,float>());
static_assert(check_49<2,float>());
static_assert(check_49<4,float>());
static_assert(check_49<8,float>());

template<class T>
native_noinline native_target("avx512f")
bool guard_49(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqps<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqps<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_50(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherqd<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_50<1,std::int32_t>());
static_assert(check_50<2,std::int32_t>());
static_assert(check_50<4,std::int32_t>());
static_assert(check_50<8,std::int32_t>());
static_assert(check_50<1,std::uint32_t>());
static_assert(check_50<2,std::uint32_t>());
static_assert(check_50<4,std::uint32_t>());
static_assert(check_50<8,std::uint32_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_51(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqd<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_51<1,std::int32_t>());
static_assert(check_51<2,std::int32_t>());
static_assert(check_51<4,std::int32_t>());
static_assert(check_51<8,std::int32_t>());
static_assert(check_51<1,std::uint32_t>());
static_assert(check_51<2,std::uint32_t>());
static_assert(check_51<4,std::uint32_t>());
static_assert(check_51<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_51(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_52(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqd<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_52<1,std::int32_t>());
static_assert(check_52<2,std::int32_t>());
static_assert(check_52<4,std::int32_t>());
static_assert(check_52<8,std::int32_t>());
static_assert(check_52<1,std::uint32_t>());
static_assert(check_52<2,std::uint32_t>());
static_assert(check_52<4,std::uint32_t>());
static_assert(check_52<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_52(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_53(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqd<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_53<1,std::int32_t>());
static_assert(check_53<2,std::int32_t>());
static_assert(check_53<4,std::int32_t>());
static_assert(check_53<8,std::int32_t>());
static_assert(check_53<1,std::uint32_t>());
static_assert(check_53<2,std::uint32_t>());
static_assert(check_53<4,std::uint32_t>());
static_assert(check_53<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_53(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqd<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_54(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherqd<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_54<1,std::int32_t>());
static_assert(check_54<2,std::int32_t>());
static_assert(check_54<4,std::int32_t>());
static_assert(check_54<8,std::int32_t>());
static_assert(check_54<1,std::uint32_t>());
static_assert(check_54<2,std::uint32_t>());
static_assert(check_54<4,std::uint32_t>());
static_assert(check_54<8,std::uint32_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_55(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int32_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int32_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqd<Scale>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_55<1,std::int32_t>());
static_assert(check_55<2,std::int32_t>());
static_assert(check_55<4,std::int32_t>());
static_assert(check_55<8,std::int32_t>());
static_assert(check_55<1,std::uint32_t>());
static_assert(check_55<2,std::uint32_t>());
static_assert(check_55<4,std::uint32_t>());
static_assert(check_55<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_55(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int32_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::simd<std::int32_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_56(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqd<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_56<1,std::int32_t>());
static_assert(check_56<2,std::int32_t>());
static_assert(check_56<4,std::int32_t>());
static_assert(check_56<8,std::int32_t>());
static_assert(check_56<1,std::uint32_t>());
static_assert(check_56<2,std::uint32_t>());
static_assert(check_56<4,std::uint32_t>());
static_assert(check_56<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_56(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_57(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqd<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_57<1,std::int32_t>());
static_assert(check_57<2,std::int32_t>());
static_assert(check_57<4,std::int32_t>());
static_assert(check_57<8,std::int32_t>());
static_assert(check_57<1,std::uint32_t>());
static_assert(check_57<2,std::uint32_t>());
static_assert(check_57<4,std::uint32_t>());
static_assert(check_57<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_57(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqd<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_58(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqd<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_58<1,std::int32_t>());
static_assert(check_58<2,std::int32_t>());
static_assert(check_58<4,std::int32_t>());
static_assert(check_58<8,std::int32_t>());
static_assert(check_58<1,std::uint32_t>());
static_assert(check_58<2,std::uint32_t>());
static_assert(check_58<4,std::uint32_t>());
static_assert(check_58<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_58(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_59(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqd<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_59<1,std::int32_t>());
static_assert(check_59<2,std::int32_t>());
static_assert(check_59<4,std::int32_t>());
static_assert(check_59<8,std::int32_t>());
static_assert(check_59<1,std::uint32_t>());
static_assert(check_59<2,std::uint32_t>());
static_assert(check_59<4,std::uint32_t>());
static_assert(check_59<8,std::uint32_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_59(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqd<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_60(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherqpd<Scale,2>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_60<1,double>());
static_assert(check_60<2,double>());
static_assert(check_60<4,double>());
static_assert(check_60<8,double>());

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_61(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,2> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqpd<Scale>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_61<1,double>());
static_assert(check_61<2,double>());
static_assert(check_61<4,double>());
static_assert(check_61<8,double>());

template<class T>
native_noinline native_target("avx2")
bool guard_61(T * base) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,2> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_62(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqpd<Scale>(value_type(source), native::predicate<2,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_62<1,double>());
static_assert(check_62<2,double>());
static_assert(check_62<4,double>());
static_assert(check_62<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_62(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_63(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqpd<Scale>(memory.data()+40, native::predicate<2,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_63<1,double>());
static_assert(check_63<2,double>());
static_assert(check_63<4,double>());
static_assert(check_63<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_63(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqpd<sizeof(T)>(base, native::predicate<2,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<2,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_64(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vgatherqpd<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_64<1,double>());
static_assert(check_64<2,double>());
static_assert(check_64<4,double>());
static_assert(check_64<8,double>());

template<int Scale, class T = double>
native_noinline native_target("avx2")
constexpr bool check_65(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqpd<Scale>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_65<1,double>());
static_assert(check_65<2,double>());
static_assert(check_65<4,double>());
static_assert(check_65<8,double>());

template<class T>
native_noinline native_target("avx2")
bool guard_65(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_66(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqpd<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_66<1,double>());
static_assert(check_66<2,double>());
static_assert(check_66<4,double>());
static_assert(check_66<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_66(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_67(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqpd<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_67<1,double>());
static_assert(check_67<2,double>());
static_assert(check_67<4,double>());
static_assert(check_67<8,double>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_67(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqpd<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T = double>
native_noinline native_target("avx512f")
constexpr bool check_68(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vgatherqpd<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_68<1,double>());
static_assert(check_68<2,double>());
static_assert(check_68<4,double>());
static_assert(check_68<8,double>());

template<class T>
native_noinline native_target("avx512f")
bool guard_68(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vgatherqpd<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T = double>
native_noinline native_target("avx512f")
constexpr bool check_69(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vscatterqpd<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_69<1,double>());
static_assert(check_69<2,double>());
static_assert(check_69<4,double>());
static_assert(check_69<8,double>());

template<class T>
native_noinline native_target("avx512f")
bool guard_69(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vscatterqpd<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vscatterqpd<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_70(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherqq<Scale,2>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_70<1,std::int64_t>());
static_assert(check_70<2,std::int64_t>());
static_assert(check_70<4,std::int64_t>());
static_assert(check_70<8,std::int64_t>());
static_assert(check_70<1,std::uint64_t>());
static_assert(check_70<2,std::uint64_t>());
static_assert(check_70<4,std::uint64_t>());
static_assert(check_70<8,std::uint64_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_71(int seed = 0) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,2> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqq<Scale>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_71<1,std::int64_t>());
static_assert(check_71<2,std::int64_t>());
static_assert(check_71<4,std::int64_t>());
static_assert(check_71<8,std::int64_t>());
static_assert(check_71<1,std::uint64_t>());
static_assert(check_71<2,std::uint64_t>());
static_assert(check_71<4,std::uint64_t>());
static_assert(check_71<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_71(T * base) {
  using value_type = native::simd<T,2,profile_0>;
  using index_type = native::simd<std::int64_t,2,profile_0>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,2> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::simd<std::int64_t,2,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_72(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqq<Scale>(value_type(source), native::predicate<2,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 2 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_72<1,std::int64_t>());
static_assert(check_72<2,std::int64_t>());
static_assert(check_72<4,std::int64_t>());
static_assert(check_72<8,std::int64_t>());
static_assert(check_72<1,std::uint64_t>());
static_assert(check_72<2,std::uint64_t>());
static_assert(check_72<4,std::uint64_t>());
static_assert(check_72<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_72(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<2,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<2; ++i) {
    if (result[i] != (i >= 2 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 2 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_73(int seed = 0) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<T,96> memory{};
  std::array<T,2> source{};
  std::array<std::int64_t,2> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<2; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqq<Scale>(memory.data()+40, native::predicate<2,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_73<1,std::int64_t>());
static_assert(check_73<2,std::int64_t>());
static_assert(check_73<4,std::int64_t>());
static_assert(check_73<8,std::int64_t>());
static_assert(check_73<1,std::uint64_t>());
static_assert(check_73<2,std::uint64_t>());
static_assert(check_73<4,std::uint64_t>());
static_assert(check_73<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_73(T * base) {
  using value_type = native::simd<T,2,profile_2>;
  using index_type = native::simd<std::int64_t,2,profile_2>;
  std::array<std::int64_t,2> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,2> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqq<sizeof(T)>(base, native::predicate<2,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<2,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_74(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    bits = ~std::uint64_t{0};
    std::array<T,value_type::lanes> result{};
    native::vpgatherqq<Scale,4>(memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_74<1,std::int64_t>());
static_assert(check_74<2,std::int64_t>());
static_assert(check_74<4,std::int64_t>());
static_assert(check_74<8,std::int64_t>());
static_assert(check_74<1,std::uint64_t>());
static_assert(check_74<2,std::uint64_t>());
static_assert(check_74<4,std::uint64_t>());
static_assert(check_74<8,std::uint64_t>());

template<int Scale, class T>
native_noinline native_target("avx2")
constexpr bool check_75(int seed = 0) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<std::int64_t,4> mask_lanes{};
    for (std::size_t i=0; i<mask_lanes.size(); ++i) mask_lanes[i] = (bits >> i) & 1 ? std::numeric_limits<std::int64_t>::min() + static_cast<int>(i) : 100 + static_cast<int>(i);
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqq<Scale>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_75<1,std::int64_t>());
static_assert(check_75<2,std::int64_t>());
static_assert(check_75<4,std::int64_t>());
static_assert(check_75<8,std::int64_t>());
static_assert(check_75<1,std::uint64_t>());
static_assert(check_75<2,std::uint64_t>());
static_assert(check_75<4,std::uint64_t>());
static_assert(check_75<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx2")
bool guard_75(T * base) {
  using value_type = native::simd<T,4,profile_0>;
  using index_type = native::simd<std::int64_t,4,profile_0>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<std::int64_t,4> mask_lanes{};
  mask_lanes[0] = -1;
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>(mask_lanes), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::simd<std::int64_t,4,profile_0>{}, static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_76(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqq<Scale>(value_type(source), native::predicate<4,profile_2>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 4 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_76<1,std::int64_t>());
static_assert(check_76<2,std::int64_t>());
static_assert(check_76<4,std::int64_t>());
static_assert(check_76<8,std::int64_t>());
static_assert(check_76<1,std::uint64_t>());
static_assert(check_76<2,std::uint64_t>());
static_assert(check_76<4,std::uint64_t>());
static_assert(check_76<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_76(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<4,profile_2>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<4; ++i) {
    if (result[i] != (i >= 4 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 4 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f,avx512vl")
constexpr bool check_77(int seed = 0) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<T,96> memory{};
  std::array<T,4> source{};
  std::array<std::int64_t,4> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<4; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqq<Scale>(memory.data()+40, native::predicate<4,profile_2>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_77<1,std::int64_t>());
static_assert(check_77<2,std::int64_t>());
static_assert(check_77<4,std::int64_t>());
static_assert(check_77<8,std::int64_t>());
static_assert(check_77<1,std::uint64_t>());
static_assert(check_77<2,std::uint64_t>());
static_assert(check_77<4,std::uint64_t>());
static_assert(check_77<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f,avx512vl")
bool guard_77(T * base) {
  using value_type = native::simd<T,4,profile_2>;
  using index_type = native::simd<std::int64_t,4,profile_2>;
  std::array<std::int64_t,4> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,4> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqq<sizeof(T)>(base, native::predicate<4,profile_2>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<4,profile_2>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_78(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    std::array<T,value_type::lanes> result{};
    native::mask_vpgatherqq<Scale>(value_type(source), native::predicate<8,profile_1>::from_bitset(bits), memory.data()+40, index_type(offsets)).store(result.data());
    for (std::size_t i=0; i<result.size(); ++i) {
      T expected = i >= 8 ? T{} : ((bits >> i) & 1) ? memory[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] : source[i];
      if (result[i] != expected) return false;
    }
  }
  return true;
}
static_assert(check_78<1,std::int64_t>());
static_assert(check_78<2,std::int64_t>());
static_assert(check_78<4,std::int64_t>());
static_assert(check_78<8,std::int64_t>());
static_assert(check_78<1,std::uint64_t>());
static_assert(check_78<2,std::uint64_t>());
static_assert(check_78<4,std::uint64_t>());
static_assert(check_78<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_78(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  std::array<T,value_type::lanes> result{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(1), base, index_type(offsets)).store(result.data());
  std::array<T,value_type::lanes> none{};
  native::mask_vpgatherqq<sizeof(T)>(value_type(source), native::predicate<8,profile_1>::from_bitset(0), static_cast<T const *>(nullptr), index_type(offsets)).store(none.data());
  for (std::size_t i=0; i<8; ++i) {
    if (result[i] != (i >= 8 ? T{} : i == 0 ? T{17} : T{33})) return false;
    if (none[i] != (i >= 8 ? T{} : T{33})) return false;
  }
  return true;
}

template<int Scale, class T>
native_noinline native_target("avx512f")
constexpr bool check_79(int seed = 0) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<T,96> memory{};
  std::array<T,8> source{};
  std::array<std::int64_t,8> offsets{};
  for (std::size_t i=0; i<memory.size(); ++i) memory[i] = static_cast<T>(i + 120 + seed);
  for (std::size_t i=0; i<source.size(); ++i) source[i] = static_cast<T>(i + 600 + seed);
  for (std::size_t i=0; i<offsets.size(); ++i) offsets[i] = static_cast<std::int64_t>((static_cast<int>(i) - 4) * (sizeof(T) > Scale ? sizeof(T) / Scale : 1));
  for (unsigned trial=0; trial<4; ++trial) {
    if (trial == 3) offsets.fill(0);
    std::uint64_t bits = trial == 0 ? 0 : trial == 1 ? ~std::uint64_t{0} : 0x5555;
    auto expected = memory;
    for (std::size_t i=0; i<8; ++i) {
      if ((bits >> i) & 1) expected[40 + static_cast<std::ptrdiff_t>(offsets[i]) * Scale / static_cast<std::ptrdiff_t>(sizeof(T))] = source[i];
    }
    native::mask_vpscatterqq<Scale>(memory.data()+40, native::predicate<8,profile_1>::from_bitset(bits), index_type(offsets), value_type(source));
    if (memory != expected) return false;
  }
  return true;
}
static_assert(check_79<1,std::int64_t>());
static_assert(check_79<2,std::int64_t>());
static_assert(check_79<4,std::int64_t>());
static_assert(check_79<8,std::int64_t>());
static_assert(check_79<1,std::uint64_t>());
static_assert(check_79<2,std::uint64_t>());
static_assert(check_79<4,std::uint64_t>());
static_assert(check_79<8,std::uint64_t>());

template<class T>
native_noinline native_target("avx512f")
bool guard_79(T * base) {
  using value_type = native::simd<T,8,profile_1>;
  using index_type = native::simd<std::int64_t,8,profile_1>;
  std::array<std::int64_t,8> offsets{};
  offsets.fill(1);
  offsets[0] = 0;
  std::array<T,8> source{};
  source.fill(T{33});
  *base = T{17};
  native::mask_vpscatterqq<sizeof(T)>(base, native::predicate<8,profile_1>::from_bitset(1), index_type(offsets), value_type(source));
  native::mask_vpscatterqq<sizeof(T)>(static_cast<T *>(nullptr), native::predicate<8,profile_1>::from_bitset(0), index_type(offsets), value_type(source));
  return *base == T{33};
}


// Runtime-only byte addressing uses memcpy oracles so no misaligned C++ lvalue
// is formed. Forward writes independently model partially overlapping stores.
native_noinline native_target("avx2")
bool check_unaligned_avx2() {
  using v = native::simd<std::uint32_t,4,profile_0>;
  using i = native::simd<std::int32_t,4,profile_0>;
  alignas(16) std::uint32_t memory[16]{};
  auto * bytes = reinterpret_cast<unsigned char *>(memory);
  for (unsigned lane=0; lane<sizeof(memory); ++lane) bytes[lane] = static_cast<unsigned char>(lane * 17 + 3);
  std::array<std::int32_t,4> offsets{1,3,7,13};
  std::array<std::uint32_t,4> expected{};
  for (unsigned lane=0; lane<4; ++lane) std::memcpy(&expected[lane],bytes+offsets[lane],4);
  std::array<std::uint32_t,4> result{};
  native::vpgatherdd<1,4>(memory,i(offsets)).store(result.data());
  if (result != expected) return false;
  native::mask_vpgatherdd<1>(v{},i(std::array<std::int32_t,4>{-1,1,-1,0}),memory,i(offsets)).store(result.data());
  return result == std::array<std::uint32_t,4>{expected[0],0,expected[2],0};
}

native_noinline native_target("avx512f,avx512vl")
bool check_unaligned_avx512() {
  using v = native::simd<std::uint32_t,4,profile_2>;
  using i = native::simd<std::int32_t,4,profile_2>;
  using p = native::predicate<4,profile_2>;
  alignas(16) std::uint32_t memory[16]{};
  std::array<unsigned char,sizeof(memory)> expected{};
  auto * bytes = reinterpret_cast<unsigned char *>(memory);
  for (unsigned lane=0; lane<sizeof(memory); ++lane) bytes[lane] = static_cast<unsigned char>(lane * 17 + 3);
  std::memcpy(expected.data(),memory,sizeof(memory));
  std::array<std::int32_t,4> offsets{1,2,3,4};
  std::array<std::uint32_t,4> values{0x01020304,0x11121314,0x21222324,0x31323334};
  // Lane 3 is deliberately inactive: lane 2 supplies the last overlapping bytes.
  for (unsigned lane=0; lane<3; ++lane) std::memcpy(expected.data()+offsets[lane],&values[lane],4);
  native::mask_vpscatterdd<1>(memory,p::from_bitset(7),i(offsets),v(values));
  if (std::memcmp(memory,expected.data(),sizeof(memory))) return false;
  std::array<std::uint32_t,4> gathered{};
  for (unsigned lane=0; lane<4; ++lane) std::memcpy(&gathered[lane],expected.data()+offsets[lane],4);
  std::array<std::uint32_t,4> result{};
  native::mask_vpgatherdd<1>(v{},p::from_bitset(15),memory,i(offsets)).store(result.data());
  if (result != gathered) return false;

  // Exercise qword data with byte offsets and an interior base/negative index.
  using qv = native::simd<std::uint64_t,2,profile_2>;
  using qi = native::simd<std::int64_t,2,profile_2>;
  using qp = native::predicate<2,profile_2>;
  alignas(16) std::uint64_t qmemory[8]{};
  auto * qbytes = reinterpret_cast<unsigned char *>(qmemory);
  std::array<unsigned char,sizeof(qmemory)> qexpected{};
  std::array<std::uint64_t,2> qvalues{0x0102030405060708ull,0x1112131415161718ull};
  std::array<std::int64_t,2> qoffsets{-7,-3};
  for (unsigned lane=0; lane<2; ++lane) std::memcpy(qexpected.data()+16+qoffsets[lane],&qvalues[lane],8);
  native::mask_vpscatterqq<1>(qmemory+2,qp::from_bitset(3),qi(qoffsets),qv(qvalues));
  if (std::memcmp(qbytes,qexpected.data(),sizeof(qmemory))) return false;
  std::array<std::uint64_t,2> qgathered{};
  for (unsigned lane=0; lane<2; ++lane) std::memcpy(&qgathered[lane],qexpected.data()+16+qoffsets[lane],8);
  std::array<std::uint64_t,2> qresult{};
  native::mask_vpgatherqq<1>(qv{},qp::from_bitset(3),qmemory+2,qi(qoffsets)).store(qresult.data());
  return qresult == qgathered;
}


int main(int argc, char **) {
  int seed = argc;
  auto cpu = native::observe_x86_capabilities();
  guarded_page pages;
  if (!pages.allocation) return 2;
  bool ran = false;
  if (native::classify_isa(cpu, profile_0).admitted()) {
    ran = true;
    std::puts("Running AVX2 indexed-memory bank.");
    if (!check_unaligned_avx2()) return 3;
    if (!check_0<1,float>(seed)) { std::puts("FAIL check_0<1,float>(seed)"); return 1; }
    if (!check_0<2,float>(seed)) { std::puts("FAIL check_0<2,float>(seed)"); return 1; }
    if (!check_0<4,float>(seed)) { std::puts("FAIL check_0<4,float>(seed)"); return 1; }
    if (!check_0<8,float>(seed)) { std::puts("FAIL check_0<8,float>(seed)"); return 1; }
    if (!check_1<1,float>(seed)) { std::puts("FAIL check_1<1,float>(seed)"); return 1; }
    if (!check_1<2,float>(seed)) { std::puts("FAIL check_1<2,float>(seed)"); return 1; }
    if (!check_1<4,float>(seed)) { std::puts("FAIL check_1<4,float>(seed)"); return 1; }
    if (!check_1<8,float>(seed)) { std::puts("FAIL check_1<8,float>(seed)"); return 1; }
    if (!check_4<1,float>(seed)) { std::puts("FAIL check_4<1,float>(seed)"); return 1; }
    if (!check_4<2,float>(seed)) { std::puts("FAIL check_4<2,float>(seed)"); return 1; }
    if (!check_4<4,float>(seed)) { std::puts("FAIL check_4<4,float>(seed)"); return 1; }
    if (!check_4<8,float>(seed)) { std::puts("FAIL check_4<8,float>(seed)"); return 1; }
    if (!check_5<1,float>(seed)) { std::puts("FAIL check_5<1,float>(seed)"); return 1; }
    if (!check_5<2,float>(seed)) { std::puts("FAIL check_5<2,float>(seed)"); return 1; }
    if (!check_5<4,float>(seed)) { std::puts("FAIL check_5<4,float>(seed)"); return 1; }
    if (!check_5<8,float>(seed)) { std::puts("FAIL check_5<8,float>(seed)"); return 1; }
    if (!check_10<1,std::int32_t>(seed)) { std::puts("FAIL check_10<1,std::int32_t>(seed)"); return 1; }
    if (!check_10<2,std::int32_t>(seed)) { std::puts("FAIL check_10<2,std::int32_t>(seed)"); return 1; }
    if (!check_10<4,std::int32_t>(seed)) { std::puts("FAIL check_10<4,std::int32_t>(seed)"); return 1; }
    if (!check_10<8,std::int32_t>(seed)) { std::puts("FAIL check_10<8,std::int32_t>(seed)"); return 1; }
    if (!check_10<1,std::uint32_t>(seed)) { std::puts("FAIL check_10<1,std::uint32_t>(seed)"); return 1; }
    if (!check_10<2,std::uint32_t>(seed)) { std::puts("FAIL check_10<2,std::uint32_t>(seed)"); return 1; }
    if (!check_10<4,std::uint32_t>(seed)) { std::puts("FAIL check_10<4,std::uint32_t>(seed)"); return 1; }
    if (!check_10<8,std::uint32_t>(seed)) { std::puts("FAIL check_10<8,std::uint32_t>(seed)"); return 1; }
    if (!check_11<1,std::int32_t>(seed)) { std::puts("FAIL check_11<1,std::int32_t>(seed)"); return 1; }
    if (!check_11<2,std::int32_t>(seed)) { std::puts("FAIL check_11<2,std::int32_t>(seed)"); return 1; }
    if (!check_11<4,std::int32_t>(seed)) { std::puts("FAIL check_11<4,std::int32_t>(seed)"); return 1; }
    if (!check_11<8,std::int32_t>(seed)) { std::puts("FAIL check_11<8,std::int32_t>(seed)"); return 1; }
    if (!check_11<1,std::uint32_t>(seed)) { std::puts("FAIL check_11<1,std::uint32_t>(seed)"); return 1; }
    if (!check_11<2,std::uint32_t>(seed)) { std::puts("FAIL check_11<2,std::uint32_t>(seed)"); return 1; }
    if (!check_11<4,std::uint32_t>(seed)) { std::puts("FAIL check_11<4,std::uint32_t>(seed)"); return 1; }
    if (!check_11<8,std::uint32_t>(seed)) { std::puts("FAIL check_11<8,std::uint32_t>(seed)"); return 1; }
    if (!check_14<1,std::int32_t>(seed)) { std::puts("FAIL check_14<1,std::int32_t>(seed)"); return 1; }
    if (!check_14<2,std::int32_t>(seed)) { std::puts("FAIL check_14<2,std::int32_t>(seed)"); return 1; }
    if (!check_14<4,std::int32_t>(seed)) { std::puts("FAIL check_14<4,std::int32_t>(seed)"); return 1; }
    if (!check_14<8,std::int32_t>(seed)) { std::puts("FAIL check_14<8,std::int32_t>(seed)"); return 1; }
    if (!check_14<1,std::uint32_t>(seed)) { std::puts("FAIL check_14<1,std::uint32_t>(seed)"); return 1; }
    if (!check_14<2,std::uint32_t>(seed)) { std::puts("FAIL check_14<2,std::uint32_t>(seed)"); return 1; }
    if (!check_14<4,std::uint32_t>(seed)) { std::puts("FAIL check_14<4,std::uint32_t>(seed)"); return 1; }
    if (!check_14<8,std::uint32_t>(seed)) { std::puts("FAIL check_14<8,std::uint32_t>(seed)"); return 1; }
    if (!check_15<1,std::int32_t>(seed)) { std::puts("FAIL check_15<1,std::int32_t>(seed)"); return 1; }
    if (!check_15<2,std::int32_t>(seed)) { std::puts("FAIL check_15<2,std::int32_t>(seed)"); return 1; }
    if (!check_15<4,std::int32_t>(seed)) { std::puts("FAIL check_15<4,std::int32_t>(seed)"); return 1; }
    if (!check_15<8,std::int32_t>(seed)) { std::puts("FAIL check_15<8,std::int32_t>(seed)"); return 1; }
    if (!check_15<1,std::uint32_t>(seed)) { std::puts("FAIL check_15<1,std::uint32_t>(seed)"); return 1; }
    if (!check_15<2,std::uint32_t>(seed)) { std::puts("FAIL check_15<2,std::uint32_t>(seed)"); return 1; }
    if (!check_15<4,std::uint32_t>(seed)) { std::puts("FAIL check_15<4,std::uint32_t>(seed)"); return 1; }
    if (!check_15<8,std::uint32_t>(seed)) { std::puts("FAIL check_15<8,std::uint32_t>(seed)"); return 1; }
    if (!check_20<1,double>(seed)) { std::puts("FAIL check_20<1,double>(seed)"); return 1; }
    if (!check_20<2,double>(seed)) { std::puts("FAIL check_20<2,double>(seed)"); return 1; }
    if (!check_20<4,double>(seed)) { std::puts("FAIL check_20<4,double>(seed)"); return 1; }
    if (!check_20<8,double>(seed)) { std::puts("FAIL check_20<8,double>(seed)"); return 1; }
    if (!check_21<1,double>(seed)) { std::puts("FAIL check_21<1,double>(seed)"); return 1; }
    if (!check_21<2,double>(seed)) { std::puts("FAIL check_21<2,double>(seed)"); return 1; }
    if (!check_21<4,double>(seed)) { std::puts("FAIL check_21<4,double>(seed)"); return 1; }
    if (!check_21<8,double>(seed)) { std::puts("FAIL check_21<8,double>(seed)"); return 1; }
    if (!check_24<1,double>(seed)) { std::puts("FAIL check_24<1,double>(seed)"); return 1; }
    if (!check_24<2,double>(seed)) { std::puts("FAIL check_24<2,double>(seed)"); return 1; }
    if (!check_24<4,double>(seed)) { std::puts("FAIL check_24<4,double>(seed)"); return 1; }
    if (!check_24<8,double>(seed)) { std::puts("FAIL check_24<8,double>(seed)"); return 1; }
    if (!check_25<1,double>(seed)) { std::puts("FAIL check_25<1,double>(seed)"); return 1; }
    if (!check_25<2,double>(seed)) { std::puts("FAIL check_25<2,double>(seed)"); return 1; }
    if (!check_25<4,double>(seed)) { std::puts("FAIL check_25<4,double>(seed)"); return 1; }
    if (!check_25<8,double>(seed)) { std::puts("FAIL check_25<8,double>(seed)"); return 1; }
    if (!check_30<1,std::int64_t>(seed)) { std::puts("FAIL check_30<1,std::int64_t>(seed)"); return 1; }
    if (!check_30<2,std::int64_t>(seed)) { std::puts("FAIL check_30<2,std::int64_t>(seed)"); return 1; }
    if (!check_30<4,std::int64_t>(seed)) { std::puts("FAIL check_30<4,std::int64_t>(seed)"); return 1; }
    if (!check_30<8,std::int64_t>(seed)) { std::puts("FAIL check_30<8,std::int64_t>(seed)"); return 1; }
    if (!check_30<1,std::uint64_t>(seed)) { std::puts("FAIL check_30<1,std::uint64_t>(seed)"); return 1; }
    if (!check_30<2,std::uint64_t>(seed)) { std::puts("FAIL check_30<2,std::uint64_t>(seed)"); return 1; }
    if (!check_30<4,std::uint64_t>(seed)) { std::puts("FAIL check_30<4,std::uint64_t>(seed)"); return 1; }
    if (!check_30<8,std::uint64_t>(seed)) { std::puts("FAIL check_30<8,std::uint64_t>(seed)"); return 1; }
    if (!check_31<1,std::int64_t>(seed)) { std::puts("FAIL check_31<1,std::int64_t>(seed)"); return 1; }
    if (!check_31<2,std::int64_t>(seed)) { std::puts("FAIL check_31<2,std::int64_t>(seed)"); return 1; }
    if (!check_31<4,std::int64_t>(seed)) { std::puts("FAIL check_31<4,std::int64_t>(seed)"); return 1; }
    if (!check_31<8,std::int64_t>(seed)) { std::puts("FAIL check_31<8,std::int64_t>(seed)"); return 1; }
    if (!check_31<1,std::uint64_t>(seed)) { std::puts("FAIL check_31<1,std::uint64_t>(seed)"); return 1; }
    if (!check_31<2,std::uint64_t>(seed)) { std::puts("FAIL check_31<2,std::uint64_t>(seed)"); return 1; }
    if (!check_31<4,std::uint64_t>(seed)) { std::puts("FAIL check_31<4,std::uint64_t>(seed)"); return 1; }
    if (!check_31<8,std::uint64_t>(seed)) { std::puts("FAIL check_31<8,std::uint64_t>(seed)"); return 1; }
    if (!check_34<1,std::int64_t>(seed)) { std::puts("FAIL check_34<1,std::int64_t>(seed)"); return 1; }
    if (!check_34<2,std::int64_t>(seed)) { std::puts("FAIL check_34<2,std::int64_t>(seed)"); return 1; }
    if (!check_34<4,std::int64_t>(seed)) { std::puts("FAIL check_34<4,std::int64_t>(seed)"); return 1; }
    if (!check_34<8,std::int64_t>(seed)) { std::puts("FAIL check_34<8,std::int64_t>(seed)"); return 1; }
    if (!check_34<1,std::uint64_t>(seed)) { std::puts("FAIL check_34<1,std::uint64_t>(seed)"); return 1; }
    if (!check_34<2,std::uint64_t>(seed)) { std::puts("FAIL check_34<2,std::uint64_t>(seed)"); return 1; }
    if (!check_34<4,std::uint64_t>(seed)) { std::puts("FAIL check_34<4,std::uint64_t>(seed)"); return 1; }
    if (!check_34<8,std::uint64_t>(seed)) { std::puts("FAIL check_34<8,std::uint64_t>(seed)"); return 1; }
    if (!check_35<1,std::int64_t>(seed)) { std::puts("FAIL check_35<1,std::int64_t>(seed)"); return 1; }
    if (!check_35<2,std::int64_t>(seed)) { std::puts("FAIL check_35<2,std::int64_t>(seed)"); return 1; }
    if (!check_35<4,std::int64_t>(seed)) { std::puts("FAIL check_35<4,std::int64_t>(seed)"); return 1; }
    if (!check_35<8,std::int64_t>(seed)) { std::puts("FAIL check_35<8,std::int64_t>(seed)"); return 1; }
    if (!check_35<1,std::uint64_t>(seed)) { std::puts("FAIL check_35<1,std::uint64_t>(seed)"); return 1; }
    if (!check_35<2,std::uint64_t>(seed)) { std::puts("FAIL check_35<2,std::uint64_t>(seed)"); return 1; }
    if (!check_35<4,std::uint64_t>(seed)) { std::puts("FAIL check_35<4,std::uint64_t>(seed)"); return 1; }
    if (!check_35<8,std::uint64_t>(seed)) { std::puts("FAIL check_35<8,std::uint64_t>(seed)"); return 1; }
    if (!check_40<1,float>(seed)) { std::puts("FAIL check_40<1,float>(seed)"); return 1; }
    if (!check_40<2,float>(seed)) { std::puts("FAIL check_40<2,float>(seed)"); return 1; }
    if (!check_40<4,float>(seed)) { std::puts("FAIL check_40<4,float>(seed)"); return 1; }
    if (!check_40<8,float>(seed)) { std::puts("FAIL check_40<8,float>(seed)"); return 1; }
    if (!check_41<1,float>(seed)) { std::puts("FAIL check_41<1,float>(seed)"); return 1; }
    if (!check_41<2,float>(seed)) { std::puts("FAIL check_41<2,float>(seed)"); return 1; }
    if (!check_41<4,float>(seed)) { std::puts("FAIL check_41<4,float>(seed)"); return 1; }
    if (!check_41<8,float>(seed)) { std::puts("FAIL check_41<8,float>(seed)"); return 1; }
    if (!check_44<1,float>(seed)) { std::puts("FAIL check_44<1,float>(seed)"); return 1; }
    if (!check_44<2,float>(seed)) { std::puts("FAIL check_44<2,float>(seed)"); return 1; }
    if (!check_44<4,float>(seed)) { std::puts("FAIL check_44<4,float>(seed)"); return 1; }
    if (!check_44<8,float>(seed)) { std::puts("FAIL check_44<8,float>(seed)"); return 1; }
    if (!check_45<1,float>(seed)) { std::puts("FAIL check_45<1,float>(seed)"); return 1; }
    if (!check_45<2,float>(seed)) { std::puts("FAIL check_45<2,float>(seed)"); return 1; }
    if (!check_45<4,float>(seed)) { std::puts("FAIL check_45<4,float>(seed)"); return 1; }
    if (!check_45<8,float>(seed)) { std::puts("FAIL check_45<8,float>(seed)"); return 1; }
    if (!check_50<1,std::int32_t>(seed)) { std::puts("FAIL check_50<1,std::int32_t>(seed)"); return 1; }
    if (!check_50<2,std::int32_t>(seed)) { std::puts("FAIL check_50<2,std::int32_t>(seed)"); return 1; }
    if (!check_50<4,std::int32_t>(seed)) { std::puts("FAIL check_50<4,std::int32_t>(seed)"); return 1; }
    if (!check_50<8,std::int32_t>(seed)) { std::puts("FAIL check_50<8,std::int32_t>(seed)"); return 1; }
    if (!check_50<1,std::uint32_t>(seed)) { std::puts("FAIL check_50<1,std::uint32_t>(seed)"); return 1; }
    if (!check_50<2,std::uint32_t>(seed)) { std::puts("FAIL check_50<2,std::uint32_t>(seed)"); return 1; }
    if (!check_50<4,std::uint32_t>(seed)) { std::puts("FAIL check_50<4,std::uint32_t>(seed)"); return 1; }
    if (!check_50<8,std::uint32_t>(seed)) { std::puts("FAIL check_50<8,std::uint32_t>(seed)"); return 1; }
    if (!check_51<1,std::int32_t>(seed)) { std::puts("FAIL check_51<1,std::int32_t>(seed)"); return 1; }
    if (!check_51<2,std::int32_t>(seed)) { std::puts("FAIL check_51<2,std::int32_t>(seed)"); return 1; }
    if (!check_51<4,std::int32_t>(seed)) { std::puts("FAIL check_51<4,std::int32_t>(seed)"); return 1; }
    if (!check_51<8,std::int32_t>(seed)) { std::puts("FAIL check_51<8,std::int32_t>(seed)"); return 1; }
    if (!check_51<1,std::uint32_t>(seed)) { std::puts("FAIL check_51<1,std::uint32_t>(seed)"); return 1; }
    if (!check_51<2,std::uint32_t>(seed)) { std::puts("FAIL check_51<2,std::uint32_t>(seed)"); return 1; }
    if (!check_51<4,std::uint32_t>(seed)) { std::puts("FAIL check_51<4,std::uint32_t>(seed)"); return 1; }
    if (!check_51<8,std::uint32_t>(seed)) { std::puts("FAIL check_51<8,std::uint32_t>(seed)"); return 1; }
    if (!check_54<1,std::int32_t>(seed)) { std::puts("FAIL check_54<1,std::int32_t>(seed)"); return 1; }
    if (!check_54<2,std::int32_t>(seed)) { std::puts("FAIL check_54<2,std::int32_t>(seed)"); return 1; }
    if (!check_54<4,std::int32_t>(seed)) { std::puts("FAIL check_54<4,std::int32_t>(seed)"); return 1; }
    if (!check_54<8,std::int32_t>(seed)) { std::puts("FAIL check_54<8,std::int32_t>(seed)"); return 1; }
    if (!check_54<1,std::uint32_t>(seed)) { std::puts("FAIL check_54<1,std::uint32_t>(seed)"); return 1; }
    if (!check_54<2,std::uint32_t>(seed)) { std::puts("FAIL check_54<2,std::uint32_t>(seed)"); return 1; }
    if (!check_54<4,std::uint32_t>(seed)) { std::puts("FAIL check_54<4,std::uint32_t>(seed)"); return 1; }
    if (!check_54<8,std::uint32_t>(seed)) { std::puts("FAIL check_54<8,std::uint32_t>(seed)"); return 1; }
    if (!check_55<1,std::int32_t>(seed)) { std::puts("FAIL check_55<1,std::int32_t>(seed)"); return 1; }
    if (!check_55<2,std::int32_t>(seed)) { std::puts("FAIL check_55<2,std::int32_t>(seed)"); return 1; }
    if (!check_55<4,std::int32_t>(seed)) { std::puts("FAIL check_55<4,std::int32_t>(seed)"); return 1; }
    if (!check_55<8,std::int32_t>(seed)) { std::puts("FAIL check_55<8,std::int32_t>(seed)"); return 1; }
    if (!check_55<1,std::uint32_t>(seed)) { std::puts("FAIL check_55<1,std::uint32_t>(seed)"); return 1; }
    if (!check_55<2,std::uint32_t>(seed)) { std::puts("FAIL check_55<2,std::uint32_t>(seed)"); return 1; }
    if (!check_55<4,std::uint32_t>(seed)) { std::puts("FAIL check_55<4,std::uint32_t>(seed)"); return 1; }
    if (!check_55<8,std::uint32_t>(seed)) { std::puts("FAIL check_55<8,std::uint32_t>(seed)"); return 1; }
    if (!check_60<1,double>(seed)) { std::puts("FAIL check_60<1,double>(seed)"); return 1; }
    if (!check_60<2,double>(seed)) { std::puts("FAIL check_60<2,double>(seed)"); return 1; }
    if (!check_60<4,double>(seed)) { std::puts("FAIL check_60<4,double>(seed)"); return 1; }
    if (!check_60<8,double>(seed)) { std::puts("FAIL check_60<8,double>(seed)"); return 1; }
    if (!check_61<1,double>(seed)) { std::puts("FAIL check_61<1,double>(seed)"); return 1; }
    if (!check_61<2,double>(seed)) { std::puts("FAIL check_61<2,double>(seed)"); return 1; }
    if (!check_61<4,double>(seed)) { std::puts("FAIL check_61<4,double>(seed)"); return 1; }
    if (!check_61<8,double>(seed)) { std::puts("FAIL check_61<8,double>(seed)"); return 1; }
    if (!check_64<1,double>(seed)) { std::puts("FAIL check_64<1,double>(seed)"); return 1; }
    if (!check_64<2,double>(seed)) { std::puts("FAIL check_64<2,double>(seed)"); return 1; }
    if (!check_64<4,double>(seed)) { std::puts("FAIL check_64<4,double>(seed)"); return 1; }
    if (!check_64<8,double>(seed)) { std::puts("FAIL check_64<8,double>(seed)"); return 1; }
    if (!check_65<1,double>(seed)) { std::puts("FAIL check_65<1,double>(seed)"); return 1; }
    if (!check_65<2,double>(seed)) { std::puts("FAIL check_65<2,double>(seed)"); return 1; }
    if (!check_65<4,double>(seed)) { std::puts("FAIL check_65<4,double>(seed)"); return 1; }
    if (!check_65<8,double>(seed)) { std::puts("FAIL check_65<8,double>(seed)"); return 1; }
    if (!check_70<1,std::int64_t>(seed)) { std::puts("FAIL check_70<1,std::int64_t>(seed)"); return 1; }
    if (!check_70<2,std::int64_t>(seed)) { std::puts("FAIL check_70<2,std::int64_t>(seed)"); return 1; }
    if (!check_70<4,std::int64_t>(seed)) { std::puts("FAIL check_70<4,std::int64_t>(seed)"); return 1; }
    if (!check_70<8,std::int64_t>(seed)) { std::puts("FAIL check_70<8,std::int64_t>(seed)"); return 1; }
    if (!check_70<1,std::uint64_t>(seed)) { std::puts("FAIL check_70<1,std::uint64_t>(seed)"); return 1; }
    if (!check_70<2,std::uint64_t>(seed)) { std::puts("FAIL check_70<2,std::uint64_t>(seed)"); return 1; }
    if (!check_70<4,std::uint64_t>(seed)) { std::puts("FAIL check_70<4,std::uint64_t>(seed)"); return 1; }
    if (!check_70<8,std::uint64_t>(seed)) { std::puts("FAIL check_70<8,std::uint64_t>(seed)"); return 1; }
    if (!check_71<1,std::int64_t>(seed)) { std::puts("FAIL check_71<1,std::int64_t>(seed)"); return 1; }
    if (!check_71<2,std::int64_t>(seed)) { std::puts("FAIL check_71<2,std::int64_t>(seed)"); return 1; }
    if (!check_71<4,std::int64_t>(seed)) { std::puts("FAIL check_71<4,std::int64_t>(seed)"); return 1; }
    if (!check_71<8,std::int64_t>(seed)) { std::puts("FAIL check_71<8,std::int64_t>(seed)"); return 1; }
    if (!check_71<1,std::uint64_t>(seed)) { std::puts("FAIL check_71<1,std::uint64_t>(seed)"); return 1; }
    if (!check_71<2,std::uint64_t>(seed)) { std::puts("FAIL check_71<2,std::uint64_t>(seed)"); return 1; }
    if (!check_71<4,std::uint64_t>(seed)) { std::puts("FAIL check_71<4,std::uint64_t>(seed)"); return 1; }
    if (!check_71<8,std::uint64_t>(seed)) { std::puts("FAIL check_71<8,std::uint64_t>(seed)"); return 1; }
    if (!check_74<1,std::int64_t>(seed)) { std::puts("FAIL check_74<1,std::int64_t>(seed)"); return 1; }
    if (!check_74<2,std::int64_t>(seed)) { std::puts("FAIL check_74<2,std::int64_t>(seed)"); return 1; }
    if (!check_74<4,std::int64_t>(seed)) { std::puts("FAIL check_74<4,std::int64_t>(seed)"); return 1; }
    if (!check_74<8,std::int64_t>(seed)) { std::puts("FAIL check_74<8,std::int64_t>(seed)"); return 1; }
    if (!check_74<1,std::uint64_t>(seed)) { std::puts("FAIL check_74<1,std::uint64_t>(seed)"); return 1; }
    if (!check_74<2,std::uint64_t>(seed)) { std::puts("FAIL check_74<2,std::uint64_t>(seed)"); return 1; }
    if (!check_74<4,std::uint64_t>(seed)) { std::puts("FAIL check_74<4,std::uint64_t>(seed)"); return 1; }
    if (!check_74<8,std::uint64_t>(seed)) { std::puts("FAIL check_74<8,std::uint64_t>(seed)"); return 1; }
    if (!check_75<1,std::int64_t>(seed)) { std::puts("FAIL check_75<1,std::int64_t>(seed)"); return 1; }
    if (!check_75<2,std::int64_t>(seed)) { std::puts("FAIL check_75<2,std::int64_t>(seed)"); return 1; }
    if (!check_75<4,std::int64_t>(seed)) { std::puts("FAIL check_75<4,std::int64_t>(seed)"); return 1; }
    if (!check_75<8,std::int64_t>(seed)) { std::puts("FAIL check_75<8,std::int64_t>(seed)"); return 1; }
    if (!check_75<1,std::uint64_t>(seed)) { std::puts("FAIL check_75<1,std::uint64_t>(seed)"); return 1; }
    if (!check_75<2,std::uint64_t>(seed)) { std::puts("FAIL check_75<2,std::uint64_t>(seed)"); return 1; }
    if (!check_75<4,std::uint64_t>(seed)) { std::puts("FAIL check_75<4,std::uint64_t>(seed)"); return 1; }
    if (!check_75<8,std::uint64_t>(seed)) { std::puts("FAIL check_75<8,std::uint64_t>(seed)"); return 1; }
    if (!guard_1(pages.last<float>())) { std::puts("FAIL guard_1(pages.last<float>())"); return 1; }
    if (!guard_5(pages.last<float>())) { std::puts("FAIL guard_5(pages.last<float>())"); return 1; }
    if (!guard_11(pages.last<std::int32_t>())) { std::puts("FAIL guard_11(pages.last<std::int32_t>())"); return 1; }
    if (!guard_11(pages.last<std::uint32_t>())) { std::puts("FAIL guard_11(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_15(pages.last<std::int32_t>())) { std::puts("FAIL guard_15(pages.last<std::int32_t>())"); return 1; }
    if (!guard_15(pages.last<std::uint32_t>())) { std::puts("FAIL guard_15(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_21(pages.last<double>())) { std::puts("FAIL guard_21(pages.last<double>())"); return 1; }
    if (!guard_25(pages.last<double>())) { std::puts("FAIL guard_25(pages.last<double>())"); return 1; }
    if (!guard_31(pages.last<std::int64_t>())) { std::puts("FAIL guard_31(pages.last<std::int64_t>())"); return 1; }
    if (!guard_31(pages.last<std::uint64_t>())) { std::puts("FAIL guard_31(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_35(pages.last<std::int64_t>())) { std::puts("FAIL guard_35(pages.last<std::int64_t>())"); return 1; }
    if (!guard_35(pages.last<std::uint64_t>())) { std::puts("FAIL guard_35(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_41(pages.last<float>())) { std::puts("FAIL guard_41(pages.last<float>())"); return 1; }
    if (!guard_45(pages.last<float>())) { std::puts("FAIL guard_45(pages.last<float>())"); return 1; }
    if (!guard_51(pages.last<std::int32_t>())) { std::puts("FAIL guard_51(pages.last<std::int32_t>())"); return 1; }
    if (!guard_51(pages.last<std::uint32_t>())) { std::puts("FAIL guard_51(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_55(pages.last<std::int32_t>())) { std::puts("FAIL guard_55(pages.last<std::int32_t>())"); return 1; }
    if (!guard_55(pages.last<std::uint32_t>())) { std::puts("FAIL guard_55(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_61(pages.last<double>())) { std::puts("FAIL guard_61(pages.last<double>())"); return 1; }
    if (!guard_65(pages.last<double>())) { std::puts("FAIL guard_65(pages.last<double>())"); return 1; }
    if (!guard_71(pages.last<std::int64_t>())) { std::puts("FAIL guard_71(pages.last<std::int64_t>())"); return 1; }
    if (!guard_71(pages.last<std::uint64_t>())) { std::puts("FAIL guard_71(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_75(pages.last<std::int64_t>())) { std::puts("FAIL guard_75(pages.last<std::int64_t>())"); return 1; }
    if (!guard_75(pages.last<std::uint64_t>())) { std::puts("FAIL guard_75(pages.last<std::uint64_t>())"); return 1; }
  }
  if (native::classify_isa(cpu, profile_1).admitted()) {
    ran = true;
    std::puts("Running AVX512F indexed-memory bank.");
    if (!check_8<1,float>(seed)) { std::puts("FAIL check_8<1,float>(seed)"); return 1; }
    if (!check_8<2,float>(seed)) { std::puts("FAIL check_8<2,float>(seed)"); return 1; }
    if (!check_8<4,float>(seed)) { std::puts("FAIL check_8<4,float>(seed)"); return 1; }
    if (!check_8<8,float>(seed)) { std::puts("FAIL check_8<8,float>(seed)"); return 1; }
    if (!check_9<1,float>(seed)) { std::puts("FAIL check_9<1,float>(seed)"); return 1; }
    if (!check_9<2,float>(seed)) { std::puts("FAIL check_9<2,float>(seed)"); return 1; }
    if (!check_9<4,float>(seed)) { std::puts("FAIL check_9<4,float>(seed)"); return 1; }
    if (!check_9<8,float>(seed)) { std::puts("FAIL check_9<8,float>(seed)"); return 1; }
    if (!check_18<1,std::int32_t>(seed)) { std::puts("FAIL check_18<1,std::int32_t>(seed)"); return 1; }
    if (!check_18<2,std::int32_t>(seed)) { std::puts("FAIL check_18<2,std::int32_t>(seed)"); return 1; }
    if (!check_18<4,std::int32_t>(seed)) { std::puts("FAIL check_18<4,std::int32_t>(seed)"); return 1; }
    if (!check_18<8,std::int32_t>(seed)) { std::puts("FAIL check_18<8,std::int32_t>(seed)"); return 1; }
    if (!check_18<1,std::uint32_t>(seed)) { std::puts("FAIL check_18<1,std::uint32_t>(seed)"); return 1; }
    if (!check_18<2,std::uint32_t>(seed)) { std::puts("FAIL check_18<2,std::uint32_t>(seed)"); return 1; }
    if (!check_18<4,std::uint32_t>(seed)) { std::puts("FAIL check_18<4,std::uint32_t>(seed)"); return 1; }
    if (!check_18<8,std::uint32_t>(seed)) { std::puts("FAIL check_18<8,std::uint32_t>(seed)"); return 1; }
    if (!check_19<1,std::int32_t>(seed)) { std::puts("FAIL check_19<1,std::int32_t>(seed)"); return 1; }
    if (!check_19<2,std::int32_t>(seed)) { std::puts("FAIL check_19<2,std::int32_t>(seed)"); return 1; }
    if (!check_19<4,std::int32_t>(seed)) { std::puts("FAIL check_19<4,std::int32_t>(seed)"); return 1; }
    if (!check_19<8,std::int32_t>(seed)) { std::puts("FAIL check_19<8,std::int32_t>(seed)"); return 1; }
    if (!check_19<1,std::uint32_t>(seed)) { std::puts("FAIL check_19<1,std::uint32_t>(seed)"); return 1; }
    if (!check_19<2,std::uint32_t>(seed)) { std::puts("FAIL check_19<2,std::uint32_t>(seed)"); return 1; }
    if (!check_19<4,std::uint32_t>(seed)) { std::puts("FAIL check_19<4,std::uint32_t>(seed)"); return 1; }
    if (!check_19<8,std::uint32_t>(seed)) { std::puts("FAIL check_19<8,std::uint32_t>(seed)"); return 1; }
    if (!check_28<1,double>(seed)) { std::puts("FAIL check_28<1,double>(seed)"); return 1; }
    if (!check_28<2,double>(seed)) { std::puts("FAIL check_28<2,double>(seed)"); return 1; }
    if (!check_28<4,double>(seed)) { std::puts("FAIL check_28<4,double>(seed)"); return 1; }
    if (!check_28<8,double>(seed)) { std::puts("FAIL check_28<8,double>(seed)"); return 1; }
    if (!check_29<1,double>(seed)) { std::puts("FAIL check_29<1,double>(seed)"); return 1; }
    if (!check_29<2,double>(seed)) { std::puts("FAIL check_29<2,double>(seed)"); return 1; }
    if (!check_29<4,double>(seed)) { std::puts("FAIL check_29<4,double>(seed)"); return 1; }
    if (!check_29<8,double>(seed)) { std::puts("FAIL check_29<8,double>(seed)"); return 1; }
    if (!check_38<1,std::int64_t>(seed)) { std::puts("FAIL check_38<1,std::int64_t>(seed)"); return 1; }
    if (!check_38<2,std::int64_t>(seed)) { std::puts("FAIL check_38<2,std::int64_t>(seed)"); return 1; }
    if (!check_38<4,std::int64_t>(seed)) { std::puts("FAIL check_38<4,std::int64_t>(seed)"); return 1; }
    if (!check_38<8,std::int64_t>(seed)) { std::puts("FAIL check_38<8,std::int64_t>(seed)"); return 1; }
    if (!check_38<1,std::uint64_t>(seed)) { std::puts("FAIL check_38<1,std::uint64_t>(seed)"); return 1; }
    if (!check_38<2,std::uint64_t>(seed)) { std::puts("FAIL check_38<2,std::uint64_t>(seed)"); return 1; }
    if (!check_38<4,std::uint64_t>(seed)) { std::puts("FAIL check_38<4,std::uint64_t>(seed)"); return 1; }
    if (!check_38<8,std::uint64_t>(seed)) { std::puts("FAIL check_38<8,std::uint64_t>(seed)"); return 1; }
    if (!check_39<1,std::int64_t>(seed)) { std::puts("FAIL check_39<1,std::int64_t>(seed)"); return 1; }
    if (!check_39<2,std::int64_t>(seed)) { std::puts("FAIL check_39<2,std::int64_t>(seed)"); return 1; }
    if (!check_39<4,std::int64_t>(seed)) { std::puts("FAIL check_39<4,std::int64_t>(seed)"); return 1; }
    if (!check_39<8,std::int64_t>(seed)) { std::puts("FAIL check_39<8,std::int64_t>(seed)"); return 1; }
    if (!check_39<1,std::uint64_t>(seed)) { std::puts("FAIL check_39<1,std::uint64_t>(seed)"); return 1; }
    if (!check_39<2,std::uint64_t>(seed)) { std::puts("FAIL check_39<2,std::uint64_t>(seed)"); return 1; }
    if (!check_39<4,std::uint64_t>(seed)) { std::puts("FAIL check_39<4,std::uint64_t>(seed)"); return 1; }
    if (!check_39<8,std::uint64_t>(seed)) { std::puts("FAIL check_39<8,std::uint64_t>(seed)"); return 1; }
    if (!check_48<1,float>(seed)) { std::puts("FAIL check_48<1,float>(seed)"); return 1; }
    if (!check_48<2,float>(seed)) { std::puts("FAIL check_48<2,float>(seed)"); return 1; }
    if (!check_48<4,float>(seed)) { std::puts("FAIL check_48<4,float>(seed)"); return 1; }
    if (!check_48<8,float>(seed)) { std::puts("FAIL check_48<8,float>(seed)"); return 1; }
    if (!check_49<1,float>(seed)) { std::puts("FAIL check_49<1,float>(seed)"); return 1; }
    if (!check_49<2,float>(seed)) { std::puts("FAIL check_49<2,float>(seed)"); return 1; }
    if (!check_49<4,float>(seed)) { std::puts("FAIL check_49<4,float>(seed)"); return 1; }
    if (!check_49<8,float>(seed)) { std::puts("FAIL check_49<8,float>(seed)"); return 1; }
    if (!check_58<1,std::int32_t>(seed)) { std::puts("FAIL check_58<1,std::int32_t>(seed)"); return 1; }
    if (!check_58<2,std::int32_t>(seed)) { std::puts("FAIL check_58<2,std::int32_t>(seed)"); return 1; }
    if (!check_58<4,std::int32_t>(seed)) { std::puts("FAIL check_58<4,std::int32_t>(seed)"); return 1; }
    if (!check_58<8,std::int32_t>(seed)) { std::puts("FAIL check_58<8,std::int32_t>(seed)"); return 1; }
    if (!check_58<1,std::uint32_t>(seed)) { std::puts("FAIL check_58<1,std::uint32_t>(seed)"); return 1; }
    if (!check_58<2,std::uint32_t>(seed)) { std::puts("FAIL check_58<2,std::uint32_t>(seed)"); return 1; }
    if (!check_58<4,std::uint32_t>(seed)) { std::puts("FAIL check_58<4,std::uint32_t>(seed)"); return 1; }
    if (!check_58<8,std::uint32_t>(seed)) { std::puts("FAIL check_58<8,std::uint32_t>(seed)"); return 1; }
    if (!check_59<1,std::int32_t>(seed)) { std::puts("FAIL check_59<1,std::int32_t>(seed)"); return 1; }
    if (!check_59<2,std::int32_t>(seed)) { std::puts("FAIL check_59<2,std::int32_t>(seed)"); return 1; }
    if (!check_59<4,std::int32_t>(seed)) { std::puts("FAIL check_59<4,std::int32_t>(seed)"); return 1; }
    if (!check_59<8,std::int32_t>(seed)) { std::puts("FAIL check_59<8,std::int32_t>(seed)"); return 1; }
    if (!check_59<1,std::uint32_t>(seed)) { std::puts("FAIL check_59<1,std::uint32_t>(seed)"); return 1; }
    if (!check_59<2,std::uint32_t>(seed)) { std::puts("FAIL check_59<2,std::uint32_t>(seed)"); return 1; }
    if (!check_59<4,std::uint32_t>(seed)) { std::puts("FAIL check_59<4,std::uint32_t>(seed)"); return 1; }
    if (!check_59<8,std::uint32_t>(seed)) { std::puts("FAIL check_59<8,std::uint32_t>(seed)"); return 1; }
    if (!check_68<1,double>(seed)) { std::puts("FAIL check_68<1,double>(seed)"); return 1; }
    if (!check_68<2,double>(seed)) { std::puts("FAIL check_68<2,double>(seed)"); return 1; }
    if (!check_68<4,double>(seed)) { std::puts("FAIL check_68<4,double>(seed)"); return 1; }
    if (!check_68<8,double>(seed)) { std::puts("FAIL check_68<8,double>(seed)"); return 1; }
    if (!check_69<1,double>(seed)) { std::puts("FAIL check_69<1,double>(seed)"); return 1; }
    if (!check_69<2,double>(seed)) { std::puts("FAIL check_69<2,double>(seed)"); return 1; }
    if (!check_69<4,double>(seed)) { std::puts("FAIL check_69<4,double>(seed)"); return 1; }
    if (!check_69<8,double>(seed)) { std::puts("FAIL check_69<8,double>(seed)"); return 1; }
    if (!check_78<1,std::int64_t>(seed)) { std::puts("FAIL check_78<1,std::int64_t>(seed)"); return 1; }
    if (!check_78<2,std::int64_t>(seed)) { std::puts("FAIL check_78<2,std::int64_t>(seed)"); return 1; }
    if (!check_78<4,std::int64_t>(seed)) { std::puts("FAIL check_78<4,std::int64_t>(seed)"); return 1; }
    if (!check_78<8,std::int64_t>(seed)) { std::puts("FAIL check_78<8,std::int64_t>(seed)"); return 1; }
    if (!check_78<1,std::uint64_t>(seed)) { std::puts("FAIL check_78<1,std::uint64_t>(seed)"); return 1; }
    if (!check_78<2,std::uint64_t>(seed)) { std::puts("FAIL check_78<2,std::uint64_t>(seed)"); return 1; }
    if (!check_78<4,std::uint64_t>(seed)) { std::puts("FAIL check_78<4,std::uint64_t>(seed)"); return 1; }
    if (!check_78<8,std::uint64_t>(seed)) { std::puts("FAIL check_78<8,std::uint64_t>(seed)"); return 1; }
    if (!check_79<1,std::int64_t>(seed)) { std::puts("FAIL check_79<1,std::int64_t>(seed)"); return 1; }
    if (!check_79<2,std::int64_t>(seed)) { std::puts("FAIL check_79<2,std::int64_t>(seed)"); return 1; }
    if (!check_79<4,std::int64_t>(seed)) { std::puts("FAIL check_79<4,std::int64_t>(seed)"); return 1; }
    if (!check_79<8,std::int64_t>(seed)) { std::puts("FAIL check_79<8,std::int64_t>(seed)"); return 1; }
    if (!check_79<1,std::uint64_t>(seed)) { std::puts("FAIL check_79<1,std::uint64_t>(seed)"); return 1; }
    if (!check_79<2,std::uint64_t>(seed)) { std::puts("FAIL check_79<2,std::uint64_t>(seed)"); return 1; }
    if (!check_79<4,std::uint64_t>(seed)) { std::puts("FAIL check_79<4,std::uint64_t>(seed)"); return 1; }
    if (!check_79<8,std::uint64_t>(seed)) { std::puts("FAIL check_79<8,std::uint64_t>(seed)"); return 1; }
    if (!guard_8(pages.last<float>())) { std::puts("FAIL guard_8(pages.last<float>())"); return 1; }
    if (!guard_9(pages.last<float>())) { std::puts("FAIL guard_9(pages.last<float>())"); return 1; }
    if (!guard_18(pages.last<std::int32_t>())) { std::puts("FAIL guard_18(pages.last<std::int32_t>())"); return 1; }
    if (!guard_18(pages.last<std::uint32_t>())) { std::puts("FAIL guard_18(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_19(pages.last<std::int32_t>())) { std::puts("FAIL guard_19(pages.last<std::int32_t>())"); return 1; }
    if (!guard_19(pages.last<std::uint32_t>())) { std::puts("FAIL guard_19(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_28(pages.last<double>())) { std::puts("FAIL guard_28(pages.last<double>())"); return 1; }
    if (!guard_29(pages.last<double>())) { std::puts("FAIL guard_29(pages.last<double>())"); return 1; }
    if (!guard_38(pages.last<std::int64_t>())) { std::puts("FAIL guard_38(pages.last<std::int64_t>())"); return 1; }
    if (!guard_38(pages.last<std::uint64_t>())) { std::puts("FAIL guard_38(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_39(pages.last<std::int64_t>())) { std::puts("FAIL guard_39(pages.last<std::int64_t>())"); return 1; }
    if (!guard_39(pages.last<std::uint64_t>())) { std::puts("FAIL guard_39(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_48(pages.last<float>())) { std::puts("FAIL guard_48(pages.last<float>())"); return 1; }
    if (!guard_49(pages.last<float>())) { std::puts("FAIL guard_49(pages.last<float>())"); return 1; }
    if (!guard_58(pages.last<std::int32_t>())) { std::puts("FAIL guard_58(pages.last<std::int32_t>())"); return 1; }
    if (!guard_58(pages.last<std::uint32_t>())) { std::puts("FAIL guard_58(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_59(pages.last<std::int32_t>())) { std::puts("FAIL guard_59(pages.last<std::int32_t>())"); return 1; }
    if (!guard_59(pages.last<std::uint32_t>())) { std::puts("FAIL guard_59(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_68(pages.last<double>())) { std::puts("FAIL guard_68(pages.last<double>())"); return 1; }
    if (!guard_69(pages.last<double>())) { std::puts("FAIL guard_69(pages.last<double>())"); return 1; }
    if (!guard_78(pages.last<std::int64_t>())) { std::puts("FAIL guard_78(pages.last<std::int64_t>())"); return 1; }
    if (!guard_78(pages.last<std::uint64_t>())) { std::puts("FAIL guard_78(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_79(pages.last<std::int64_t>())) { std::puts("FAIL guard_79(pages.last<std::int64_t>())"); return 1; }
    if (!guard_79(pages.last<std::uint64_t>())) { std::puts("FAIL guard_79(pages.last<std::uint64_t>())"); return 1; }
  }
  if (native::classify_isa(cpu, profile_2).admitted()) {
    ran = true;
    std::puts("Running AVX512F+VL indexed-memory bank.");
    if (!check_unaligned_avx512()) return 4;
    if (!check_2<1,float>(seed)) { std::puts("FAIL check_2<1,float>(seed)"); return 1; }
    if (!check_2<2,float>(seed)) { std::puts("FAIL check_2<2,float>(seed)"); return 1; }
    if (!check_2<4,float>(seed)) { std::puts("FAIL check_2<4,float>(seed)"); return 1; }
    if (!check_2<8,float>(seed)) { std::puts("FAIL check_2<8,float>(seed)"); return 1; }
    if (!check_3<1,float>(seed)) { std::puts("FAIL check_3<1,float>(seed)"); return 1; }
    if (!check_3<2,float>(seed)) { std::puts("FAIL check_3<2,float>(seed)"); return 1; }
    if (!check_3<4,float>(seed)) { std::puts("FAIL check_3<4,float>(seed)"); return 1; }
    if (!check_3<8,float>(seed)) { std::puts("FAIL check_3<8,float>(seed)"); return 1; }
    if (!check_6<1,float>(seed)) { std::puts("FAIL check_6<1,float>(seed)"); return 1; }
    if (!check_6<2,float>(seed)) { std::puts("FAIL check_6<2,float>(seed)"); return 1; }
    if (!check_6<4,float>(seed)) { std::puts("FAIL check_6<4,float>(seed)"); return 1; }
    if (!check_6<8,float>(seed)) { std::puts("FAIL check_6<8,float>(seed)"); return 1; }
    if (!check_7<1,float>(seed)) { std::puts("FAIL check_7<1,float>(seed)"); return 1; }
    if (!check_7<2,float>(seed)) { std::puts("FAIL check_7<2,float>(seed)"); return 1; }
    if (!check_7<4,float>(seed)) { std::puts("FAIL check_7<4,float>(seed)"); return 1; }
    if (!check_7<8,float>(seed)) { std::puts("FAIL check_7<8,float>(seed)"); return 1; }
    if (!check_12<1,std::int32_t>(seed)) { std::puts("FAIL check_12<1,std::int32_t>(seed)"); return 1; }
    if (!check_12<2,std::int32_t>(seed)) { std::puts("FAIL check_12<2,std::int32_t>(seed)"); return 1; }
    if (!check_12<4,std::int32_t>(seed)) { std::puts("FAIL check_12<4,std::int32_t>(seed)"); return 1; }
    if (!check_12<8,std::int32_t>(seed)) { std::puts("FAIL check_12<8,std::int32_t>(seed)"); return 1; }
    if (!check_12<1,std::uint32_t>(seed)) { std::puts("FAIL check_12<1,std::uint32_t>(seed)"); return 1; }
    if (!check_12<2,std::uint32_t>(seed)) { std::puts("FAIL check_12<2,std::uint32_t>(seed)"); return 1; }
    if (!check_12<4,std::uint32_t>(seed)) { std::puts("FAIL check_12<4,std::uint32_t>(seed)"); return 1; }
    if (!check_12<8,std::uint32_t>(seed)) { std::puts("FAIL check_12<8,std::uint32_t>(seed)"); return 1; }
    if (!check_13<1,std::int32_t>(seed)) { std::puts("FAIL check_13<1,std::int32_t>(seed)"); return 1; }
    if (!check_13<2,std::int32_t>(seed)) { std::puts("FAIL check_13<2,std::int32_t>(seed)"); return 1; }
    if (!check_13<4,std::int32_t>(seed)) { std::puts("FAIL check_13<4,std::int32_t>(seed)"); return 1; }
    if (!check_13<8,std::int32_t>(seed)) { std::puts("FAIL check_13<8,std::int32_t>(seed)"); return 1; }
    if (!check_13<1,std::uint32_t>(seed)) { std::puts("FAIL check_13<1,std::uint32_t>(seed)"); return 1; }
    if (!check_13<2,std::uint32_t>(seed)) { std::puts("FAIL check_13<2,std::uint32_t>(seed)"); return 1; }
    if (!check_13<4,std::uint32_t>(seed)) { std::puts("FAIL check_13<4,std::uint32_t>(seed)"); return 1; }
    if (!check_13<8,std::uint32_t>(seed)) { std::puts("FAIL check_13<8,std::uint32_t>(seed)"); return 1; }
    if (!check_16<1,std::int32_t>(seed)) { std::puts("FAIL check_16<1,std::int32_t>(seed)"); return 1; }
    if (!check_16<2,std::int32_t>(seed)) { std::puts("FAIL check_16<2,std::int32_t>(seed)"); return 1; }
    if (!check_16<4,std::int32_t>(seed)) { std::puts("FAIL check_16<4,std::int32_t>(seed)"); return 1; }
    if (!check_16<8,std::int32_t>(seed)) { std::puts("FAIL check_16<8,std::int32_t>(seed)"); return 1; }
    if (!check_16<1,std::uint32_t>(seed)) { std::puts("FAIL check_16<1,std::uint32_t>(seed)"); return 1; }
    if (!check_16<2,std::uint32_t>(seed)) { std::puts("FAIL check_16<2,std::uint32_t>(seed)"); return 1; }
    if (!check_16<4,std::uint32_t>(seed)) { std::puts("FAIL check_16<4,std::uint32_t>(seed)"); return 1; }
    if (!check_16<8,std::uint32_t>(seed)) { std::puts("FAIL check_16<8,std::uint32_t>(seed)"); return 1; }
    if (!check_17<1,std::int32_t>(seed)) { std::puts("FAIL check_17<1,std::int32_t>(seed)"); return 1; }
    if (!check_17<2,std::int32_t>(seed)) { std::puts("FAIL check_17<2,std::int32_t>(seed)"); return 1; }
    if (!check_17<4,std::int32_t>(seed)) { std::puts("FAIL check_17<4,std::int32_t>(seed)"); return 1; }
    if (!check_17<8,std::int32_t>(seed)) { std::puts("FAIL check_17<8,std::int32_t>(seed)"); return 1; }
    if (!check_17<1,std::uint32_t>(seed)) { std::puts("FAIL check_17<1,std::uint32_t>(seed)"); return 1; }
    if (!check_17<2,std::uint32_t>(seed)) { std::puts("FAIL check_17<2,std::uint32_t>(seed)"); return 1; }
    if (!check_17<4,std::uint32_t>(seed)) { std::puts("FAIL check_17<4,std::uint32_t>(seed)"); return 1; }
    if (!check_17<8,std::uint32_t>(seed)) { std::puts("FAIL check_17<8,std::uint32_t>(seed)"); return 1; }
    if (!check_22<1,double>(seed)) { std::puts("FAIL check_22<1,double>(seed)"); return 1; }
    if (!check_22<2,double>(seed)) { std::puts("FAIL check_22<2,double>(seed)"); return 1; }
    if (!check_22<4,double>(seed)) { std::puts("FAIL check_22<4,double>(seed)"); return 1; }
    if (!check_22<8,double>(seed)) { std::puts("FAIL check_22<8,double>(seed)"); return 1; }
    if (!check_23<1,double>(seed)) { std::puts("FAIL check_23<1,double>(seed)"); return 1; }
    if (!check_23<2,double>(seed)) { std::puts("FAIL check_23<2,double>(seed)"); return 1; }
    if (!check_23<4,double>(seed)) { std::puts("FAIL check_23<4,double>(seed)"); return 1; }
    if (!check_23<8,double>(seed)) { std::puts("FAIL check_23<8,double>(seed)"); return 1; }
    if (!check_26<1,double>(seed)) { std::puts("FAIL check_26<1,double>(seed)"); return 1; }
    if (!check_26<2,double>(seed)) { std::puts("FAIL check_26<2,double>(seed)"); return 1; }
    if (!check_26<4,double>(seed)) { std::puts("FAIL check_26<4,double>(seed)"); return 1; }
    if (!check_26<8,double>(seed)) { std::puts("FAIL check_26<8,double>(seed)"); return 1; }
    if (!check_27<1,double>(seed)) { std::puts("FAIL check_27<1,double>(seed)"); return 1; }
    if (!check_27<2,double>(seed)) { std::puts("FAIL check_27<2,double>(seed)"); return 1; }
    if (!check_27<4,double>(seed)) { std::puts("FAIL check_27<4,double>(seed)"); return 1; }
    if (!check_27<8,double>(seed)) { std::puts("FAIL check_27<8,double>(seed)"); return 1; }
    if (!check_32<1,std::int64_t>(seed)) { std::puts("FAIL check_32<1,std::int64_t>(seed)"); return 1; }
    if (!check_32<2,std::int64_t>(seed)) { std::puts("FAIL check_32<2,std::int64_t>(seed)"); return 1; }
    if (!check_32<4,std::int64_t>(seed)) { std::puts("FAIL check_32<4,std::int64_t>(seed)"); return 1; }
    if (!check_32<8,std::int64_t>(seed)) { std::puts("FAIL check_32<8,std::int64_t>(seed)"); return 1; }
    if (!check_32<1,std::uint64_t>(seed)) { std::puts("FAIL check_32<1,std::uint64_t>(seed)"); return 1; }
    if (!check_32<2,std::uint64_t>(seed)) { std::puts("FAIL check_32<2,std::uint64_t>(seed)"); return 1; }
    if (!check_32<4,std::uint64_t>(seed)) { std::puts("FAIL check_32<4,std::uint64_t>(seed)"); return 1; }
    if (!check_32<8,std::uint64_t>(seed)) { std::puts("FAIL check_32<8,std::uint64_t>(seed)"); return 1; }
    if (!check_33<1,std::int64_t>(seed)) { std::puts("FAIL check_33<1,std::int64_t>(seed)"); return 1; }
    if (!check_33<2,std::int64_t>(seed)) { std::puts("FAIL check_33<2,std::int64_t>(seed)"); return 1; }
    if (!check_33<4,std::int64_t>(seed)) { std::puts("FAIL check_33<4,std::int64_t>(seed)"); return 1; }
    if (!check_33<8,std::int64_t>(seed)) { std::puts("FAIL check_33<8,std::int64_t>(seed)"); return 1; }
    if (!check_33<1,std::uint64_t>(seed)) { std::puts("FAIL check_33<1,std::uint64_t>(seed)"); return 1; }
    if (!check_33<2,std::uint64_t>(seed)) { std::puts("FAIL check_33<2,std::uint64_t>(seed)"); return 1; }
    if (!check_33<4,std::uint64_t>(seed)) { std::puts("FAIL check_33<4,std::uint64_t>(seed)"); return 1; }
    if (!check_33<8,std::uint64_t>(seed)) { std::puts("FAIL check_33<8,std::uint64_t>(seed)"); return 1; }
    if (!check_36<1,std::int64_t>(seed)) { std::puts("FAIL check_36<1,std::int64_t>(seed)"); return 1; }
    if (!check_36<2,std::int64_t>(seed)) { std::puts("FAIL check_36<2,std::int64_t>(seed)"); return 1; }
    if (!check_36<4,std::int64_t>(seed)) { std::puts("FAIL check_36<4,std::int64_t>(seed)"); return 1; }
    if (!check_36<8,std::int64_t>(seed)) { std::puts("FAIL check_36<8,std::int64_t>(seed)"); return 1; }
    if (!check_36<1,std::uint64_t>(seed)) { std::puts("FAIL check_36<1,std::uint64_t>(seed)"); return 1; }
    if (!check_36<2,std::uint64_t>(seed)) { std::puts("FAIL check_36<2,std::uint64_t>(seed)"); return 1; }
    if (!check_36<4,std::uint64_t>(seed)) { std::puts("FAIL check_36<4,std::uint64_t>(seed)"); return 1; }
    if (!check_36<8,std::uint64_t>(seed)) { std::puts("FAIL check_36<8,std::uint64_t>(seed)"); return 1; }
    if (!check_37<1,std::int64_t>(seed)) { std::puts("FAIL check_37<1,std::int64_t>(seed)"); return 1; }
    if (!check_37<2,std::int64_t>(seed)) { std::puts("FAIL check_37<2,std::int64_t>(seed)"); return 1; }
    if (!check_37<4,std::int64_t>(seed)) { std::puts("FAIL check_37<4,std::int64_t>(seed)"); return 1; }
    if (!check_37<8,std::int64_t>(seed)) { std::puts("FAIL check_37<8,std::int64_t>(seed)"); return 1; }
    if (!check_37<1,std::uint64_t>(seed)) { std::puts("FAIL check_37<1,std::uint64_t>(seed)"); return 1; }
    if (!check_37<2,std::uint64_t>(seed)) { std::puts("FAIL check_37<2,std::uint64_t>(seed)"); return 1; }
    if (!check_37<4,std::uint64_t>(seed)) { std::puts("FAIL check_37<4,std::uint64_t>(seed)"); return 1; }
    if (!check_37<8,std::uint64_t>(seed)) { std::puts("FAIL check_37<8,std::uint64_t>(seed)"); return 1; }
    if (!check_42<1,float>(seed)) { std::puts("FAIL check_42<1,float>(seed)"); return 1; }
    if (!check_42<2,float>(seed)) { std::puts("FAIL check_42<2,float>(seed)"); return 1; }
    if (!check_42<4,float>(seed)) { std::puts("FAIL check_42<4,float>(seed)"); return 1; }
    if (!check_42<8,float>(seed)) { std::puts("FAIL check_42<8,float>(seed)"); return 1; }
    if (!check_43<1,float>(seed)) { std::puts("FAIL check_43<1,float>(seed)"); return 1; }
    if (!check_43<2,float>(seed)) { std::puts("FAIL check_43<2,float>(seed)"); return 1; }
    if (!check_43<4,float>(seed)) { std::puts("FAIL check_43<4,float>(seed)"); return 1; }
    if (!check_43<8,float>(seed)) { std::puts("FAIL check_43<8,float>(seed)"); return 1; }
    if (!check_46<1,float>(seed)) { std::puts("FAIL check_46<1,float>(seed)"); return 1; }
    if (!check_46<2,float>(seed)) { std::puts("FAIL check_46<2,float>(seed)"); return 1; }
    if (!check_46<4,float>(seed)) { std::puts("FAIL check_46<4,float>(seed)"); return 1; }
    if (!check_46<8,float>(seed)) { std::puts("FAIL check_46<8,float>(seed)"); return 1; }
    if (!check_47<1,float>(seed)) { std::puts("FAIL check_47<1,float>(seed)"); return 1; }
    if (!check_47<2,float>(seed)) { std::puts("FAIL check_47<2,float>(seed)"); return 1; }
    if (!check_47<4,float>(seed)) { std::puts("FAIL check_47<4,float>(seed)"); return 1; }
    if (!check_47<8,float>(seed)) { std::puts("FAIL check_47<8,float>(seed)"); return 1; }
    if (!check_52<1,std::int32_t>(seed)) { std::puts("FAIL check_52<1,std::int32_t>(seed)"); return 1; }
    if (!check_52<2,std::int32_t>(seed)) { std::puts("FAIL check_52<2,std::int32_t>(seed)"); return 1; }
    if (!check_52<4,std::int32_t>(seed)) { std::puts("FAIL check_52<4,std::int32_t>(seed)"); return 1; }
    if (!check_52<8,std::int32_t>(seed)) { std::puts("FAIL check_52<8,std::int32_t>(seed)"); return 1; }
    if (!check_52<1,std::uint32_t>(seed)) { std::puts("FAIL check_52<1,std::uint32_t>(seed)"); return 1; }
    if (!check_52<2,std::uint32_t>(seed)) { std::puts("FAIL check_52<2,std::uint32_t>(seed)"); return 1; }
    if (!check_52<4,std::uint32_t>(seed)) { std::puts("FAIL check_52<4,std::uint32_t>(seed)"); return 1; }
    if (!check_52<8,std::uint32_t>(seed)) { std::puts("FAIL check_52<8,std::uint32_t>(seed)"); return 1; }
    if (!check_53<1,std::int32_t>(seed)) { std::puts("FAIL check_53<1,std::int32_t>(seed)"); return 1; }
    if (!check_53<2,std::int32_t>(seed)) { std::puts("FAIL check_53<2,std::int32_t>(seed)"); return 1; }
    if (!check_53<4,std::int32_t>(seed)) { std::puts("FAIL check_53<4,std::int32_t>(seed)"); return 1; }
    if (!check_53<8,std::int32_t>(seed)) { std::puts("FAIL check_53<8,std::int32_t>(seed)"); return 1; }
    if (!check_53<1,std::uint32_t>(seed)) { std::puts("FAIL check_53<1,std::uint32_t>(seed)"); return 1; }
    if (!check_53<2,std::uint32_t>(seed)) { std::puts("FAIL check_53<2,std::uint32_t>(seed)"); return 1; }
    if (!check_53<4,std::uint32_t>(seed)) { std::puts("FAIL check_53<4,std::uint32_t>(seed)"); return 1; }
    if (!check_53<8,std::uint32_t>(seed)) { std::puts("FAIL check_53<8,std::uint32_t>(seed)"); return 1; }
    if (!check_56<1,std::int32_t>(seed)) { std::puts("FAIL check_56<1,std::int32_t>(seed)"); return 1; }
    if (!check_56<2,std::int32_t>(seed)) { std::puts("FAIL check_56<2,std::int32_t>(seed)"); return 1; }
    if (!check_56<4,std::int32_t>(seed)) { std::puts("FAIL check_56<4,std::int32_t>(seed)"); return 1; }
    if (!check_56<8,std::int32_t>(seed)) { std::puts("FAIL check_56<8,std::int32_t>(seed)"); return 1; }
    if (!check_56<1,std::uint32_t>(seed)) { std::puts("FAIL check_56<1,std::uint32_t>(seed)"); return 1; }
    if (!check_56<2,std::uint32_t>(seed)) { std::puts("FAIL check_56<2,std::uint32_t>(seed)"); return 1; }
    if (!check_56<4,std::uint32_t>(seed)) { std::puts("FAIL check_56<4,std::uint32_t>(seed)"); return 1; }
    if (!check_56<8,std::uint32_t>(seed)) { std::puts("FAIL check_56<8,std::uint32_t>(seed)"); return 1; }
    if (!check_57<1,std::int32_t>(seed)) { std::puts("FAIL check_57<1,std::int32_t>(seed)"); return 1; }
    if (!check_57<2,std::int32_t>(seed)) { std::puts("FAIL check_57<2,std::int32_t>(seed)"); return 1; }
    if (!check_57<4,std::int32_t>(seed)) { std::puts("FAIL check_57<4,std::int32_t>(seed)"); return 1; }
    if (!check_57<8,std::int32_t>(seed)) { std::puts("FAIL check_57<8,std::int32_t>(seed)"); return 1; }
    if (!check_57<1,std::uint32_t>(seed)) { std::puts("FAIL check_57<1,std::uint32_t>(seed)"); return 1; }
    if (!check_57<2,std::uint32_t>(seed)) { std::puts("FAIL check_57<2,std::uint32_t>(seed)"); return 1; }
    if (!check_57<4,std::uint32_t>(seed)) { std::puts("FAIL check_57<4,std::uint32_t>(seed)"); return 1; }
    if (!check_57<8,std::uint32_t>(seed)) { std::puts("FAIL check_57<8,std::uint32_t>(seed)"); return 1; }
    if (!check_62<1,double>(seed)) { std::puts("FAIL check_62<1,double>(seed)"); return 1; }
    if (!check_62<2,double>(seed)) { std::puts("FAIL check_62<2,double>(seed)"); return 1; }
    if (!check_62<4,double>(seed)) { std::puts("FAIL check_62<4,double>(seed)"); return 1; }
    if (!check_62<8,double>(seed)) { std::puts("FAIL check_62<8,double>(seed)"); return 1; }
    if (!check_63<1,double>(seed)) { std::puts("FAIL check_63<1,double>(seed)"); return 1; }
    if (!check_63<2,double>(seed)) { std::puts("FAIL check_63<2,double>(seed)"); return 1; }
    if (!check_63<4,double>(seed)) { std::puts("FAIL check_63<4,double>(seed)"); return 1; }
    if (!check_63<8,double>(seed)) { std::puts("FAIL check_63<8,double>(seed)"); return 1; }
    if (!check_66<1,double>(seed)) { std::puts("FAIL check_66<1,double>(seed)"); return 1; }
    if (!check_66<2,double>(seed)) { std::puts("FAIL check_66<2,double>(seed)"); return 1; }
    if (!check_66<4,double>(seed)) { std::puts("FAIL check_66<4,double>(seed)"); return 1; }
    if (!check_66<8,double>(seed)) { std::puts("FAIL check_66<8,double>(seed)"); return 1; }
    if (!check_67<1,double>(seed)) { std::puts("FAIL check_67<1,double>(seed)"); return 1; }
    if (!check_67<2,double>(seed)) { std::puts("FAIL check_67<2,double>(seed)"); return 1; }
    if (!check_67<4,double>(seed)) { std::puts("FAIL check_67<4,double>(seed)"); return 1; }
    if (!check_67<8,double>(seed)) { std::puts("FAIL check_67<8,double>(seed)"); return 1; }
    if (!check_72<1,std::int64_t>(seed)) { std::puts("FAIL check_72<1,std::int64_t>(seed)"); return 1; }
    if (!check_72<2,std::int64_t>(seed)) { std::puts("FAIL check_72<2,std::int64_t>(seed)"); return 1; }
    if (!check_72<4,std::int64_t>(seed)) { std::puts("FAIL check_72<4,std::int64_t>(seed)"); return 1; }
    if (!check_72<8,std::int64_t>(seed)) { std::puts("FAIL check_72<8,std::int64_t>(seed)"); return 1; }
    if (!check_72<1,std::uint64_t>(seed)) { std::puts("FAIL check_72<1,std::uint64_t>(seed)"); return 1; }
    if (!check_72<2,std::uint64_t>(seed)) { std::puts("FAIL check_72<2,std::uint64_t>(seed)"); return 1; }
    if (!check_72<4,std::uint64_t>(seed)) { std::puts("FAIL check_72<4,std::uint64_t>(seed)"); return 1; }
    if (!check_72<8,std::uint64_t>(seed)) { std::puts("FAIL check_72<8,std::uint64_t>(seed)"); return 1; }
    if (!check_73<1,std::int64_t>(seed)) { std::puts("FAIL check_73<1,std::int64_t>(seed)"); return 1; }
    if (!check_73<2,std::int64_t>(seed)) { std::puts("FAIL check_73<2,std::int64_t>(seed)"); return 1; }
    if (!check_73<4,std::int64_t>(seed)) { std::puts("FAIL check_73<4,std::int64_t>(seed)"); return 1; }
    if (!check_73<8,std::int64_t>(seed)) { std::puts("FAIL check_73<8,std::int64_t>(seed)"); return 1; }
    if (!check_73<1,std::uint64_t>(seed)) { std::puts("FAIL check_73<1,std::uint64_t>(seed)"); return 1; }
    if (!check_73<2,std::uint64_t>(seed)) { std::puts("FAIL check_73<2,std::uint64_t>(seed)"); return 1; }
    if (!check_73<4,std::uint64_t>(seed)) { std::puts("FAIL check_73<4,std::uint64_t>(seed)"); return 1; }
    if (!check_73<8,std::uint64_t>(seed)) { std::puts("FAIL check_73<8,std::uint64_t>(seed)"); return 1; }
    if (!check_76<1,std::int64_t>(seed)) { std::puts("FAIL check_76<1,std::int64_t>(seed)"); return 1; }
    if (!check_76<2,std::int64_t>(seed)) { std::puts("FAIL check_76<2,std::int64_t>(seed)"); return 1; }
    if (!check_76<4,std::int64_t>(seed)) { std::puts("FAIL check_76<4,std::int64_t>(seed)"); return 1; }
    if (!check_76<8,std::int64_t>(seed)) { std::puts("FAIL check_76<8,std::int64_t>(seed)"); return 1; }
    if (!check_76<1,std::uint64_t>(seed)) { std::puts("FAIL check_76<1,std::uint64_t>(seed)"); return 1; }
    if (!check_76<2,std::uint64_t>(seed)) { std::puts("FAIL check_76<2,std::uint64_t>(seed)"); return 1; }
    if (!check_76<4,std::uint64_t>(seed)) { std::puts("FAIL check_76<4,std::uint64_t>(seed)"); return 1; }
    if (!check_76<8,std::uint64_t>(seed)) { std::puts("FAIL check_76<8,std::uint64_t>(seed)"); return 1; }
    if (!check_77<1,std::int64_t>(seed)) { std::puts("FAIL check_77<1,std::int64_t>(seed)"); return 1; }
    if (!check_77<2,std::int64_t>(seed)) { std::puts("FAIL check_77<2,std::int64_t>(seed)"); return 1; }
    if (!check_77<4,std::int64_t>(seed)) { std::puts("FAIL check_77<4,std::int64_t>(seed)"); return 1; }
    if (!check_77<8,std::int64_t>(seed)) { std::puts("FAIL check_77<8,std::int64_t>(seed)"); return 1; }
    if (!check_77<1,std::uint64_t>(seed)) { std::puts("FAIL check_77<1,std::uint64_t>(seed)"); return 1; }
    if (!check_77<2,std::uint64_t>(seed)) { std::puts("FAIL check_77<2,std::uint64_t>(seed)"); return 1; }
    if (!check_77<4,std::uint64_t>(seed)) { std::puts("FAIL check_77<4,std::uint64_t>(seed)"); return 1; }
    if (!check_77<8,std::uint64_t>(seed)) { std::puts("FAIL check_77<8,std::uint64_t>(seed)"); return 1; }
    if (!guard_2(pages.last<float>())) { std::puts("FAIL guard_2(pages.last<float>())"); return 1; }
    if (!guard_3(pages.last<float>())) { std::puts("FAIL guard_3(pages.last<float>())"); return 1; }
    if (!guard_6(pages.last<float>())) { std::puts("FAIL guard_6(pages.last<float>())"); return 1; }
    if (!guard_7(pages.last<float>())) { std::puts("FAIL guard_7(pages.last<float>())"); return 1; }
    if (!guard_12(pages.last<std::int32_t>())) { std::puts("FAIL guard_12(pages.last<std::int32_t>())"); return 1; }
    if (!guard_12(pages.last<std::uint32_t>())) { std::puts("FAIL guard_12(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_13(pages.last<std::int32_t>())) { std::puts("FAIL guard_13(pages.last<std::int32_t>())"); return 1; }
    if (!guard_13(pages.last<std::uint32_t>())) { std::puts("FAIL guard_13(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_16(pages.last<std::int32_t>())) { std::puts("FAIL guard_16(pages.last<std::int32_t>())"); return 1; }
    if (!guard_16(pages.last<std::uint32_t>())) { std::puts("FAIL guard_16(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_17(pages.last<std::int32_t>())) { std::puts("FAIL guard_17(pages.last<std::int32_t>())"); return 1; }
    if (!guard_17(pages.last<std::uint32_t>())) { std::puts("FAIL guard_17(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_22(pages.last<double>())) { std::puts("FAIL guard_22(pages.last<double>())"); return 1; }
    if (!guard_23(pages.last<double>())) { std::puts("FAIL guard_23(pages.last<double>())"); return 1; }
    if (!guard_26(pages.last<double>())) { std::puts("FAIL guard_26(pages.last<double>())"); return 1; }
    if (!guard_27(pages.last<double>())) { std::puts("FAIL guard_27(pages.last<double>())"); return 1; }
    if (!guard_32(pages.last<std::int64_t>())) { std::puts("FAIL guard_32(pages.last<std::int64_t>())"); return 1; }
    if (!guard_32(pages.last<std::uint64_t>())) { std::puts("FAIL guard_32(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_33(pages.last<std::int64_t>())) { std::puts("FAIL guard_33(pages.last<std::int64_t>())"); return 1; }
    if (!guard_33(pages.last<std::uint64_t>())) { std::puts("FAIL guard_33(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_36(pages.last<std::int64_t>())) { std::puts("FAIL guard_36(pages.last<std::int64_t>())"); return 1; }
    if (!guard_36(pages.last<std::uint64_t>())) { std::puts("FAIL guard_36(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_37(pages.last<std::int64_t>())) { std::puts("FAIL guard_37(pages.last<std::int64_t>())"); return 1; }
    if (!guard_37(pages.last<std::uint64_t>())) { std::puts("FAIL guard_37(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_42(pages.last<float>())) { std::puts("FAIL guard_42(pages.last<float>())"); return 1; }
    if (!guard_43(pages.last<float>())) { std::puts("FAIL guard_43(pages.last<float>())"); return 1; }
    if (!guard_46(pages.last<float>())) { std::puts("FAIL guard_46(pages.last<float>())"); return 1; }
    if (!guard_47(pages.last<float>())) { std::puts("FAIL guard_47(pages.last<float>())"); return 1; }
    if (!guard_52(pages.last<std::int32_t>())) { std::puts("FAIL guard_52(pages.last<std::int32_t>())"); return 1; }
    if (!guard_52(pages.last<std::uint32_t>())) { std::puts("FAIL guard_52(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_53(pages.last<std::int32_t>())) { std::puts("FAIL guard_53(pages.last<std::int32_t>())"); return 1; }
    if (!guard_53(pages.last<std::uint32_t>())) { std::puts("FAIL guard_53(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_56(pages.last<std::int32_t>())) { std::puts("FAIL guard_56(pages.last<std::int32_t>())"); return 1; }
    if (!guard_56(pages.last<std::uint32_t>())) { std::puts("FAIL guard_56(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_57(pages.last<std::int32_t>())) { std::puts("FAIL guard_57(pages.last<std::int32_t>())"); return 1; }
    if (!guard_57(pages.last<std::uint32_t>())) { std::puts("FAIL guard_57(pages.last<std::uint32_t>())"); return 1; }
    if (!guard_62(pages.last<double>())) { std::puts("FAIL guard_62(pages.last<double>())"); return 1; }
    if (!guard_63(pages.last<double>())) { std::puts("FAIL guard_63(pages.last<double>())"); return 1; }
    if (!guard_66(pages.last<double>())) { std::puts("FAIL guard_66(pages.last<double>())"); return 1; }
    if (!guard_67(pages.last<double>())) { std::puts("FAIL guard_67(pages.last<double>())"); return 1; }
    if (!guard_72(pages.last<std::int64_t>())) { std::puts("FAIL guard_72(pages.last<std::int64_t>())"); return 1; }
    if (!guard_72(pages.last<std::uint64_t>())) { std::puts("FAIL guard_72(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_73(pages.last<std::int64_t>())) { std::puts("FAIL guard_73(pages.last<std::int64_t>())"); return 1; }
    if (!guard_73(pages.last<std::uint64_t>())) { std::puts("FAIL guard_73(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_76(pages.last<std::int64_t>())) { std::puts("FAIL guard_76(pages.last<std::int64_t>())"); return 1; }
    if (!guard_76(pages.last<std::uint64_t>())) { std::puts("FAIL guard_76(pages.last<std::uint64_t>())"); return 1; }
    if (!guard_77(pages.last<std::int64_t>())) { std::puts("FAIL guard_77(pages.last<std::int64_t>())"); return 1; }
    if (!guard_77(pages.last<std::uint64_t>())) { std::puts("FAIL guard_77(pages.last<std::uint64_t>())"); return 1; }
  }
  if (!native::classify_isa(cpu, profile_0).admitted()) std::puts("SKIP AVX2 indexed-memory bank.");
  if (!native::classify_isa(cpu, profile_1).admitted()) std::puts("SKIP AVX512F indexed-memory bank.");
  if (!native::classify_isa(cpu, profile_2).admitted()) std::puts("SKIP AVX512F+VL indexed-memory bank.");
  std::puts(ran ? "Indexed memory runtime and protected-page checks passed." : "SKIP: indexed memory ISA unavailable.");
  return ran ? 0 : 77;
}
