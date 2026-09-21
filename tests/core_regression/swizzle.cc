// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "support/profile.h"
#include "support/guarded_pages.h"
#include "support/fp_environment.h"
#include "math_contract.h"
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <span>
#include <type_traits>
#include <utility>

namespace {
  constexpr bool compact_test_mask=[]<native::isa A>() {
    if constexpr(decltype(A)::family==native::x86) return A==native::avx512;
    else return false;
  }.template operator()<test_arch>();
  template<class T,std::size_t N> constexpr bool original_layout =
    sizeof(test_vec<T,N>)==sizeof(T)*N && alignof(test_vec<T,N>)==sizeof(T)*N;
  static_assert(original_layout<float,1> && original_layout<std::int32_t,1> &&
    original_layout<std::uint32_t,1> && original_layout<native::mask32,1>);
  static_assert(sizeof(test_vec<bool,1>)==1 && alignof(test_vec<bool,1>)==1);
  static_assert(original_layout<float,4> && original_layout<std::int32_t,4> &&
    original_layout<std::uint32_t,4> && original_layout<native::mask32,4>);
#if defined(__AVX2__)
  static_assert(original_layout<float,8> && original_layout<std::int32_t,8> &&
    original_layout<std::uint32_t,8> && original_layout<native::mask32,8>);
#endif
#if defined(__AVX512F__)
  static_assert(original_layout<float,16> && original_layout<std::int32_t,16> &&
    original_layout<std::uint32_t,16> && original_layout<native::mask32,16>);
#endif
  template<class A,class B> constexpr bool unequal_shapes_rejected =
    !requires(A a,B b){a+b;} && !requires(A a,B b){a-b;} &&
    !requires(A a,B b){a*b;} && !requires(A a,B b){a/b;} &&
    !requires(A a,B b){a==b;} && !requires(A a,B b){a!=b;} &&
    !requires(A a,B b){a<b;} && !requires(A a,B b){a<=b;} &&
    !requires(A a,B b){a>b;} && !requires(A a,B b){a>=b;};
  template<class T> constexpr bool logical_shapes_rejected =
    unequal_shapes_rejected<test_vec<T,2>,test_vec<T,3>> &&
    unequal_shapes_rejected<test_vec<T,3>,test_vec<T,2>> &&
    unequal_shapes_rejected<test_vec<T,3>,test_vec<T,4>> &&
    unequal_shapes_rejected<test_vec<T,4>,test_vec<T,3>> &&
    unequal_shapes_rejected<test_vec<T,2>,test_vec<T,4>> &&
    unequal_shapes_rejected<test_vec<T,4>,test_vec<T,2>>;
  static_assert(logical_shapes_rejected<float> && logical_shapes_rejected<std::int32_t> &&
    logical_shapes_rejected<std::uint32_t>);

  void require(bool condition, char const * message) {
    if (!condition) { std::fprintf(stderr, "%s\n", message); std::abort(); }
  }
  template<class V> concept writable_xy = requires(V & a, V const & b) { a.xy = b.xy; };
  template<class V> concept const_writable_xy = requires(V const & a, V const & b) { a.xy = b.xy; };
  template<class V> concept rvalue_writable_xy = requires(V a, V const & b) { std::move(a).xy = b.xy; };
  template<class V> concept repeated_writable = requires(V & a) { a.xxxx = a.xxxx; };
  template<class V> concept nested_component_writable = requires(V & a) { a.xyz.x = typename V::value_type{}; };
  template<class V> concept nested_property_writable = requires(V & a) { a.xyz.xy = a.xy; };
  template<class V> concept addressable_property = requires(V & a) { &a.xy; };
  template<class T> void lvalue_only(T &);
  template<class V> concept binds_mutable_reference = requires(V & a) { lvalue_only(a.xy); };
  template<class V> concept readable_z = requires(V const & a) { a.z; };
  template<class V> concept readable_w = requires(V const & a) { a.w; };
  template<class V> concept readable_xyz = requires(V const & a) { a.xyz; };

