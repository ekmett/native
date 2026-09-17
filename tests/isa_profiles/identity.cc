// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
import simd.wide;
import simd.avx2;
import simd.avx512;

template<class Arch> struct family {
  using value = simd::vec<float,4,Arch>;
};
static_assert(!std::same_as<family<simd::avx2>,family<simd::avx512>>);
static_assert(!std::same_as<family<simd::avx2>::value,family<simd::avx512>::value>);
static_assert(std::same_as<decltype(simd::vec{simd::avx2{},1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f}),simd::vec<float,8,simd::avx2>>);
static_assert(std::same_as<decltype(simd::vec{simd::avx512{},1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f,12.f,13.f,14.f,15.f,16.f}),simd::vec<float,16,simd::avx512>>);
template<template<class,std::size_t,class> class Vector,class Arch>
using generic_float4 = Vector<float,4,Arch>;
static_assert(std::same_as<generic_float4<simd::vec,simd::avx2>,family<simd::avx2>::value>);
static_assert(std::same_as<decltype(simd::vec(simd::avx2{},std::array<float,4>{})),simd::vec<float,4,simd::avx2>>);
static_assert(std::same_as<decltype(simd::vec(std::declval<simd::vec<float,4,simd::avx2>>())),simd::vec<float,4,simd::avx2>>);
static_assert(sizeof(simd::vec<float,8,simd::avx2>) == 32);
static_assert(sizeof(simd::vec<float,16,simd::avx512>) == 64);
// A broader importing ISA must not change AVX2's full-vector mask topology.
static_assert(!simd::vec<float,8,simd::avx2>::mask::compact);
static_assert(simd::vec<float,8,simd::avx512>::mask::compact);
static_assert(std::same_as<decltype(std::declval<simd::vec<float,8,simd::avx2>>() < std::declval<simd::vec<float,8,simd::avx2>>()),simd::vec<float,8,simd::avx2>::mask>);
static_assert(std::same_as<decltype(std::declval<simd::vec<float,8,simd::avx512>>() < std::declval<simd::vec<float,8,simd::avx512>>()),simd::vec<float,8,simd::avx512>::mask>);
static_assert(std::same_as<decltype(std::declval<simd::vec<int,8,simd::avx2>>() == std::declval<simd::vec<int,8,simd::avx2>>()),simd::vec<int,8,simd::avx2>::mask>);
static_assert(std::same_as<decltype(std::declval<simd::vec<int,16,simd::avx512>>() == std::declval<simd::vec<int,16,simd::avx512>>()),simd::vec<int,16,simd::avx512>::mask>);
extern "C" bool profile_identities() { return true; }
