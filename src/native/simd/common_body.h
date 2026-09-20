namespace native {
namespace detail {
#ifdef NATIVE_ARCH_REQUIRES
  template<class T, std::size_t N, ::native::isa Arch, std::size_t L> requires NATIVE_ARCH_REQUIRES(Arch)
  struct register_memory<vec<T,N,Arch>,L> {
    using V=vec<T,N,Arch>;
    static constexpr std::size_t lanes = L;
    native_nodiscard static native_inline native_pure V load_partial(native_noescape float const * p, std::size_t n, float fill = 0) noexcept {
      alignas(64) std::array<float, L> a; a.fill(fill);
      for (std::size_t i = 0; i < n; ++i) a[i] = p[i];
      return V::load(a.data());
    }
    native_inline void store_partial(native_noescape float * p, std::size_t n) const noexcept {
      alignas(64) std::array<float, L> a; static_cast<V const &>(*this).store(a.data());
      for (std::size_t i = 0; i < n; ++i) p[i] = a[i];
    }
  };
#endif

}
  template<simd_custom_element T, std::size_t N, isa Arch> requires NATIVE_COMMON_ARCH(Arch)
  struct native_empty_bases vec<T,N,Arch> : simd_customization<T,
      vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>, detail::swizzle_access<T,N,Arch> {
    using base = simd_customization<T,vec<typename simd_traits<T>::storage_type,N,Arch>,vec<T,N,Arch>>;
    using base::base;
    /// Default-construct the element customization; its initialization contract is retained.
    native_inline constexpr vec() = default;
    static constexpr isa architecture=Arch;
    using mask = typename vec<typename simd_traits<T>::storage_type,N,Arch>::mask;
    using predicate_type = predicate<N,Arch>;
    template<class U> using rebind = vec<U,N,Arch>;
  };
  /// \ingroup vector_memory
  /// Load all `V::lanes` elements using the element's memory customization.
  /// \pre `p` addresses that many readable elements and meets alignment `A`.
  /// Exceptions propagate from the selected `V::load_memory<A>` operation.
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && requires(U const * p) { V::template load_memory<A>(p); }
  native_nodiscard native_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template load_memory<A>(p))) {
    return V::template load_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Load custom elements through their typed memory operation, then project raw storage.
  /// The customization owns any normalization; this is not an arbitrary pointer cast.
  template <class V, simd_custom_element U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  native_nodiscard native_inline V load_simd(U const * p, simd_memory<A,Access> = {})
      noexcept(noexcept(V::template rebind<U>::template load_memory<A>(p).to_native())) {
    return V::template rebind<U>::template load_memory<A>(p).to_native();
  }
  /// \ingroup vector_memory
  /// Store all logical lanes using the element's memory customization.
  /// \pre `p` addresses that many writable elements and meets alignment `A`.
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && requires(V value,U * p) { value.template store_memory<A>(p); }
  native_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(value.template store_memory<A>(p))) {
    value.template store_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Convert raw storage to the custom element vector before its typed store.
  /// Construction and storage both contribute to the exception specification.
  template <simd_custom_element U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && std::same_as<typename V::value_type,typename simd_traits<U>::storage_type>
  native_inline void store_simd(U * p,V value,simd_memory<A,Access> = {})
      noexcept(noexcept(typename V::template rebind<U>(value).template store_memory<A>(p))) {
    typename V::template rebind<U>(value).template store_memory<A>(p);
  }
  /// \ingroup vector_memory
  /// Load an array or fixed-extent span whose extent equals the lane count.
  template<class V,class U,std::size_t N> requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) &&(N==V::lanes) &&
    requires(U const * p) { ::native::load_simd<V>(p); }
  native_nodiscard native_inline V load_simd(std::array<U,N> const & values)
      noexcept(noexcept(::native::load_simd<V>(values.data()))) {
    return ::native::load_simd<V>(values.data());
  }
  /// \ingroup vector_memory
  /// Load an array or fixed-extent span whose extent equals the lane count.
  template<class V,class U,std::size_t N> requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) &&(N==V::lanes) &&
    requires(U * p) { ::native::load_simd<V>(p); }
  native_nodiscard native_inline V load_simd(std::span<U,N> values)
      noexcept(noexcept(::native::load_simd<V>(values.data()))) {
    return ::native::load_simd<V>(values.data());
  }
  /// \ingroup vector_memory
  /// Load `count` elements and fill the remaining lanes with `fill`.
  /// \pre `count <= V::lanes`; `p` may be null only when `count == 0`.
  /// Uses an element temporary; construction, assignment and loading may throw.
  /// The alignment hint does not extend the readable prefix.
  template <class V, class U, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && std::default_initializable<U> &&
      std::constructible_from<U,typename V::value_type &> && std::is_copy_assignable_v<U> &&
      requires(U const * p) { ::native::load_simd<V>(p); }
  native_nodiscard native_inline V load_simd_partial(U const * p,std::size_t count,
      typename V::value_type fill={},simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_constructible_v<U,typename V::value_type &> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::native::load_simd<V>(p))) {
    std::array<U,V::lanes> temporary; temporary.fill(U(fill));
    for(std::size_t i=0;i<count;++i) temporary[i]=p[i];
    return ::native::load_simd<V>(temporary.data());
  }
  /// \ingroup vector_memory
  /// Store the first `count` logical lanes, leaving the following memory alone.
  /// \pre `count <= V::lanes`; `p` may be null only when `count == 0`.
  /// Element construction, assignment and storage determine the exception guarantee.
  template <class U, class V, std::size_t A=1, simd_access Access=simd_access::ordinary>
    requires detail::memory_architecture<V,U>::known && NATIVE_COMMON_ARCH(detail::memory_architecture_v<V,U>) && std::default_initializable<U> && std::is_copy_assignable_v<U> &&
      requires(U * p,V value) { ::native::store_simd(p,value); }
  native_inline void store_simd_partial(U * p,V value,std::size_t count,simd_memory<A,Access> = {})
      noexcept(std::is_nothrow_default_constructible_v<U> &&
        std::is_nothrow_copy_assignable_v<U> && noexcept(::native::store_simd(p,value))) {
    std::array<U,V::lanes> temporary; ::native::store_simd(temporary.data(),value);
    for(std::size_t i=0;i<count;++i) p[i]=temporary[i];
  }

}
