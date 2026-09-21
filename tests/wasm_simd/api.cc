// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <type_traits>
#include <utility>
import native;

constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
template <class T> using vector = native::simd<T, 16 / sizeof(T), architecture>;

template <class V, class U>
concept scalar_operations = requires(V value, U scalar) {
  { value + scalar } -> std::same_as<V>;
  { scalar + value } -> std::same_as<V>;
  { value - scalar } -> std::same_as<V>;
  { scalar - value } -> std::same_as<V>;
  { value & scalar } -> std::same_as<V>;
  { scalar & value } -> std::same_as<V>;
  { value | scalar } -> std::same_as<V>;
  { scalar | value } -> std::same_as<V>;
  { value ^ scalar } -> std::same_as<V>;
  { scalar ^ value } -> std::same_as<V>;
  { value == scalar } -> std::same_as<typename V::mask_type>;
  { scalar == value } -> std::same_as<typename V::mask_type>;
  { value != scalar } -> std::same_as<typename V::mask_type>;
  { scalar != value } -> std::same_as<typename V::mask_type>;
  { value < scalar } -> std::same_as<typename V::mask_type>;
  { scalar < value } -> std::same_as<typename V::mask_type>;
  { value <= scalar } -> std::same_as<typename V::mask_type>;
  { scalar <= value } -> std::same_as<typename V::mask_type>;
  { value > scalar } -> std::same_as<typename V::mask_type>;
  { scalar > value } -> std::same_as<typename V::mask_type>;
  { value >= scalar } -> std::same_as<typename V::mask_type>;
  { scalar >= value } -> std::same_as<typename V::mask_type>;
  { value += scalar } -> std::same_as<V &>;
  { value -= scalar } -> std::same_as<V &>;
  { value &= scalar } -> std::same_as<V &>;
  { value |= scalar } -> std::same_as<V &>;
  { value ^= scalar } -> std::same_as<V &>;
};

template <class V, class U>
concept any_scalar_operation =
    requires(V v, U u) { v + u; } || requires(V v, U u) { u + v; } ||
    requires(V v, U u) { v - u; } || requires(V v, U u) { u - v; } ||
    requires(V v, U u) { v * u; } || requires(V v, U u) { u * v; } ||
    requires(V v, U u) { v & u; } || requires(V v, U u) { u & v; } ||
    requires(V v, U u) { v | u; } || requires(V v, U u) { u | v; } ||
    requires(V v, U u) { v ^ u; } || requires(V v, U u) { u ^ v; } ||
    requires(V v, U u) { v == u; } || requires(V v, U u) { u == v; } ||
    requires(V v, U u) { v != u; } || requires(V v, U u) { u != v; } ||
    requires(V v, U u) { v < u; } || requires(V v, U u) { u < v; } ||
    requires(V v, U u) { v <= u; } || requires(V v, U u) { u <= v; } ||
    requires(V v, U u) { v > u; } || requires(V v, U u) { u > v; } ||
    requires(V v, U u) { v >= u; } || requires(V v, U u) { u >= v; } ||
    requires(V v, U u) { v += u; } || requires(V v, U u) { v -= u; } ||
    requires(V v, U u) { v *= u; } || requires(V v, U u) { v &= u; } ||
    requires(V v, U u) { v |= u; } || requires(V v, U u) { v ^= u; };

enum integer_enum { one = 1 };
struct integer_conversion {
  constexpr operator int() const noexcept { return 1; }
};

template <class T, class U> consteval bool integer_participation() {
  using V = vector<T>;
  if constexpr (native::simd_integer_element<U>) {
    static_assert(std::is_convertible_v<U, V>);
    static_assert(scalar_operations<V, U>);
    if constexpr (sizeof(T) > 1) {
      static_assert(requires(V v, U u) { v * u; u * v; v *= u; });
    } else {
      static_assert(!requires(V v, U u) { v * u; });
    }
  } else {
    static_assert(!std::is_constructible_v<V, U>);
    static_assert(!std::is_assignable_v<V &, U>);
    static_assert(!any_scalar_operation<V, U>);
  }
  return []<std::size_t... I>(std::index_sequence<I...>) {
    constexpr bool accepts_lanes = requires(U value) { V{(static_cast<void>(I), value)...}; };
    static_assert(accepts_lanes == native::simd_integer_element<U>);
    return true;
  }(std::make_index_sequence<V::lanes>{});
}

