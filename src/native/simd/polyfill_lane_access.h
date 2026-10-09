// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
// Included in native half/BF16 classes under their actual feature target.
    /// Read one exact half representation when software permission is present.
    template<std::size_t I> requires(architecture.has(polyfill) && I<lanes)
    [[nodiscard]] hint_inline constexpr value_type get() const noexcept {
      std::array<std::uint16_t,lanes> words{}; store_bits(words.data()); return value_type::from_bits(words[I]);
    }
    /// Replace one half representation, preserving all other lanes.
    template<std::size_t I> requires(architecture.has(polyfill) && I<lanes)
    [[nodiscard]] hint_inline constexpr simd set(value_type value) const noexcept {
      std::array<std::uint16_t,lanes> words{}; store_bits(words.data()); words[I]=value.to_bits(); return load_bits(words.data());
    }
    /// Synonym for replacing one exact representation.
    template<std::size_t I> requires(architecture.has(polyfill) && I<lanes)
    [[nodiscard]] hint_inline constexpr simd replace(value_type value) const noexcept { return this->template set<I>(value); }
    /// Broadcast a selected native half lane with explicit permission.
    template<std::size_t I> requires(architecture.has(polyfill) && I<lanes)
    [[nodiscard]] friend hint_inline constexpr simd broadcast(simd value,imm_t<I>) noexcept {
      return simd(value.template get<I>());
    }
