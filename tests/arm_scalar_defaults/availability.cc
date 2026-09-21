// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
#include <utility>
import native.arm.crc;
import native.arm.rdm;
import native.arm.pmull;
import native.arm.sha;

using native::isa;
using native::arm_feature;
constexpr auto crc = native::feature_closure(arm_feature::crc);
constexpr auto rdm = native::feature_closure(arm_feature::rdm);
constexpr auto pmull = native::feature_closure(arm_feature::pmull);
constexpr auto sha1 = native::feature_closure(arm_feature::sha1);
constexpr auto provider = native::target_features<native::arm>("neon,crc,aes,sha2,rdm");
constexpr bool present = NATIVE_ARM_DEFAULT_PRESENT;

// Immediate-only weak overloads participate in unevaluated requires expressions.
// reject.cc proves that nonconstant inputs still cannot use them at runtime.
#define CRC_CONTRACT(name) \
  template<class A, class B> concept default_##name = requires(A a, B b) { native::name(a,b); }; \
  template<isa<native::arm> Arch, class A, class B> concept explicit_##name = requires(A a, B b) { native::name<Arch>(a,b); }; \
  template<class T> consteval bool name##_contract() { \
    return default_##name<std::uint32_t,T> \
      && explicit_##name<crc,std::uint32_t,T> \
      && explicit_##name<isa<native::arm>(arm_feature::neon),std::uint32_t,T>; \
  } \
  static_assert(name##_contract<std::uint8_t>() && name##_contract<std::uint16_t>() \
    && name##_contract<std::uint32_t>() && name##_contract<std::uint64_t>()); \
  static_assert(!default_##name<std::int32_t,std::uint8_t> \
    && !default_##name<std::uint32_t,std::int32_t> \
    && !default_##name<std::uint32_t,float> \
    && !explicit_##name<crc,std::uint32_t,std::int32_t>);
CRC_CONTRACT(crc32)
CRC_CONTRACT(crc32c)
#undef CRC_CONTRACT

#define RDM_CONTRACT(name) \
  template<class A, class B, class C> concept default_##name = requires(A a, B b, C c) { native::name(a,b,c); }; \
  template<isa<native::arm> Arch, class T> concept explicit_##name = requires(T x) { native::name<Arch>(x,x,x); }; \
  template<class T> consteval bool name##_contract() { \
    return default_##name<T,T,T> && explicit_##name<rdm,T> \
      && explicit_##name<isa<native::arm>(arm_feature::neon),T>; \
  } \
  static_assert(name##_contract<std::int16_t>() && name##_contract<std::int32_t>()); \
  static_assert(!default_##name<std::int32_t,std::int16_t,std::int16_t> \
    && !default_##name<std::uint32_t,std::uint32_t,std::uint32_t> \
    && !default_##name<std::int64_t,std::int64_t,std::int64_t> \
    && !explicit_##name<rdm,std::uint32_t>);
RDM_CONTRACT(sqrdmlah)
RDM_CONTRACT(sqrdmlsh)
#undef RDM_CONTRACT

template<class T> concept default_sha1h = requires(T x) { native::sha1h(x); };
template<isa<native::arm> Arch, class T> concept explicit_sha1h = requires(T x) { native::sha1h<Arch>(x); };
static_assert(default_sha1h<std::uint32_t>);
static_assert(explicit_sha1h<sha1,std::uint32_t> && explicit_sha1h<isa<native::arm>(arm_feature::neon),std::uint32_t>);
static_assert(!default_sha1h<std::int32_t> && !default_sha1h<std::uint16_t>
  && !default_sha1h<std::uint64_t> && !explicit_sha1h<sha1,std::int32_t>);

template<class A, class B> concept default_pmull = requires(A a, B b) { native::pmull(a,b); };
template<isa<native::arm> Arch, class T> concept explicit_pmull = requires(T x) { native::pmull<Arch>(x,x); };
static_assert(default_pmull<std::uint64_t,std::uint64_t>);
static_assert(explicit_pmull<pmull,std::uint64_t> && explicit_pmull<isa<native::arm>(arm_feature::neon),std::uint64_t>);
static_assert(!default_pmull<std::uint32_t,std::uint64_t>
  && !default_pmull<std::int64_t,std::int64_t> && !explicit_pmull<pmull,std::int64_t>);
#if NATIVE_ARM_DEFAULT_PRESENT
static_assert(std::same_as<decltype(native::pmull(std::uint64_t{},std::uint64_t{})),
  native::simd<std::uint64_t,2,provider>>);
static_assert(noexcept(native::pmull(std::uint64_t{},std::uint64_t{})));
#endif
static_assert(std::same_as<decltype(native::pmull<pmull>(std::uint64_t{},std::uint64_t{})),
  native::simd<std::uint64_t,2,pmull>>);

// Vector operands continue to deduce their own ISA, even when it differs from
// the provider's baseline. Scalar-return lane forms also deduce from the vector.
using rdm_words = native::simd<std::int32_t,2,rdm>;
using sha_words = native::simd<std::uint32_t,4,sha1>;
using polynomial = native::simd<std::uint64_t,2,pmull>;
using bytes = native::simd<std::uint8_t,8,isa<native::arm>(arm_feature::neon)>;
static_assert(std::same_as<decltype(native::sqrdmlah(rdm_words{},rdm_words{},rdm_words{})),rdm_words>);
static_assert(std::same_as<decltype(native::sqrdmlsh(rdm_words{},rdm_words{},rdm_words{})),rdm_words>);
static_assert(std::same_as<decltype(native::sha1c(sha_words{},std::uint32_t{},sha_words{})),sha_words>);
static_assert(std::same_as<decltype(native::pmull2(polynomial{},polynomial{})),polynomial>);
static_assert(std::same_as<decltype(native::pmull(bytes{},bytes{})),native::simd<std::uint16_t,8,isa<native::arm>(arm_feature::neon)>>);
static_assert(std::same_as<decltype(native::sqrdmlah_lane<rdm,1>(std::int32_t{},std::int32_t{},rdm_words{})),std::int32_t>);
template<class V> concept valid_lane = requires(V v) { native::sqrdmlah_lane<rdm,2>(std::int32_t{},std::int32_t{},v); };
static_assert(!valid_lane<rdm_words>);
// Function targets enable instructions, but do not replace module defaults.
__attribute__((target("crc,rdm,aes,sha2"))) consteval bool scoped_contract() {
  return default_crc32<std::uint32_t,std::uint64_t>
    && default_sqrdmlah<std::int32_t,std::int32_t,std::int32_t>
    && default_sha1h<std::uint32_t>
    && default_pmull<std::uint64_t,std::uint64_t>;
}
static_assert(scoped_contract());
int main() { return 0; }
