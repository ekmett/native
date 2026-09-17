#pragma once
#include <cstddef>
#include <cstdint>

namespace profile_test {
  inline constexpr std::size_t count = 96;
  inline constexpr std::size_t columns = 8;
  inline constexpr std::size_t words = count * columns;
  using entry = std::size_t (*)(std::uint32_t *, std::size_t);
}

extern "C" std::size_t profile_avx2_header(std::uint32_t *, std::size_t);
extern "C" std::size_t profile_avx2_import(std::uint32_t *, std::size_t);
extern "C" std::size_t profile_avx512_header(std::uint32_t *, std::size_t);
extern "C" std::size_t profile_avx512_import(std::uint32_t *, std::size_t);
extern "C" bool profile_identities();
