// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

// The including fixture imports one set of owning operation modules.
namespace scalar_defaults_fixture {
  template<class T> concept call_0 = requires(T a, std::uint32_t b) {
    { native::andn(a, b) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_0<std::uint32_t> == true);
  template<class T> concept call_1 = requires(T a, std::uint32_t control) {
    { native::bextr(a, control) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_1<std::uint32_t> == true);
  template<class T> concept call_2 = requires(T a, unsigned start, unsigned length) {
    { native::bextr(a, start, length) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_2<std::uint32_t> == true);
  template<class T> concept call_3 = requires(T a) {
    { native::blsi(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_3<std::uint32_t> == true);
  template<class T> concept call_4 = requires(T a) {
    { native::blsmsk(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_4<std::uint32_t> == true);
  template<class T> concept call_5 = requires(T a) {
    { native::blsr(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_5<std::uint32_t> == true);
  template<class T> concept call_6 = requires(T a, unsigned count) {
    { native::bzhi(a, count) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_6<std::uint32_t> == true);
  template<class T> concept call_7 = requires(T a, std::uint32_t b, std::uint32_t* high) {
    { native::mulx(a, b, high) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_7<std::uint32_t> == true);
  template<class T> concept call_8 = requires(T a, std::uint32_t mask) {
    { native::pdep(a, mask) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_8<std::uint32_t> == true);
  template<class T> concept call_9 = requires(T a, std::uint32_t mask) {
    { native::pext(a, mask) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_9<std::uint32_t> == true);
  template<class T> concept call_10 = requires(T a, unsigned count) {
    { native::shlx(a, count) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_10<std::uint32_t> == true);
  template<class T> concept call_11 = requires(T a, unsigned count) {
    { native::shrx(a, count) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_11<std::uint32_t> == true);
  template<class T> concept call_12 = requires(T a) {
    { native::rorx<7>(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_12<std::uint32_t> == true);
  template<class T> concept call_13 = requires(T a, std::uint64_t b) {
    { native::andn(a, b) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_13<std::uint64_t> == true);
  template<class T> concept call_14 = requires(T a, std::uint32_t control) {
    { native::bextr(a, control) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_14<std::uint64_t> == true);
  template<class T> concept call_15 = requires(T a, unsigned start, unsigned length) {
    { native::bextr(a, start, length) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_15<std::uint64_t> == true);
  template<class T> concept call_16 = requires(T a) {
    { native::blsi(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_16<std::uint64_t> == true);
  template<class T> concept call_17 = requires(T a) {
    { native::blsmsk(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_17<std::uint64_t> == true);
  template<class T> concept call_18 = requires(T a) {
    { native::blsr(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_18<std::uint64_t> == true);
  template<class T> concept call_19 = requires(T a, unsigned count) {
    { native::bzhi(a, count) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_19<std::uint64_t> == true);
  template<class T> concept call_20 = requires(T a, std::uint64_t b, std::uint64_t* high) {
    { native::mulx(a, b, high) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_20<std::uint64_t> == true);
  template<class T> concept call_21 = requires(T a, std::uint64_t mask) {
    { native::pdep(a, mask) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_21<std::uint64_t> == true);
  template<class T> concept call_22 = requires(T a, std::uint64_t mask) {
    { native::pext(a, mask) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_22<std::uint64_t> == true);
  template<class T> concept call_23 = requires(T a, unsigned count) {
    { native::shlx(a, count) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_23<std::uint64_t> == true);
  template<class T> concept call_24 = requires(T a, unsigned count) {
    { native::shrx(a, count) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_24<std::uint64_t> == true);
  template<class T> concept call_25 = requires(T a) {
    { native::rorx<7>(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_25<std::uint64_t> == true);
  template<class T> concept call_26 = requires(T a, unsigned count) {
    { native::sarx(a, count) } noexcept -> std::same_as<std::int32_t>;
  };
  static_assert(call_26<std::int32_t> == true);
  template<class T> concept call_27 = requires(T a, unsigned count) {
    { native::sarx(a, count) } noexcept -> std::same_as<std::int64_t>;
  };
  static_assert(call_27<std::int64_t> == true);
  template<class T> concept call_28 = requires(T a) {
    { native::tzcnt(a) } noexcept -> std::same_as<std::uint16_t>;
  };
  static_assert(call_28<std::uint16_t> == true);
  template<class T> concept call_29 = requires(T a) {
    { native::tzcnt(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_29<std::uint32_t> == true);
  template<class T> concept call_30 = requires(T a) {
    { native::tzcnt(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_30<std::uint64_t> == true);
  template<class T> concept call_31 = requires(T a) {
    { native::popcnt(a) } noexcept -> std::same_as<std::uint16_t>;
  };
  static_assert(call_31<std::uint16_t> == true);
  template<class T> concept call_32 = requires(T a) {
    { native::popcnt(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_32<std::uint32_t> == true);
  template<class T> concept call_33 = requires(T a) {
    { native::popcnt(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_33<std::uint64_t> == true);
  template<class T> concept call_34 = requires(T a) {
    { native::lzcnt(a) } noexcept -> std::same_as<std::uint16_t>;
  };
  static_assert(call_34<std::uint16_t> == true);
  template<class T> concept call_35 = requires(T a) {
    { native::lzcnt(a) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_35<std::uint32_t> == true);
  template<class T> concept call_36 = requires(T a) {
    { native::lzcnt(a) } noexcept -> std::same_as<std::uint64_t>;
  };
  static_assert(call_36<std::uint64_t> == true);
  template<class T> concept call_37 = requires(T a, std::uint8_t b) {
    { native::crc32c(a, b) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_37<std::uint32_t> == true);
  template<class T> concept call_38 = requires(T a, std::uint16_t b) {
    { native::crc32c(a, b) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_38<std::uint32_t> == true);
  template<class T> concept call_39 = requires(T a, std::uint32_t b) {
    { native::crc32c(a, b) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_39<std::uint32_t> == true);
  template<class T> concept call_40 = requires(T a, std::uint64_t b) {
    { native::crc32c(a, b) } noexcept -> std::same_as<std::uint32_t>;
  };
  static_assert(call_40<std::uint32_t> == true);
  template<class T> concept call_41 = requires(T a) {
    { native::cvtss_sh<7>(a) } noexcept -> std::same_as<std::uint16_t>;
  };
  static_assert(call_41<float> == (expected.has(native::x86_feature::f16c)));
  template<class T> concept call_42 = requires(T a) {
    { native::cvtsh_ss(a) } noexcept -> std::same_as<float>;
  };
  static_assert(call_42<std::uint16_t> == (expected.has(native::x86_feature::f16c)));

  // Below-feature defaults participate for immediate calls only. The negative
  // build fixtures reject calls with runtime inputs rather than probing syntax.
  static_assert(native::andn(std::uint32_t{0xf0}, std::uint32_t{0xff}) == 0xf);
  static_assert(native::pdep(std::uint32_t{3}, std::uint32_t{0x50}) == 0x50);
  static_assert(native::popcnt(std::uint16_t{0x5555}) == 8);
  static_assert(native::lzcnt(std::uint64_t{1}) == 63);
  static_assert(native::crc32c(std::uint32_t{0}, std::uint8_t{0}) == 0);
  static_assert(native::rorx<255>(std::uint64_t{1}) == 2);
  static_assert([] {
    std::uint64_t high = 0;
    auto low = native::mulx(~std::uint64_t{0}, ~std::uint64_t{0}, &high);
    return low == 1 && high == ~std::uint64_t{1};
  }());

  // Explicit tags remain valid independently of the provider baseline.
  template<unsigned I> concept rorx_immediate = requires(std::uint32_t a) {
    native::rorx<native::x86_feature::bmi2, I>(a);
    native::rorx<I, native::x86_feature::bmi2>(a);
  };
  template<unsigned I> concept half_immediate = requires(float a) {
    native::cvtss_sh<native::x86_feature::f16c, I>(a);
    native::cvtss_sh<I, native::x86_feature::f16c>(a);
  };
  static_assert(rorx_immediate<0> && rorx_immediate<255> && !rorx_immediate<256>);
  static_assert(half_immediate<0> && half_immediate<255> && !half_immediate<256>);
  template<unsigned I> concept weak_rotate_immediate = requires(std::uint64_t a) {
    native::rorx<native::isa<native::x86>{}, I>(a);
    native::rorx<I, native::isa<native::x86>{}>(a);
  };
  static_assert(weak_rotate_immediate<0> && weak_rotate_immediate<255> &&
                !weak_rotate_immediate<256>);

  // Existing integer-literal controls and scalar conversions remain accepted.
  static_assert(requires(std::uint64_t a, double d) {
    native::bextr<native::x86_feature::bmi1>(a, 3, 7);
    native::bzhi<native::x86_feature::bmi2>(a, 5);
    native::shlx<native::x86_feature::bmi2>(a, 4);
    native::cvtss_sh<0, native::x86_feature::f16c>(d);
    native::cvtsh_ss<native::x86_feature::f16c>(0x3c00);
  });
}