  template<class T> using word = std::conditional_t<sizeof(T)==4,std::uint32_t,std::uint64_t>;
  template<class T> word<T> bits(T value) { return std::bit_cast<word<T>>(value); }
  template<class V> auto lanes(V const & value) {
    std::array<typename V::value_type,V::lanes> result{};
    native::store_simd(result.data(),value);
    return result;
  }
  template<class V,std::size_t N> void expect(V const & actual,std::array<typename V::value_type,N> const & expected,char const * message) {
    static_assert(V::lanes==N);
    auto got=lanes(actual);
    for(std::size_t i=0;i<N;++i) require(bits(got[i])==bits(expected[i]),message);
  }

  template<class T,std::size_t N> void shape() {
    using V=test_vec<T,N>;
    static_assert(std::is_trivially_copyable_v<V>);
    static_assert(sizeof(V)==16 && alignof(V)==16);
    static_assert(sizeof(V)==sizeof(typename V::native_type));
    static_assert(alignof(V)==alignof(typename V::native_type));
    static_assert(std::same_as<typename V::template rebind<std::int32_t>,test_vec<std::int32_t,N>>);
    static_assert(std::same_as<decltype(std::declval<V>()+T(2)),V>);
    static_assert(std::same_as<decltype(T(2)+std::declval<V>()),V>);
    static_assert(std::same_as<decltype(std::declval<V>()-T(2)),V>);
    static_assert(std::same_as<decltype(T(2)*std::declval<V>()),V>);
    static_assert(std::same_as<decltype(std::declval<V>()<std::declval<V>()),typename V::mask>);
    static_assert(std::same_as<decltype(std::declval<V &>().xy),test_vec<T,2>>);
    static_assert(std::same_as<decltype((std::declval<V &>().xy)),test_vec<T,2>>);
    static_assert(std::same_as<decltype(std::declval<V &>().xy=std::declval<V const &>().xy),test_vec<T,2>>);
    static_assert(!nested_component_writable<V> && !nested_property_writable<V>);
    static_assert(!addressable_property<V> && !binds_mutable_reference<V>);
    static_assert(std::same_as<decltype(std::declval<V &>().xxxx),test_vec<T,4>>);
    static_assert(writable_xy<V> && !const_writable_xy<V> && !rvalue_writable_xy<V> && !repeated_writable<V>);
    if constexpr(N==2) {
      constexpr V constructed{T(1),T(2)};
      static_assert(std::same_as<decltype(constructed),V const>);
      static_assert(!readable_z<V> && !readable_xyz<V>);
      expect(constructed,std::array{T(1),T(2)},"constexpr two-lane constructor values");
      expect(test_vec<T,2>(T(1),T(2)),std::array{T(1),T(2)},"explicit two-lane values");
    } else {
      constexpr V constructed{T(1),T(2),T(3)};
      static_assert(std::same_as<decltype(constructed),V const>);
      static_assert(readable_z<V> && readable_xyz<V>);
      expect(constructed,std::array{T(1),T(2),T(3)},"constexpr three-lane constructor values");
      expect(test_vec<T,3>(T(1),T(2),T(3)),std::array{T(1),T(2),T(3)},"explicit three-lane values");
    }
    static_assert(!readable_w<V>);
    static_assert(std::same_as<decltype(V(std::array<T,N>{})),V>);
    expect(V{},std::array<T,N>{},"short value initialization zeros logical lanes");
    auto value=V(std::array<T,N>{});
    require((value==V(T(0))).to_bitset()==((std::uint64_t(1)<<N)-1),"short comparison mask shape");
  }

