// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "support/profile.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>

namespace {
  [[noreturn]] void fail(char const * message) {
    std::fprintf(stderr, "%s\n", message);
    std::abort();
  }

  struct pages {
    std::size_t page = std::size_t(sysconf(_SC_PAGESIZE));
    unsigned char *base = static_cast<unsigned char *>(mmap(nullptr, page * 3, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0));
    pages() {
      if (base == MAP_FAILED) fail("mmap");
      if (mprotect(base + page, page, PROT_READ | PROT_WRITE)) {
        munmap(base, page * 3); fail("mprotect");
      }
    }
    ~pages() { munmap(base, page * 3); }
  };
  template<class T, std::size_t N> void check(pages &memory) {
    using V = native::vec<T,N,test_arch>;
    std::array<T,N> input{}, output{};
    for (std::size_t i=0;i<N;++i) input[i]=T(~std::uint64_t(0) - i * 0x12345678u);
    for (T fill : {T(0), T(1), T(~T(0))}) {
      for (std::size_t n=0;n<=N;++n) {
        auto test=[&](unsigned char *p) {
          if (n) std::memcpy(p,input.data(),n*sizeof(T));
          auto value=V::load_partial(reinterpret_cast<T const *>(p),n,fill);
          value.store(output.data());
          for (std::size_t i=0;i<N;++i)
            if (output[i] != (i<n?input[i]:fill)) fail("bounded load mismatch");
        };
        // An exact guard boundary, and every byte-address residue independently.
        test(memory.base+memory.page*2-n*sizeof(T));
        for (unsigned residue=0;residue<64;++residue) test(memory.base+memory.page+residue);
      }
      auto empty=V::load_partial(nullptr,0,fill); empty.store(output.data());
      for (T lane:output) if(lane!=fill) fail("null empty load");
    }
  }
}
int main() {
  pages memory;
  check<std::uint32_t,1>(memory);check<std::int32_t,1>(memory);
  check<std::uint64_t,1>(memory);check<std::int64_t,1>(memory);
#if NATIVE_TEST_PROFILE != 0
  check<std::uint32_t,4>(memory);check<std::int32_t,4>(memory);
  check<std::uint64_t,2>(memory);check<std::int64_t,2>(memory);
#endif
#if NATIVE_TEST_PROFILE >= 256
  check<std::uint32_t,8>(memory);check<std::int32_t,8>(memory);
  check<std::uint64_t,4>(memory);check<std::int64_t,4>(memory);
#endif
#if NATIVE_TEST_PROFILE == 512
  check<std::uint32_t,16>(memory);check<std::int32_t,16>(memory);
  check<std::uint64_t,8>(memory);check<std::int64_t,8>(memory);
#endif
}
