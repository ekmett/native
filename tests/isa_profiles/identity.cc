// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
import native.wide;
import native;

template<native::isa Arch,class... X>
concept isa_deducible=requires(X... x) { native::vec(Arch,x...); };
static_assert(!isa_deducible<native::avx2,float,float,float,float>);
static_assert(!isa_deducible<native::avx2,std::array<float,4>>);

template<native::isa Arch> struct family {
  using value = native::vec<float,4,Arch>;
};
static_assert(!std::same_as<family<native::avx2>,family<native::avx512>>);
static_assert(!std::same_as<family<native::avx2>::value,family<native::avx512>::value>);
static_assert(std::same_as<decltype(native::vec<float,8,native::avx2>{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f}),native::vec<float,8,native::avx2>>);
static_assert(std::same_as<decltype(native::vec<float,16,native::avx512>{1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f,12.f,13.f,14.f,15.f,16.f}),native::vec<float,16,native::avx512>>);
template<template<class,std::size_t,native::isa> class Vector,native::isa Arch>
using generic_float4 = Vector<float,4,Arch>;
static_assert(std::same_as<generic_float4<native::vec,native::avx2>,family<native::avx2>::value>);
static_assert(std::same_as<decltype(native::vec<float,4,native::avx2>(std::array<float,4>{})),native::vec<float,4,native::avx2>>);
static_assert(std::same_as<decltype(native::vec(std::declval<native::vec<float,4,native::avx2>>())),native::vec<float,4,native::avx2>>);
static_assert(sizeof(native::vec<float,8,native::avx2>) == 32);
static_assert(sizeof(native::vec<float,16,native::avx512>) == 64);
// A broader importing ISA must not change AVX2's full-vector mask topology.
static_assert(!native::vec<float,8,native::avx2>::mask::compact);
static_assert(native::vec<float,8,native::avx512>::mask::compact);
static_assert(std::same_as<decltype(std::declval<native::vec<float,8,native::avx2>>() < std::declval<native::vec<float,8,native::avx2>>()),native::vec<float,8,native::avx2>::mask>);
static_assert(std::same_as<decltype(std::declval<native::vec<float,8,native::avx512>>() < std::declval<native::vec<float,8,native::avx512>>()),native::vec<float,8,native::avx512>::mask>);
static_assert(std::same_as<decltype(std::declval<native::vec<int,8,native::avx2>>() == std::declval<native::vec<int,8,native::avx2>>()),native::vec<int,8,native::avx2>::mask>);
static_assert(std::same_as<decltype(std::declval<native::vec<int,16,native::avx512>>() == std::declval<native::vec<int,16,native::avx512>>()),native::vec<int,16,native::avx512>::mask>);
extern "C" bool profile_identities() { return true; }
