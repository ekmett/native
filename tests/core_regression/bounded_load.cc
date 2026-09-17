// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <simd/vec.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace {
  struct pages {
    std::size_t page = std::size_t(sysconf(_SC_PAGESIZE));
    unsigned char *base = static_cast<unsigned char *>(mmap(nullptr, page * 3, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0));
    pages() {
      if (base == MAP_FAILED) throw std::runtime_error("mmap");
      if (mprotect(base + page, page, PROT_READ | PROT_WRITE)) {
        munmap(base, page * 3); throw std::runtime_error("mprotect");
      }
    }
    ~pages() { munmap(base, page * 3); }
  };
  template<class T, std::size_t N> void check(pages &memory) {
    using V = simd::vec<T,N,SIMD_ARCH>;
    std::array<T,N> input{}, output{};
    for (std::size_t i=0;i<N;++i) input[i]=T(~std::uint64_t(0) - i * 0x12345678u);
    for (T fill : {T(0), T(1), T(~T(0))}) {
      for (std::size_t n=0;n<=N;++n) {
        auto test=[&](unsigned char *p) {
          if (n) std::memcpy(p,input.data(),n*sizeof(T));
          auto value=V::load_partial(reinterpret_cast<T const *>(p),n,fill);
          value.store(output.data());
          for (std::size_t i=0;i<N;++i)
            if (output[i] != (i<n?input[i]:fill)) throw std::runtime_error("bounded load mismatch");
        };
        // An exact guard boundary, and every byte-address residue independently.
        test(memory.base+memory.page*2-n*sizeof(T));
        for (unsigned residue=0;residue<64;++residue) test(memory.base+memory.page+residue);
      }
      auto empty=V::load_partial(nullptr,0,fill); empty.store(output.data());
      for (T lane:output) if(lane!=fill) throw std::runtime_error("null empty load");
    }
  }
}
int main() {
  pages memory;
  check<std::uint32_t,1>(memory);check<std::int32_t,1>(memory);
  check<std::uint64_t,1>(memory);check<std::int64_t,1>(memory);
#if SIMD_PROFILE != 0
  check<std::uint32_t,4>(memory);check<std::int32_t,4>(memory);
  check<std::uint64_t,2>(memory);check<std::int64_t,2>(memory);
#endif
#if SIMD_HAS_AVX2
  check<std::uint32_t,8>(memory);check<std::int32_t,8>(memory);
  check<std::uint64_t,4>(memory);check<std::int64_t,4>(memory);
#endif
#if SIMD_HAS_AVX512F && SIMD_HAS_AVX512DQ
  check<std::uint32_t,16>(memory);check<std::int32_t,16>(memory);
  check<std::uint64_t,8>(memory);check<std::int64_t,8>(memory);
#endif
}
