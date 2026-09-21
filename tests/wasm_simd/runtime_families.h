// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

__attribute__((target("simd128"))) bool runtime_mixed() {
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<std::int16_t, 8> a{}, b{}, product{};
    std::array<std::int32_t, 4> dot{};
    for (unsigned i = 0; i < 8; ++i) {
      a[i] = std::int16_t(random_word());
      b[i] = std::int16_t(random_word());
      product[i] = limit<std::int16_t>((std::int64_t(a[i]) * b[i] + 16384) >> 15);
    }
    // Include the one saturating multiplication case and the wrapping dot case.
    a[0] = a[1] = b[0] = b[1] = INT16_MIN;
    product[0] = product[1] = INT16_MAX;
    for (unsigned i = 0; i < 4; ++i) {
      auto sum = std::int64_t(a[2 * i]) * b[2 * i] + std::int64_t(a[2 * i + 1]) * b[2 * i + 1];
      dot[i] = std::bit_cast<std::int32_t>(std::uint32_t(sum));
    }
    if (!equals(native::q15mulr_sat(V<std::int16_t>(a), V<std::int16_t>(b)), product) ||
        !equals(native::dot(V<std::int16_t>(a), V<std::int16_t>(b)), dot))
      return false;
    std::array<std::uint8_t, 16> bytes{}, indices{}, expected{};
    for (unsigned i = 0; i < 16; ++i) {
      bytes[i] = std::uint8_t(random_word());
      indices[i] = std::uint8_t(random_word());
    }
    indices[0] = 0;
    indices[1] = 15;
    indices[2] = 16;
    indices[3] = 255;
    for (unsigned i = 0; i < 16; ++i)
      expected[i] = indices[i] < 16 ? bytes[indices[i]] : 0;
    if (!equals(native::swizzle(V<std::uint8_t>(bytes), V<std::uint8_t>(indices)), expected))
      return false;
    std::array<std::uint32_t, 4> words{};
    for (auto &word : words)
      word = std::uint32_t(random_word());
    V<std::uint32_t> p(words);
    if (!equals(native::shuffle<3, 5, 1, 7>(p, p),
                std::array{words[3], words[1], words[1], words[3]}))
      return false;
    auto scalar = std::uint32_t(random_word());
    if (native::load_lane<3>(&scalar, p).get<3>() != scalar)
      return false;
    native::store_lane<2>(&scalar, p);
    if (scalar != words[2] ||
        !equals(native::load_zero<V<std::uint32_t>>(&scalar), std::array{scalar, 0u, 0u, 0u}) ||
        !all(native::load_splat<V<std::uint32_t>>(&scalar) == V<std::uint32_t>(scalar)))
      return false;
    std::array<std::uint32_t, 6> out{9u, 9u, 9u, 9u, 9u, 9u};
    auto count = trial % 5;
    native::store_simd_partial(out.data() + 1, p, count);
    for (unsigned i = 0; i < 6; ++i)
      if (out[i] != (i > 0 && i <= count ? words[i - 1] : 9u))
        return false;
    auto partial = native::load_simd_partial<V<std::uint32_t>>(out.data() + 1, count, 8u);
    for (unsigned i = count; i < 4; ++i)
      words[i] = 8;
    if (!equals(partial, words))
      return false;
  }
  native::store_simd_partial(static_cast<std::uint32_t *>(nullptr), V<std::uint32_t>(1u), 0);
  return all(native::load_simd_partial<V<std::uint32_t>>(
                 static_cast<std::uint32_t const *>(nullptr), 0, 7u) == V<std::uint32_t>(7u));
}

template <class To, class From> To scalar_saturation(From value) {
  if (std::isnan(value))
    return 0;
  auto truncated = std::trunc(double(value));
  if (truncated <= double(std::numeric_limits<To>::min()))
    return std::numeric_limits<To>::min();
  if (truncated >= double(std::numeric_limits<To>::max()))
    return std::numeric_limits<To>::max();
  return To(truncated);
}

__attribute__((target("simd128"))) bool runtime_conversions() {
  for (unsigned trial = 0; trial < 128; ++trial) {
    std::array<float, 4> f{}, fexpected{};
    std::array<double, 2> d{}, dexpected{};
    std::array<std::int32_t, 4> si{}, sexpected{};
    std::array<std::uint32_t, 4> ui{}, uexpected{};
    for (unsigned i = 0; i < 4; ++i) {
      f[i] = std::bit_cast<float>(std::uint32_t(random_word()));
      si[i] = std::int32_t(random_word());
      ui[i] = std::uint32_t(random_word());
      sexpected[i] = scalar_saturation<std::int32_t>(f[i]);
      uexpected[i] = scalar_saturation<std::uint32_t>(f[i]);
    }
    if (!equals(native::trunc_sat<std::int32_t>(V<float>(f)), sexpected) ||
        !equals(native::trunc_sat<std::uint32_t>(V<float>(f)), uexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i) {
      d[i] = std::bit_cast<double>(random_word());
      sexpected[i] = scalar_saturation<std::int32_t>(d[i]);
      uexpected[i] = scalar_saturation<std::uint32_t>(d[i]);
    }
    sexpected[2] = sexpected[3] = 0;
    uexpected[2] = uexpected[3] = 0;
    if (!equals(native::trunc_sat<std::int32_t>(V<double>(d)), sexpected) ||
        !equals(native::trunc_sat<std::uint32_t>(V<double>(d)), uexpected))
      return false;
    for (unsigned i = 0; i < 4; ++i)
      fexpected[i] = float(si[i]);
    if (!equals(native::convert<float>(V<std::int32_t>(si)), fexpected))
      return false;
    for (unsigned i = 0; i < 4; ++i)
      fexpected[i] = float(ui[i]);
    if (!equals(native::convert<float>(V<std::uint32_t>(ui)), fexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i)
      dexpected[i] = double(si[i]);
    if (!equals(native::convert<double>(V<std::int32_t>(si)), dexpected))
      return false;
    for (unsigned i = 0; i < 2; ++i)
      dexpected[i] = double(ui[i]);
    if (!equals(native::convert<double>(V<std::uint32_t>(ui)), dexpected))
      return false;
    auto promoted = native::convert<double>(V<float>(f));
    auto demoted = native::convert<float>(V<double>(d));
    promoted.store(dexpected.data());
    demoted.store(fexpected.data());
    for (unsigned i = 0; i < 2; ++i)
      if (!fp_equal(dexpected[i], double(f[i])) || !fp_equal(fexpected[i], float(d[i])))
        return false;
    if (!fp_equal(fexpected[2], 0.f) || !fp_equal(fexpected[3], 0.f))
      return false;
  }
  return true;
}