  template<std::size_t N> void masks() {
    using V=test_vec<float,N>;using M=typename V::mask;
    using Full=test_vec<native::mask32,N>;
    static_assert(std::is_trivially_copyable_v<M>);
    static_assert(M::compact==compact_test_mask);
    static_assert(sizeof(Full)==16 && std::is_trivially_copyable_v<Full>);
    constexpr auto low=(std::uint64_t(1)<<N)-1;
    auto full=Full::from_bitset(~std::uint64_t(0));
    require(full.to_bitset()==low && all(full),"short vector mask clips physical padding");
    auto selected=M::from_bitset(~std::uint64_t(0));
    require(selected.to_bitset()==low && all(selected),"short full mask clips physical padding");
    auto alternating=M::from_bitset(0x55555555u);
    require((~alternating).to_bitset()==(0xaaaaaaaau&low),"short mask complement clips padding");
    auto values=lanes(select(alternating,V(1.f),V(2.f)));
    for(std::size_t i=0;i<N;++i)require(values[i]==((i&1)?2.f:1.f),"short mask selection");
  }

  void properties() {
    using V2=test_vec<float,2>;using V3=test_vec<float,3>;
    auto negzero=std::bit_cast<float>(0x80000000u);
    auto signaling=std::bit_cast<float>(0x7fa12345u);
    auto negative_nan=std::bit_cast<float>(0xffc45678u);
    auto tiny=std::bit_cast<float>(1u);
    V2 a(std::array{negzero,signaling}),b(std::array{tiny,negative_nan});
    auto saved=a.xy;
    auto && detached=a.xy;
    a.xy=b.xy;
    expect(a,std::array{tiny,negative_nan},"same-name property assignment scatters");
    expect(saved,std::array{negzero,signaling},"copied swizzle owns its original words");
    expect(detached,std::array{negzero,signaling},"auto-rvalue-reference extends detached snapshot lifetime");
    V2 chain_a{1.f,2.f},chain_b{3.f,4.f};
    chain_a.xy=chain_b.xy=a.xy;
    expect(chain_a,std::array{tiny,negative_nan},"chained property assignment first receiver");
    expect(chain_b,std::array{tiny,negative_nan},"chained property assignment second receiver");
    auto temporary=V2(std::array{negzero,signaling}).xy;
    expect(temporary,std::array{negzero,signaling},"rvalue property read owns its words");
    auto repeated=a.xxxx;
    expect(repeated,std::array{tiny,tiny,tiny,tiny},"repeated property reads preserve words");
    V3 triple(std::array{negzero,signaling,negative_nan});
    auto before=triple.xyz;
    triple.xyz=triple.zyx;
    expect(triple,std::array{negative_nan,signaling,negzero},"overlapping scatter snapshots all source words");
    expect(before,std::array{negzero,signaling,negative_nan},"three-lane snapshot is independent");
    V3 const immutable(std::array{negzero,signaling,negative_nan});
    auto const_read=immutable.zyx;
    expect(const_read,std::array{negative_nan,signaling,negzero},"const swizzle read owns words");
    triple.xy=b.xy;
    expect(triple,std::array{tiny,negative_nan,negzero},"two-lane scatter retains untouched third lane");
    triple.x=signaling;
    expect(triple,std::array{signaling,negative_nan,negzero},"component scatter preserves exact words");
    V3 compound{1.f,2.f,3.f};
    compound.xy+=V2{4.f,5.f};
    expect(compound,std::array{5.f,7.f,3.f},"compound property addition scatters to parent");
    compound.xy*=V2{2.f,3.f};
    expect(compound,std::array{10.f,21.f,3.f},"compound property multiplication scatters to parent");
    int receivers=0,operands=0;
    auto receiver=[&]() -> V3 & {++receivers;return compound;};
    auto operand=[&]() {++operands;return V2{1.f,2.f};};
    receiver().xy+=operand();
    require(receivers==1 && operands==1,"compound receiver and RHS evaluated once");
    expect(compound,std::array{11.f,23.f,3.f},"compound evaluated-once numerical result");
  }