template <class T> consteval bool integer_participation() {
  return integer_participation<T, std::int8_t>() &&
         integer_participation<T, std::uint8_t>() &&
         integer_participation<T, std::int16_t>() &&
         integer_participation<T, std::uint16_t>() &&
         integer_participation<T, std::int32_t>() &&
         integer_participation<T, std::uint32_t>() &&
         integer_participation<T, std::int64_t>() &&
         integer_participation<T, std::uint64_t>() &&
         integer_participation<T, bool>() && integer_participation<T, float>() &&
         integer_participation<T, double>() && integer_participation<T, long double>() &&
         integer_participation<T, integer_enum>() && integer_participation<T, integer_conversion>();
}

static_assert(integer_participation<std::int8_t>());
static_assert(integer_participation<std::uint8_t>());
static_assert(integer_participation<std::int16_t>());
static_assert(integer_participation<std::uint16_t>());
static_assert(integer_participation<std::int32_t>());
static_assert(integer_participation<std::uint32_t>());
static_assert(integer_participation<std::int64_t>());
static_assert(integer_participation<std::uint64_t>());

template <class T>
__attribute__((target("simd128"))) constexpr bool integer_values(std::uint64_t input) {
  using V = vector<T>;
  using U = std::make_unsigned_t<T>;
  auto expected = std::bit_cast<T>(static_cast<U>(input));
  V value(input);
  if (!all(value == V(expected)) || !all(V(std::int64_t(-1)) == V(T(-1))))
    return false;
  auto lanes = [&]<std::size_t... I>(std::index_sequence<I...>) __attribute__((target("simd128"))) {
    return V{(input + I)...};
  }(std::make_index_sequence<V::lanes>{});
  std::array<T, V::lanes> actual{};
  lanes.store(actual.data());
  for (std::size_t i = 0; i < V::lanes; ++i)
    if (actual[i] != std::bit_cast<T>(static_cast<U>(input + i)))
      return false;
  if (!all((value + input) == V(expected) + V(expected)) ||
      !all((input - value) == V(T(0))) ||
      !all((value & input) == value) || !all((input | value) == value) ||
      !all((value ^ input) == V(T(0))) ||
      !all(value == input) || any(value != input) ||
      any(value < input) || any(input > value) ||
      !all(value <= input) || !all(input >= value))
    return false;
  value += input;
  value -= input;
  value &= input;
  value |= input;
  value ^= input;
  if (any(value))
    return false;
  if constexpr (sizeof(T) > 1) {
    value = input;
    value *= input;
    if (!all(value == V(expected) * V(expected)) ||
        !all(input * V(expected) == value))
      return false;
  }
  return true;
}

static_assert(integer_values<std::int8_t>(0xfedcba9876543280ull));
static_assert(integer_values<std::uint8_t>(0xfedcba9876543280ull));
static_assert(integer_values<std::int16_t>(0xfedcba9876548000ull));
static_assert(integer_values<std::uint16_t>(0xfedcba9876548000ull));
static_assert(integer_values<std::int32_t>(0xfedcba9880000000ull));
static_assert(integer_values<std::uint32_t>(0xfedcba9880000000ull));
static_assert(integer_values<std::int64_t>(0x8000000000000000ull));
static_assert(integer_values<std::uint64_t>(0x8000000000000000ull));

template <class T>
__attribute__((target("simd128"))) constexpr bool floating_values(T input) {
  using V = vector<T>;
  static_assert(std::is_constructible_v<V, int> && std::is_constructible_v<V, double>);
  static_assert(std::is_constructible_v<V, bool> && std::is_constructible_v<V, integer_conversion>);
  return all(V(input) + 0.5 == V(T(input + T(0.5)))) &&
         all(V(2) == V(T(2))) && all(V(true) == V(T(1))) &&
         all(V(integer_conversion{}) == V(T(1)));
}

static_assert(floating_values<float>(1.25f));
static_assert(floating_values<double>(1.25));
volatile std::uint64_t runtime_input = 0xfedcba9880000000ull;
volatile double runtime_float = 1.25;

__attribute__((target("simd128"), noinline)) bool runtime() {
  auto input = runtime_input;
  return integer_values<std::int8_t>(input) && integer_values<std::uint8_t>(input) &&
         integer_values<std::int16_t>(input) && integer_values<std::uint16_t>(input) &&
         integer_values<std::int32_t>(input) && integer_values<std::uint32_t>(input) &&
         integer_values<std::int64_t>(input) && integer_values<std::uint64_t>(input) &&
         floating_values<float>(float(runtime_float)) && floating_values<double>(runtime_float);
}

int main() {
  return runtime() ? 0 : 1;
}
