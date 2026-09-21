// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included only in the SIMD128 backend target scope.
namespace native {
  namespace detail {
    template <class T>
    concept wasm_number =
        simd_integer_element<T> || std::same_as<T, float> || std::same_as<T, double>;
    template <class T>
    using wasm_word = std::conditional_t<
        sizeof(T) == 1, std::uint8_t,
        std::conditional_t<sizeof(T) == 2, std::uint16_t,
                           std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>>>;
    template <class T>
    using wasm_format =
        std::conditional_t<sizeof(T) == 4, constexpr_float::binary32, constexpr_float::binary64>;
    template <class V> constexpr auto wasm_lanes(V v) noexcept {
      std::array<typename V::value_type, V::lanes> a{};
      v.store(a.data());
      return a;
    }
    template <class V, class F, class... W> constexpr V wasm_map(F f, V v, W... w) noexcept {
      auto inputs = std::tuple{wasm_lanes(v), wasm_lanes(w)...};
      std::array<typename V::value_type, V::lanes> r{};
      for (std::size_t i = 0; i < V::lanes; ++i)
        r[i] = std::apply([&](auto const &...a) { return f(a[i]...); }, inputs);
      return V::load(r.data());
    }
    template <class T, std::size_t N, isa<> A>
      requires ordinary_simd_element<T> && NATIVE_ARCH_REQUIRES
    (A) && (wasm_number<T> || simd_mask_element<T>) &&
        (sizeof(T) * N == 16) struct value_traits<simd<T, N, A>> {
      static constexpr isa<> value = A;
      static constexpr bool known = true;
      static constexpr bool aggregate_default = false;
    };
  } // namespace detail