  template<std::size_t N> void floating_arithmetic() {
    using V=test_vec<float,N>;using Physical=test_vec<float,4>;
    constexpr std::array<std::uint32_t,14> bank{0u,0x80000000u,1u,0x007fffffu,
      0x00800000u,0x3f000001u,0x3f800000u,0xbf800000u,0x40000000u,
      0x7f7fffffu,0x7f800000u,0xff800000u,0x7fc12345u,0xffa12345u};
    for(auto mode:{native::test::fp_mode::gradual,native::test::fp_mode::flush}) {
      native::test::fp_scope controls(mode);
      for(std::size_t row=0;row<bank.size();++row) {
        std::array<float,N> a{},b{},c{};
        std::array<float,4> pa{1.f,1.f,1.f,1.f},pb=pa,pc=pa;
        for(std::size_t i=0;i<N;++i) {
          a[i]=pa[i]=std::bit_cast<float>(bank[(row+i)%bank.size()]);
          b[i]=pb[i]=std::bit_cast<float>(bank[(row+i+3)%bank.size()]);
          c[i]=pc[i]=std::bit_cast<float>(bank[(row+i+7)%bank.size()]);
        }
        V x(a),y(b),z(c);Physical px(pa),py(pb),pz(pc);
        auto compare=[&](auto operation,char const * message) {
          auto actual=lanes(operation(x,y,z));auto expected=lanes(operation(px,py,pz));
          for(std::size_t i=0;i<N;++i)
            require(native::math_test::equivalent_fp32(bits(actual[i]),bits(expected[i])),message);
        };
        compare([](auto x,auto y,auto){return x+y;},"short add matches active physical lanes");
        compare([](auto x,auto y,auto){return x-y;},"short subtract matches active physical lanes");
        compare([](auto x,auto y,auto){return x*y;},"short multiply matches active physical lanes");
        compare([](auto x,auto y,auto){return x/y;},"short divide matches active physical lanes");
        compare([](auto x,auto y,auto z){return fma(x,y,z);},"short FMA matches active physical lanes");
        compare([](auto x,auto,auto){return sqrt(x);},"short sqrt matches active physical lanes");
      }
      // Volatile values prevent a constant-folded quotient from replacing the
      // padded hardware operation whose exception behavior is under test.
      static volatile float numerators[]{2.f,4.f,8.f},denominators[]{1.f,2.f,4.f};
      std::array<float,N> a{},b{};
      for(std::size_t i=0;i<N;++i){a[i]=numerators[i];b[i]=denominators[i];}
      V x(a),y(b);
      auto state=native::test::read_fp_state();state.status=0;native::test::write_fp_state(state);
      auto quotient=lanes(x/y);
      auto status=native::test::read_fp_state().status;
#if defined(__x86_64__) || defined(_M_X64)
      constexpr std::uint64_t invalid_or_divzero=5;
#else
      constexpr std::uint64_t invalid_or_divzero=3;
#endif
      require((status&invalid_or_divzero)==0,"inactive division padding raises no invalid or divide-by-zero");
      for(float q:quotient) require(q==2.f,"benign short quotient");
      require(controls.controls_match(),"short arithmetic preserves FP controls");
    }
  }

  template<class T,std::size_t N> void integer_arithmetic() {
    using V=test_vec<T,N>;using Physical=test_vec<T,4>;
    constexpr std::array<std::uint32_t,7> bank{0u,1u,2u,0x7fffffffu,0x80000000u,0xfffffffeu,0xffffffffu};
    for(std::size_t row=0;row<bank.size();++row) {
      std::array<T,N> a{},b{};std::array<T,4> pa{},pb{};
      for(std::size_t i=0;i<N;++i) {
        a[i]=pa[i]=std::bit_cast<T>(bank[(row+i)%bank.size()]);
        b[i]=pb[i]=std::bit_cast<T>(bank[(row+i+2)%bank.size()]);
      }
      V x(a),y(b);Physical px(pa),py(pb);
      auto compare=[&](auto operation,auto reference,char const * message) {
        auto actual=lanes(operation(x,y));auto expected=lanes(operation(px,py));
        for(std::size_t i=0;i<N;++i) {
          require(bits(actual[i])==bits(expected[i]),message);
          require(bits(actual[i])==reference(bits(a[i]),bits(b[i])),"short integer modulo oracle");
        }
      };
      compare([](auto x,auto y){return x+y;},[](auto x,auto y){return x+y;},"short integer add physical lanes");
      compare([](auto x,auto y){return x-y;},[](auto x,auto y){return x-y;},"short integer subtract physical lanes");
      compare([](auto x,auto y){return x*y;},[](auto x,auto y){return x*y;},"short integer multiply physical lanes");
    }
  }

