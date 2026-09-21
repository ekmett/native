
namespace native::detail::NATIVE_BACKEND {
  template<class T> using swizzle_word = std::conditional_t<sizeof(T)==1,std::uint8_t,
    std::conditional_t<sizeof(T)==2,std::uint16_t,std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>>>;
  template<class T> using swizzle_native = swizzle_word<T> __attribute__((ext_vector_type(4)));
  template<std::size_t... I> struct swizzle {
    static constexpr std::size_t size=sizeof...(I);
    template<class Self> using result = std::conditional_t<size==1,typename Self::value_type,
      vec<typename Self::value_type,size,Self::architecture>>;
    static constexpr bool unique=[] {
      constexpr std::size_t indices[]{I...};
      for(std::size_t i=0;i<size;++i)
        for(std::size_t j=0;j<i;++j) if(indices[i]==indices[j]) return false;
      return true;
    }();
    template<std::size_t D> static consteval int slot() {
      constexpr std::size_t indices[]{I...};
      for(std::size_t i=0;i<size;++i) if(indices[i]==D) return int(4+i);
      return int(D);
    }
    template<class Self> static consteval bool readable() {
      using T=typename Self::value_type;
      if constexpr(!((I<Self::lanes)&&...) || !std::is_trivially_copyable_v<T> ||
          !std::default_initializable<T> || !(sizeof(T)==1 || sizeof(T)==2 || sizeof(T)==4 || sizeof(T)==8)) return false;
      else if constexpr(!requires(Self const & self,T * p) { self.template store_memory<1>(p); }) return false;
      else if constexpr(size==1) return std::copy_constructible<T>;
      else return requires(T const * p) {
        sizeof(result<Self>);
        { result<Self>::template load_memory<1>(p) } -> std::same_as<result<Self>>;
      };
    }
    template<class Self> static consteval bool writable() {
      if constexpr(std::is_const_v<Self> || !unique || !readable<Self>()) return false;
      else if constexpr(size>1 && !requires(result<Self> const & rhs,typename Self::value_type * p) { rhs.template store_memory<1>(p); }) return false;
      else return std::is_copy_assignable_v<typename Self::value_type> &&
        std::is_copy_constructible_v<result<Self>> && requires(Self & self,typename Self::value_type const * p) {
          { Self::template load_memory<1>(p) } -> std::same_as<Self>;
          self=Self::template load_memory<1>(p);
        };
    }
    template<class V> static constexpr bool native_words=[] {
      using T=typename V::value_type;
      if constexpr(simd_custom_element<T>) return false;
      else if constexpr(requires { typename V::native_type; }) return sizeof(typename V::native_type)==sizeof(swizzle_native<T>);
      else return false;
    }();
    template<class Self> static native_inline auto words(Self const & self) {
      using T=typename Self::value_type;
      if constexpr(native_words<Self>) {
        return std::bit_cast<swizzle_native<T>>(self.to_native());
      } else {
        std::array<T,4> values{};
        self.template store_memory<1>(values.data());
        return std::bit_cast<swizzle_native<T>>(values);
      }
    }
    template<class V> static native_inline V from_words(swizzle_native<typename V::value_type> words) {
      using T=typename V::value_type;
      if constexpr(native_words<V>) {
        return V::from_native(std::bit_cast<typename V::native_type>(words));
      } else {
        auto values=std::bit_cast<std::array<T,4>>(words);
        return V::template load_memory<1>(values.data());
      }
    }
    template<class Self>
    native_nodiscard static native_inline result<Self> read(Self const & self) {
      using T=typename Self::value_type;
      auto value=words(self);
      if constexpr(size==1) {
        constexpr std::size_t indices[]{I...};
        return std::bit_cast<T>(swizzle_word<T>(value[indices[0]]));
      } else {
        constexpr int indices[]{int(I)...};
        auto shuffled=__builtin_shufflevector(value,value,
          indices[0],indices[1],indices[2%size],indices[3%size]);
        return from_words<result<Self>>(shuffled);
      }
    }
    template<class Self>
    native_inline static result<Self> write(Self & self,result<Self> rhs) {
      using T=typename Self::value_type;
      auto before=words(self);
      auto replacement=[&] {
        if constexpr(size==1) return swizzle_native<T>{std::bit_cast<swizzle_word<T>>(rhs),0,0,0};
        else return words(rhs);
      }();
      auto shuffled=__builtin_shufflevector(before,replacement,slot<0>(),slot<1>(),slot<2>(),slot<3>());
      self=from_words<Self>(shuffled);
      return rhs;
    }
  };
}
namespace native::detail {
  // Properties are compiler accessors, not proxy objects: a read owns its lanes,
  // and assignment materializes the complete right side before any scatter.
  template<class T,std::size_t N,::native::isa Arch> requires NATIVE_ARCH_REQUIRES(Arch) &&(N<=4)
  struct swizzle_access<T,N,Arch> {
    template<std::size_t K> using result = std::conditional_t<K==1,T,vec<T,K,Arch>>;
#define NATIVE_SWIZZLE_FIELD(NAME,K,...) \
    template<class Self> requires(::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::template readable<Self>()) \
    native_nodiscard native_inline result<K> get_##NAME(this Self const & self) { return ::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::read(self); } \
    template<class Self> requires(::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::template writable<Self>()) \
    native_inline result<K> set_##NAME(this Self & self,result<K> rhs) { return ::NATIVE_BACKEND_NAMESPACE::swizzle<__VA_ARGS__>::write(self,rhs); } \
    __declspec(property(get=get_##NAME,put=set_##NAME)) result<K> NAME;
#define NATIVE_SWIZZLE_ROW2(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##X,2,I,0) NATIVE_SWIZZLE_FIELD(A##Y,2,I,1) \
    NATIVE_SWIZZLE_FIELD(A##Z,2,I,2) NATIVE_SWIZZLE_FIELD(A##W,2,I,3)
#define NATIVE_SWIZZLE_ROW3(A,I,B,J,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##B##X,3,I,J,0) NATIVE_SWIZZLE_FIELD(A##B##Y,3,I,J,1) \
    NATIVE_SWIZZLE_FIELD(A##B##Z,3,I,J,2) NATIVE_SWIZZLE_FIELD(A##B##W,3,I,J,3)
#define NATIVE_SWIZZLE_PLANE3(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW3(A,I,X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW3(A,I,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW3(A,I,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW3(A,I,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE_ROW4(A,I,B,J,C,K,X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(A##B##C##X,4,I,J,K,0) NATIVE_SWIZZLE_FIELD(A##B##C##Y,4,I,J,K,1) \
    NATIVE_SWIZZLE_FIELD(A##B##C##Z,4,I,J,K,2) NATIVE_SWIZZLE_FIELD(A##B##C##W,4,I,J,K,3)
#define NATIVE_SWIZZLE_PLANE4(A,I,B,J,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW4(A,I,B,J,X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW4(A,I,B,J,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_ROW4(A,I,B,J,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW4(A,I,B,J,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE_CUBE4(A,I,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE4(A,I,X,0,X,Y,Z,W) NATIVE_SWIZZLE_PLANE4(A,I,Y,1,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE4(A,I,Z,2,X,Y,Z,W) NATIVE_SWIZZLE_PLANE4(A,I,W,3,X,Y,Z,W)
#define NATIVE_SWIZZLE4(X,Y,Z,W) \
    NATIVE_SWIZZLE_FIELD(X,1,0) NATIVE_SWIZZLE_FIELD(Y,1,1) NATIVE_SWIZZLE_FIELD(Z,1,2) NATIVE_SWIZZLE_FIELD(W,1,3) \
    NATIVE_SWIZZLE_ROW2(X,0,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_ROW2(W,3,X,Y,Z,W) \
    NATIVE_SWIZZLE_PLANE3(X,0,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_PLANE3(W,3,X,Y,Z,W) \
    NATIVE_SWIZZLE_CUBE4(X,0,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(Y,1,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(Z,2,X,Y,Z,W) NATIVE_SWIZZLE_CUBE4(W,3,X,Y,Z,W)
    NATIVE_SWIZZLE4(x,y,z,w)
#undef NATIVE_SWIZZLE4
#undef NATIVE_SWIZZLE_CUBE4
#undef NATIVE_SWIZZLE_PLANE4
#undef NATIVE_SWIZZLE_ROW4
#undef NATIVE_SWIZZLE_PLANE3
#undef NATIVE_SWIZZLE_ROW3
#undef NATIVE_SWIZZLE_ROW2
#undef NATIVE_SWIZZLE_FIELD
  };
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