  /// Canonical zero/all-one SIMD128 comparison lanes.
  template <class U, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(U) * N == 16) struct alignas(16) simd<mask_lane<U>, N, A> {
    using value_type = mask_lane<U>;
    using native_type = v128_t;
    using mask_type = simd;
    using mask = simd;
    template <class T> using rebind = simd<T, N, A>;
    static constexpr auto architecture = A;
    static constexpr std::size_t lanes = N;
    static constexpr bool compact = false;

  private:
    native_type value_{};

  public:
    /// Initialize every lane to zero.
    constexpr simd() noexcept = default;
    /// Broadcast one truth value as canonical zero/all-one lanes.
    explicit native_inline constexpr simd(bool x) noexcept
        : simd(from_bitset(x ? ~std::uint64_t{} : 0)) {}
    /// Return the implementation register without changing its bits.
    native_inline constexpr native_type to_native() const noexcept {
      return value_;
    }
    /// Adopt implementation bits; mask callers must supply canonical lanes.
    static native_inline constexpr simd unsafe_from_native(native_type x) noexcept {
      simd r;
      r.value_ = x;
      return r;
    }
    /// Adopt implementation storage; mask specializations normalize nonzero lanes.
    static native_inline constexpr simd from_native(native_type x) noexcept {
      if consteval {
        auto words = __builtin_bit_cast(std::array<U, N>, x);
        for (auto &w : words)
          w = w ? U(~U(0)) : U(0);
        return unsafe_from_native(__builtin_bit_cast(native_type, words));
      } else {
        if constexpr (sizeof(U) == 1)
          return unsafe_from_native(wasm_i8x16_ne(x, wasm_i32x4_splat(0)));
        else if constexpr (sizeof(U) == 2)
          return unsafe_from_native(wasm_i16x8_ne(x, wasm_i32x4_splat(0)));
        else if constexpr (sizeof(U) == 4)
          return unsafe_from_native(wasm_i32x4_ne(x, wasm_i32x4_splat(0)));
        else if constexpr (sizeof(U) == 8)
          return unsafe_from_native(wasm_i64x2_ne(x, wasm_i32x4_splat(0)));
      }
    }
    /// Expand low scalar bits to canonical mask lanes, ignoring excess bits.
    /// Adopt unsigned lane representations without numerical conversion.
    static native_inline constexpr simd from_bitset(std::uint64_t bits) noexcept {
      std::array<U, N> a{};
      for (std::size_t i = 0; i < N; ++i)
        a[i] = ((bits >> i) & 1) ? U(~U(0)) : U(0);
      return unsafe_from_native(__builtin_bit_cast(native_type, a));
    }
    /// Gather lane truth values into low scalar bits.
    native_inline constexpr std::uint64_t to_bitset() const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<U, N>, value_);
        std::uint64_t r = 0;
        for (std::size_t i = 0; i < N; ++i)
          r |= std::uint64_t(a[i] != 0) << i;
        return r;
      } else {
        if constexpr (sizeof(U) == 1)
          return wasm_i8x16_bitmask(value_);
        else if constexpr (sizeof(U) == 2)
          return wasm_i16x8_bitmask(value_);
        else if constexpr (sizeof(U) == 4)
          return wasm_i32x4_bitmask(value_);
        else if constexpr (sizeof(U) == 8)
          return wasm_i64x2_bitmask(value_);
      }
    }
    /// Return the compact lane truth bitset.
    native_inline constexpr std::uint64_t bits() const noexcept {
      return to_bitset();
    }
    /// Read exactly N canonical mask lane objects.
    static native_inline constexpr simd load(value_type const *p) noexcept {
      std::array<U, N> a{};
      for (std::size_t i = 0; i < N; ++i)
        a[i] = p[i].to_bits();
      return unsafe_from_native(__builtin_bit_cast(native_type, a));
    }
    /// Write exactly N canonical mask lane objects.
    native_inline constexpr void store(value_type *p) const noexcept {
      auto a = __builtin_bit_cast(std::array<U, N>, value_);
      for (std::size_t i = 0; i < N; ++i)
        p[i] = value_type::from_bits(a[i]);
    }
    /// Load all lanes; Align is the caller-provided pointer alignment.
    template <std::size_t Align>
    static native_inline constexpr simd load_memory(value_type const *p) noexcept {
      return load(p);
    }
    /// Store all lanes; Align is the caller-provided pointer alignment.
    template <std::size_t Align>
    native_inline constexpr void store_memory(value_type *p) const noexcept {
      store(p);
    }
    /// Test whether at least one lane is true.
    friend native_inline constexpr bool any(simd v) noexcept {
      if consteval {
        return v.to_bitset() != 0;
      } else {
        return wasm_v128_any_true(v.value_);
      }
    }
    /// Test whether every lane is true.
    friend native_inline constexpr bool all(simd v) noexcept {
      return v.to_bitset() == ((std::uint64_t{1} << N) - 1);
    }
    /// Test whether no lane is true.
    friend native_inline constexpr bool none(simd v) noexcept {
      return !any(v);
    }
    /// Intersect representation bits.
    friend native_inline constexpr simd operator&(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() & b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_and(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator&=(simd b) noexcept {
      return *this = *this & b;
    }
    /// Unite representation bits.
    friend native_inline constexpr simd operator|(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() | b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_or(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator|=(simd b) noexcept {
      return *this = *this | b;
    }
    /// Exclusive-or representation bits.
    friend native_inline constexpr simd operator^(simd a, simd b) noexcept {
      if consteval {
        return from_bitset(a.to_bitset() ^ b.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_xor(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator^=(simd b) noexcept {
      return *this = *this ^ b;
    }
    /// Complement every representation bit (or lane truth for masks).
    friend native_inline constexpr simd operator~(simd a) noexcept {
      if consteval {
        return from_bitset(~a.to_bitset());
      } else {
        return unsafe_from_native(wasm_v128_not(a.value_));
      }
    }
    /// Complement every mask lane.
    friend native_inline constexpr simd operator!(simd a) noexcept {
      return ~a;
    }
    /// Compare lane equality and return canonical mask lanes.
    friend native_inline constexpr simd operator==(simd a, simd b) noexcept {
      return ~(a ^ b);
    }
    /// Compare lane inequality and return canonical mask lanes.
    friend native_inline constexpr simd operator!=(simd a, simd b) noexcept {
      return a ^ b;
    }
    /// Choose mask lanes from a where m is true, otherwise from b.
    friend native_inline constexpr simd select(simd m, simd a, simd b) noexcept {
      return (m & a) | (~m & b);
    }
  };

  /// One complete WebAssembly SIMD128 register with semantic element lanes.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) struct alignas(16) simd<T, N, A> {
    using value_type = T;
    using native_type = v128_t;
    using word_type = detail::wasm_word<T>;
    using mask_type = simd<mask_lane<word_type>, N, A>;
    using mask = mask_type;
    using bits_type = simd<word_type, N, A>;
    using register_type = simd;
    template <class U> using rebind = simd<U, N, A>;
    static constexpr auto architecture = A;
    static constexpr std::size_t lanes = N;

  private:
    native_type value_{};

  public:
    /// Initialize every lane to zero.
    constexpr simd() noexcept = default;
    /// Broadcast the element value to every lane.
    native_inline constexpr simd(T x) noexcept {
      if consteval {
        std::array<T, N> a{};
        a.fill(x);
        value_ = __builtin_bit_cast(native_type, a);
      } else {
        if constexpr (std::same_as<T, float>)
          value_ = wasm_f32x4_splat(x);
        else if constexpr (std::same_as<T, double>)
          value_ = wasm_f64x2_splat(x);
        else if constexpr (std::same_as<T, std::int8_t>)
          value_ = wasm_i8x16_splat(x);
        else if constexpr (std::same_as<T, std::uint8_t>)
          value_ = wasm_u8x16_splat(x);
        else if constexpr (std::same_as<T, std::int16_t>)
          value_ = wasm_i16x8_splat(x);
        else if constexpr (std::same_as<T, std::uint16_t>)
          value_ = wasm_u16x8_splat(x);
        else if constexpr (std::same_as<T, std::int32_t>)
          value_ = wasm_i32x4_splat(x);
        else if constexpr (std::same_as<T, std::uint32_t>)
          value_ = wasm_u32x4_splat(x);
        else if constexpr (std::same_as<T, std::int64_t>)
          value_ = wasm_i64x2_splat(x);
        else if constexpr (std::same_as<T, std::uint64_t>)
          value_ = wasm_u64x2_splat(x);
      }
    }
    /// Copy the array in lane order.
    native_inline constexpr explicit simd(std::array<T, N> const &a) noexcept
        : simd(load(a.data())) {}
    /// Construct exactly N lanes from element-typed arguments.
    template <class... U>
      requires(sizeof...(U) == N && (std::same_as<U, T> && ...))
    native_inline constexpr simd(U... x) noexcept : simd(std::array<T, N>{x...}) {}
    /// Return the implementation register without changing its bits.
    native_inline constexpr native_type to_native() const noexcept {
      return value_;
    }
    /// Adopt implementation storage; mask specializations normalize nonzero lanes.
    static native_inline constexpr simd from_native(native_type x) noexcept {
      simd r;
      r.value_ = x;
      return r;
    }
    /// Adopt implementation bits; mask callers must supply canonical lanes.
    static native_inline constexpr simd unsafe_from_native(native_type x) noexcept {
      return from_native(x);
    }
    /// Load exactly 16 unaligned bytes as N element objects.
    static native_inline constexpr simd load(T const *p) noexcept {
      if consteval {
        std::array<T, N> a{};
        for (std::size_t i = 0; i < N; ++i)
          a[i] = p[i];
        return from_native(__builtin_bit_cast(native_type, a));
      } else {
        return from_native(wasm_v128_load(p));
      }
    }
    /// Store exactly 16 unaligned bytes in lane order.
    native_inline constexpr void store(T *p) const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<T, N>, value_);
        for (std::size_t i = 0; i < N; ++i)
          p[i] = a[i];
      } else {
        wasm_v128_store(p, value_);
      }
    }
    /// Load all lanes; Align is the caller-provided pointer alignment.
    template <std::size_t Align>
    static native_inline constexpr simd load_memory(T const *p) noexcept {
      if consteval {
        return load(p);
      } else {
        return load(static_cast<T const *>(__builtin_assume_aligned(p, Align)));
      }
    }
    /// Store all lanes; Align is the caller-provided pointer alignment.
    template <std::size_t Align> native_inline constexpr void store_memory(T *p) const noexcept {
      if consteval {
        store(p);
      } else {
        store(static_cast<T *>(__builtin_assume_aligned(p, Align)));
      }
    }
    /// Read n lanes and fill the rest; require n <= N. Null is valid when n is zero.
    static native_inline constexpr simd load_partial(T const *p, std::size_t n,
                                                     T fill = {}) noexcept {
      std::array<T, N> a{};
      a.fill(fill);
      for (std::size_t i = 0; i < n; ++i)
        a[i] = p[i];
      return load(a.data());
    }
    /// Write only the first n lanes; require n <= N. Null is valid when n is zero.
    native_inline constexpr void store_partial(T *p, std::size_t n) const noexcept {
      auto a = detail::wasm_lanes(*this);
      for (std::size_t i = 0; i < n; ++i)
        p[i] = a[i];
    }
    /// Load lane representations from corresponding unsigned words.
    static native_inline constexpr simd load_bits(word_type const *p) noexcept {
      return from_native(bits_type::load(p).to_native());
    }
    /// Store lane representations as corresponding unsigned words.
    native_inline constexpr void store_bits(word_type *p) const noexcept {
      bits().store(p);
    }
    /// Reinterpret each lane as an unsigned word of the same width.
    native_inline constexpr bits_type bits() const noexcept {
      return bits_type::from_native(value_);
    }
    /// Adopt unsigned lane representations without numerical conversion.
    static native_inline constexpr simd from_bits(bits_type x) noexcept {
      return from_native(x.to_native());
    }
    /// Extract the compile-time-selected lane as its scalar element type.
    template <std::size_t I>
      requires(I < N)
    native_inline constexpr T get() const noexcept {
      if consteval {
        return __builtin_bit_cast(std::array<T, N>, value_)[I];
      } else {
        if constexpr (std::same_as<T, float>)
          return wasm_f32x4_extract_lane(value_, I);
        else if constexpr (std::same_as<T, double>)
          return wasm_f64x2_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::int8_t>)
          return wasm_i8x16_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::uint8_t>)
          return wasm_u8x16_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::int16_t>)
          return wasm_i16x8_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::uint16_t>)
          return wasm_u16x8_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::int32_t>)
          return wasm_i32x4_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::uint32_t>)
          return wasm_u32x4_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::int64_t>)
          return wasm_i64x2_extract_lane(value_, I);
        else if constexpr (std::same_as<T, std::uint64_t>)
          return wasm_u64x2_extract_lane(value_, I);
      }
    }
    /// Replace one compile-time-selected lane and preserve every other lane.
    template <std::size_t I>
      requires(I < N)
    native_inline constexpr simd replace(T x) const noexcept {
      if consteval {
        auto a = __builtin_bit_cast(std::array<T, N>, value_);
        a[I] = x;
        return load(a.data());
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_u8x16_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_u16x8_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_u32x4_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_replace_lane(value_, I, x));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_u64x2_replace_lane(value_, I, x));
      }
    }
    /// Add lanes; integer results wrap and floating results round to nearest-even.
    friend native_inline constexpr simd operator+(simd a, simd b) noexcept {
      if consteval {
        return detail::wasm_map(
            [](T x, T y) {
              if constexpr (std::is_floating_point_v<T>) {
                using F = detail::wasm_format<T>;
                return std::bit_cast<T>(detail::constexpr_float::add_bits<F>(
                    std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
              } else {
                return std::bit_cast<T>(
                    word_type(std::uint64_t(word_type(x)) + std::uint64_t(word_type(y))));
              }
            },
            a, b);
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_i8x16_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_i16x8_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_i32x4_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_add(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_i64x2_add(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator+=(simd b) noexcept {
      return *this = *this + b;
    }
    /// Subtract or negate lanes; integer results wrap and floating signs are preserved.
    friend native_inline constexpr simd operator-(simd a, simd b) noexcept {
      if consteval {
        return detail::wasm_map(
            [](T x, T y) {
              if constexpr (std::is_floating_point_v<T>) {
                using F = detail::wasm_format<T>;
                return std::bit_cast<T>(detail::constexpr_float::sub_bits<F>(
                    std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
              } else {
                return std::bit_cast<T>(
                    word_type(std::uint64_t(word_type(x)) - std::uint64_t(word_type(y))));
              }
            },
            a, b);
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_i8x16_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_i16x8_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_i32x4_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_sub(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_i64x2_sub(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator-=(simd b) noexcept {
      return *this = *this - b;
    }
    /// Multiply lanes; integer results wrap and floating results round to nearest-even.
    friend native_inline constexpr simd operator*(simd a, simd b) noexcept
      requires(sizeof(T) > 1)
    {
      if consteval {
        return detail::wasm_map(
            [](T x, T y) {
              if constexpr (std::is_floating_point_v<T>) {
                using F = detail::wasm_format<T>;
                return std::bit_cast<T>(detail::constexpr_float::mul_bits<F>(
                    std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
              } else {
                return std::bit_cast<T>(
                    word_type(std::uint64_t(word_type(x)) * std::uint64_t(word_type(y))));
              }
            },
            a, b);
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_i16x8_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_i32x4_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_mul(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_i64x2_mul(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator*=(simd b) noexcept
      requires(sizeof(T) > 1)
    {
      return *this = *this * b;
    }
    /// Divide floating lanes with WebAssembly IEEE semantics.
    friend native_inline constexpr simd operator/(simd a, simd b) noexcept
      requires std::is_floating_point_v<T>
    {
      if consteval {
        return detail::wasm_map(
            [](T x, T y) {
              if constexpr (std::is_floating_point_v<T>) {
                using F = detail::wasm_format<T>;
                return std::bit_cast<T>(detail::constexpr_float::div_bits<F>(
                    std::bit_cast<word_type>(x), std::bit_cast<word_type>(y)));
              } else {
                return std::bit_cast<T>(
                    word_type(std::uint64_t(word_type(x)) / std::uint64_t(word_type(y))));
              }
            },
            a, b);
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_div(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_div(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator/=(simd b) noexcept
      requires std::is_floating_point_v<T>
    {
      return *this = *this / b;
    }
    /// Intersect representation bits.
    friend native_inline constexpr simd operator&(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i)
          x[i] &= y[i];
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_and(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator&=(simd b) noexcept {
      return *this = *this & b;
    }
    /// Unite representation bits.
    friend native_inline constexpr simd operator|(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i)
          x[i] |= y[i];
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_or(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator|=(simd b) noexcept {
      return *this = *this | b;
    }
    /// Exclusive-or representation bits.
    friend native_inline constexpr simd operator^(simd a, simd b) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        auto y = __builtin_bit_cast(std::array<word_type, N>, b.value_);
        for (std::size_t i = 0; i < N; ++i)
          x[i] ^= y[i];
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_xor(a.value_, b.value_));
      }
    }
    /// Apply the corresponding lane operation and update this value.
    native_inline constexpr simd &operator^=(simd b) noexcept {
      return *this = *this ^ b;
    }
    /// Complement every representation bit (or lane truth for masks).
    friend native_inline constexpr simd operator~(simd a) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        for (auto &w : x)
          w = word_type(~w);
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        return from_native(wasm_v128_not(a.value_));
      }
    }
    /// Subtract or negate lanes; integer results wrap and floating signs are preserved.
    friend native_inline constexpr simd operator-(simd a) noexcept {
      if consteval {
        auto x = __builtin_bit_cast(std::array<word_type, N>, a.value_);
        for (auto &w : x) {
          if constexpr (std::is_floating_point_v<T>)
            w ^= word_type{1} << (sizeof(T) * 8 - 1);
          else
            w = word_type(0 - w);
        }
        return from_native(__builtin_bit_cast(native_type, x));
      } else {
        if constexpr (std::same_as<T, float>)
          return from_native(wasm_f32x4_neg(a.value_));
        else if constexpr (std::same_as<T, double>)
          return from_native(wasm_f64x2_neg(a.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_neg(a.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_i8x16_neg(a.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_neg(a.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_i16x8_neg(a.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_neg(a.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_i32x4_neg(a.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_neg(a.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_i64x2_neg(a.value_));
      }
    }
    /// Compare lane equality and return canonical mask lanes.
    friend native_inline constexpr mask_type operator==(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] == y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_eq(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_eq(a.value_, b.value_));
      }
    }
    /// Compare lane inequality and return canonical mask lanes.
    friend native_inline constexpr mask_type operator!=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] != y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_ne(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_ne(a.value_, b.value_));
      }
    }
    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator<(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] < y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_u8x16_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_u16x8_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_u32x4_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_lt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(
              wasm_i64x2_lt(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                            wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
      }
    }
    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator<=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] <= y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_u8x16_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_u16x8_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_u32x4_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_le(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(
              wasm_i64x2_le(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                            wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
      }
    }
    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator>(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] > y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_u8x16_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_u16x8_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_u32x4_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_gt(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(
              wasm_i64x2_gt(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                            wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
      }
    }
    /// Compare lanes using their signedness; unordered floating lanes are false.
    friend native_inline constexpr mask_type operator>=(simd a, simd b) noexcept {
      if consteval {
        auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
        std::uint64_t bits = 0;
        for (std::size_t i = 0; i < N; ++i)
          bits |= std::uint64_t(x[i] >= y[i]) << i;
        return mask_type::from_bitset(bits);
      } else {
        if constexpr (std::same_as<T, float>)
          return mask_type::unsafe_from_native(wasm_f32x4_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, double>)
          return mask_type::unsafe_from_native(wasm_f64x2_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int8_t>)
          return mask_type::unsafe_from_native(wasm_i8x16_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return mask_type::unsafe_from_native(wasm_u8x16_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int16_t>)
          return mask_type::unsafe_from_native(wasm_i16x8_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return mask_type::unsafe_from_native(wasm_u16x8_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int32_t>)
          return mask_type::unsafe_from_native(wasm_i32x4_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return mask_type::unsafe_from_native(wasm_u32x4_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::int64_t>)
          return mask_type::unsafe_from_native(wasm_i64x2_ge(a.value_, b.value_));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return mask_type::unsafe_from_native(
              wasm_i64x2_ge(wasm_v128_xor(a.value_, wasm_i64x2_splat(INT64_MIN)),
                            wasm_v128_xor(b.value_, wasm_i64x2_splat(INT64_MIN))));
      }
    }
    /// WebAssembly shifts reduce the count modulo the lane width.
    native_inline constexpr simd left(unsigned count) const noexcept
      requires simd_integer_element<T>
    {
      count %= sizeof(T) * 8;
      if consteval {
        return detail::wasm_map(
            [&](T x) { return std::bit_cast<T>(word_type(std::uint64_t(word_type(x)) << count)); },
            *this);
      } else {
        if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_shl(value_, count));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_i8x16_shl(value_, count));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_shl(value_, count));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_i16x8_shl(value_, count));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_shl(value_, count));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_i32x4_shl(value_, count));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_shl(value_, count));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_i64x2_shl(value_, count));
      }
    }
    template <std::size_t K>
    native_inline constexpr simd left() const noexcept
      requires simd_integer_element<T>
    {
      return left(K);
    }
    /// Shift integer lanes left, reducing the count modulo the lane width.
    friend native_inline constexpr simd operator<<(simd a, unsigned n) noexcept
      requires simd_integer_element<T>
    {
      return a.left(n);
    }
    /// Shift integer lanes left, reducing the count modulo the lane width.
    template <std::size_t K>
    friend native_inline constexpr simd operator<<(simd a, imm_t<K>) noexcept
      requires simd_integer_element<T>
    {
      return a.left(K);
    }
    /// WebAssembly shifts reduce the count modulo the lane width.
    native_inline constexpr simd right(unsigned count) const noexcept
      requires simd_integer_element<T>
    {
      count %= sizeof(T) * 8;
      if consteval {
        return detail::wasm_map([&](T x) { return T(x >> count); }, *this);
      } else {
        if constexpr (std::same_as<T, std::int8_t>)
          return from_native(wasm_i8x16_shr(value_, count));
        else if constexpr (std::same_as<T, std::uint8_t>)
          return from_native(wasm_u8x16_shr(value_, count));
        else if constexpr (std::same_as<T, std::int16_t>)
          return from_native(wasm_i16x8_shr(value_, count));
        else if constexpr (std::same_as<T, std::uint16_t>)
          return from_native(wasm_u16x8_shr(value_, count));
        else if constexpr (std::same_as<T, std::int32_t>)
          return from_native(wasm_i32x4_shr(value_, count));
        else if constexpr (std::same_as<T, std::uint32_t>)
          return from_native(wasm_u32x4_shr(value_, count));
        else if constexpr (std::same_as<T, std::int64_t>)
          return from_native(wasm_i64x2_shr(value_, count));
        else if constexpr (std::same_as<T, std::uint64_t>)
          return from_native(wasm_u64x2_shr(value_, count));
      }
    }
    template <std::size_t K>
    native_inline constexpr simd right() const noexcept
      requires simd_integer_element<T>
    {
      return right(K);
    }
    /// Shift integer lanes right, extending signed lanes and reducing the count modulo their width.
    friend native_inline constexpr simd operator>>(simd a, unsigned n) noexcept
      requires simd_integer_element<T>
    {
      return a.right(n);
    }
    /// Shift integer lanes right, extending signed lanes and reducing the count modulo their width.
    template <std::size_t K>
    friend native_inline constexpr simd operator>>(simd a, imm_t<K>) noexcept
      requires simd_integer_element<T>
    {
      return a.right(K);
    }
  };
  /// Select full lanes from a where m is true, otherwise from b.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> select(typename simd<T, N, A>::mask_type m, simd<T, N, A> a,
                                     simd<T, N, A> b) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      auto bits = m.to_bitset();
      for (std::size_t i = 0; i < N; ++i)
        if (!((bits >> i) & 1))
          x[i] = y[i];
      return simd<T, N, A>::load(x.data());
    } else {
      return simd<T, N, A>::from_native(
          wasm_v128_bitselect(a.to_native(), b.to_native(), m.to_native()));
    }
  }
  /// Select individual representation bits using the unsigned mask words.
  template <detail::wasm_number T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> bit_select(simd<detail::wasm_word<T>, N, A> m, simd<T, N, A> a,
                                         simd<T, N, A> b) noexcept {
    return simd<T, N, A>::from_bits((m & a.bits()) | (~m & b.bits()));
  }
  /// Reinterpret canonical mask lanes as corresponding integer words.
  template <class To, class U, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(U) * N == 16) && (sizeof(To) == sizeof(U)) &&
      simd_integer_element<To> native_inline
      constexpr simd<To, N, A> mask_bits(simd<mask_lane<U>, N, A> m) noexcept {
    return simd<To, N, A>::from_native(m.to_native());
  }
  /// Broadcast one compile-time-selected lane.
  template <std::size_t I, detail::wasm_number T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) &&
      (I < N) native_inline constexpr simd<T, N, A> broadcast(simd<T, N, A> v, imm_t<I>) noexcept {
    return simd<T, N, A>(v.template get<I>());
  }
  /// Select N lanes from the concatenation of two inputs, using constant indices.
  template <std::size_t... I, detail::wasm_number T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) && (sizeof...(I) == N) &&
      ((I < 2 * N) && ...) native_inline
      constexpr simd<T, N, A> shuffle(simd<T, N, A> a, simd<T, N, A> b) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(a), y = detail::wasm_lanes(b);
      std::array<T, N> r{(I < N ? x[I] : y[I - N])...};
      return simd<T, N, A>::load(r.data());
    } else {
      using V = T __attribute__((ext_vector_type(N)));
      return simd<T, N, A>::from_native(__builtin_bit_cast(
          v128_t, __builtin_shufflevector(__builtin_bit_cast(V, a.to_native()),
                                          __builtin_bit_cast(V, b.to_native()), I...)));
    }
  }
  /// Look up byte indices 0..15; every other index produces zero.
  template <isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) native_inline
      constexpr simd<std::uint8_t, 16, A> swizzle(simd<std::uint8_t, 16, A> v,
                                                  simd<std::uint8_t, 16, A> indices) noexcept {
    if consteval {
      auto x = detail::wasm_lanes(v), i = detail::wasm_lanes(indices);
      for (auto &n : i)
        n = n < 16 ? x[n] : 0;
      return simd<std::uint8_t, 16, A>::load(i.data());
    } else {
      return simd<std::uint8_t, 16, A>::from_native(
          wasm_i8x16_swizzle(v.to_native(), indices.to_native()));
    }
  }
  /// Compute correctly rounded square roots; negative finite lanes produce NaN.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> sqrt(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(detail::constexpr_float::sqrt_bits<F>(w));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_sqrt(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_sqrt(v.to_native()));
    }
  }
  /// Round floating lanes toward negative infinity.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> floor(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<F>(
                w, detail::constexpr_float::rounding::downward));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_floor(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_floor(v.to_native()));
    }
  }
  /// Round floating lanes toward positive infinity.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> ceil(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<F>(
                w, detail::constexpr_float::rounding::upward));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_ceil(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_ceil(v.to_native()));
    }
  }
  /// Round floating lanes toward zero.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> trunc(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<F>(
                w, detail::constexpr_float::rounding::toward_zero));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_trunc(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_trunc(v.to_native()));
    }
  }
  /// Round floating lanes to nearest integers, choosing even at ties.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline
      constexpr simd<T, N, A> round_even(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(detail::constexpr_float::round_integral_bits<F>(w));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_nearest(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_nearest(v.to_native()));
    }
  }
  /// Clear the floating sign bit without changing payload bits.
  template <std::floating_point T, std::size_t N, isa<> A>
    requires NATIVE_ARCH_REQUIRES
  (A) && (sizeof(T) * N == 16) native_inline constexpr simd<T, N, A> abs(simd<T, N, A> v) noexcept {
    if consteval {
      return detail::wasm_map(
          [](T x) {
            using F = detail::wasm_format<T>;
            using bits_type = typename F::bits_type;
            auto w = std::bit_cast<bits_type>(x);
            return std::bit_cast<T>(bits_type(w & ~F::sign_mask));
          },
          v);
    } else {
      if constexpr (sizeof(T) == 4)
        return simd<T, N, A>::from_native(wasm_f32x4_abs(v.to_native()));
      else
        return simd<T, N, A>::from_native(wasm_f64x2_abs(v.to_native()));
    }
  }
} // namespace native