  template<class T,std::size_t N> void memory() {
    using V=test_vec<T,N>;
    native::test::guarded_pages source,destination;
    std::array<T,N> bank{};
    for(std::size_t i=0;i<N;++i) bank[i]=T(i+1);
    auto * input=reinterpret_cast<T *>(source.end())-N;
    auto * output=reinterpret_cast<T *>(destination.end())-N;
    for(std::size_t i=0;i<N;++i) {std::construct_at(input+i,bank[i]);std::construct_at(output+i,T(19));}
    auto value=native::load_simd<V>(input);
    expect(value,bank,"full short load touches exactly its logical lanes");
    expect(V::loadu(input),bank,"native short loadu touches exactly its logical lanes");
    V::loadu(input).storeu(output);
    for(std::size_t i=0;i<N;++i) require(bits(output[i])==bits(bank[i]),"native short storeu touches exactly its logical lanes");
    native::store_simd(output,value);
    for(std::size_t i=0;i<N;++i) require(bits(output[i])==bits(bank[i]),"full short store touches exactly its logical lanes");
    expect(native::load_simd<V>(std::span<T const,N>(input,N)),bank,"short span load");
    expect(native::load_simd<V>(bank),bank,"short array load");
    for(std::size_t count=0;count<=N;++count) {
      auto * tail=reinterpret_cast<T *>(source.end())-count;
      auto * target=reinterpret_cast<T *>(destination.end())-count;
      for(std::size_t i=0;i<count;++i) {std::construct_at(tail+i,bank[i]);std::construct_at(target+i,T(19));}
      auto partial=native::load_simd_partial<V>(tail,count,T(7));
      auto expected=bank;for(std::size_t i=count;i<N;++i)expected[i]=T(7);
      expect(partial,expected,"short partial load and fill");
      native::store_simd_partial(target,partial,count);
      for(std::size_t i=0;i<count;++i)require(bits(target[i])==bits(bank[i]),"short partial store");
    }
    auto empty=native::load_simd_partial<V>(static_cast<T const *>(nullptr),0,T(7));
    native::store_simd_partial(static_cast<T *>(nullptr),empty,0);
    auto expected=bank;expected.fill(T(7));expect(empty,expected,"empty null short memory");
  }
}
int main() {
  shape<float,2>();shape<float,3>();
  shape<std::int32_t,2>();shape<std::int32_t,3>();
  shape<std::uint32_t,2>();shape<std::uint32_t,3>();
  masks<2>();masks<3>();
  properties();
  floating_arithmetic<2>();floating_arithmetic<3>();
  integer_arithmetic<std::int32_t,2>();integer_arithmetic<std::int32_t,3>();
  integer_arithmetic<std::uint32_t,2>();integer_arithmetic<std::uint32_t,3>();
  memory<float,2>();memory<float,3>();
  memory<std::int32_t,2>();memory<std::int32_t,3>();
  memory<std::uint32_t,2>();memory<std::uint32_t,3>();
  constexpr char const * arch=[]<native::isa A>() {
    if constexpr(decltype(A)::family==native::x86) return A==native::avx512?"avx512":"avx2";
    else return "neon";
  }.template operator()<test_arch>();
  using M2=typename test_vec<float,2>::mask;
  using M3=typename test_vec<float,3>::mask;
  std::printf("swizzle passed: arch=%s, float/int32/uint32 logical lanes=2,3, compact masks=%d,%d; owning snapshots, exact word scatter and guarded memory\n",
    arch,int(M2::compact),int(M3::compact));
}
