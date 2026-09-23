// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <type_traits>
#include <vector>
import native;
import native.math;

constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
constexpr auto relaxed_architecture = native::feature_closure(native::wasm_feature::relaxed_simd);
using vector = native::simd<float, 4, architecture>;
using batch = std::array<vector, 3>;
using registers = native::wide<vector, 3>;
template<class V> concept has_fma = requires(V v) { native::fma(v,v,v); };
static_assert(!has_fma<vector>);
static_assert(!has_fma<native::simd<float,4,relaxed_architecture>>);
template<class V> concept has_scaleb = requires(V v) { native::scaleb(v,v); };
template<class V,class M> concept has_masked_scaleb = requires(V v,M m) {
  native::masked_scaleb(m,v,v,v); native::masked_scaleb_zero(m,v,v);
};
static_assert(!has_scaleb<vector> && !has_masked_scaleb<vector,vector::mask>);
static_assert(!has_scaleb<native::simd<float,4,relaxed_architecture>>);
static_assert(!has_scaleb<batch> && !has_masked_scaleb<batch,std::array<vector::mask,3>>);
static_assert(!has_scaleb<registers> && !has_masked_scaleb<registers,native::wide<vector::mask,3>>);
template<class V> concept has_wide_scaleb = requires(V v) { wide::scaleb(v,v); };
template<class V,class M> concept has_wide_masked_scaleb = requires(V v,M m) {
  wide::masked_scaleb(m,v,v,v); wide::masked_scaleb_zero(m,v,v);
};
static_assert(!has_wide_scaleb<batch> && !has_wide_masked_scaleb<batch,std::array<vector::mask,3>>);


static void require(bool good, char const * message) {
  if (!good) { std::fprintf(stderr, "%s\n", message); std::abort(); }
}
static bool equivalent(float a, float b) {
  return std::bit_cast<std::uint32_t>(a) == std::bit_cast<std::uint32_t>(b) ||
    (std::isnan(a) && std::isnan(b));
}
static void exact(float a, float b) {
  if (!equivalent(a,b)) {
    std::fprintf(stderr, "graph mismatch: %a (%08x) != %a (%08x)\n", a,
      std::bit_cast<std::uint32_t>(a), b, std::bit_cast<std::uint32_t>(b));
    std::abort();
  }
}

#pragma clang attribute push(__attribute__((target("simd128"))), apply_to=function)
constexpr std::array<float, 12> constant_input{
  -0.f, 0.f, -1.f, 1.f, -87.f, 88.f, 0.7853981852531433f, -1.5707963705062866f,
  8191.99951171875f, -8191.99951171875f, 0x1p-149f, -0x1p-149f};
struct result {
  std::array<float,12> exponential{}, flushed{}, sine{}, cosine{};
};
constexpr result evaluate(std::array<float,12> const & input) {
  batch x{vector::load(input.data()),vector::load(input.data()+4),vector::load(input.data()+8)};
  auto e = math::exp(x), f = math::exp<true>(x);
  auto [s,c] = math::sincos(x);
  result r;
  for(std::size_t j=0;j<3;++j) {
    e[j].store(r.exponential.data()+4*j); f[j].store(r.flushed.data()+4*j);
    s[j].store(r.sine.data()+4*j); c[j].store(r.cosine.data()+4*j);
  }
  return r;
}
constexpr auto constant_result = evaluate(constant_input);
static_assert(constant_result.exponential[0] == 1.f && constant_result.exponential[1] == 1.f);
static_assert(std::bit_cast<std::uint32_t>(constant_result.sine[0]) == 0x80000000u);
static_assert(constant_result.cosine[0] == 1.f);
static_assert(constant_result.sine[10] == 0x1p-149f && constant_result.sine[11] == -0x1p-149f);
static_assert(std::same_as<decltype(math::exp(vector(1.f))),vector>);
static_assert(std::same_as<decltype(math::exp(batch{})),batch>);
static_assert(std::same_as<decltype(math::exp(registers{})),registers>);
static_assert(std::same_as<decltype(native::math::exp(registers{})),registers>);
static_assert(std::same_as<decltype(native::math::sincos(registers{})),std::pair<registers,registers>>);
constexpr bool empty_shapes() {
  std::array<vector,0> a{};
  native::wide<vector,0> w{};
  auto e=math::exp(a), f=native::exp<true>(a);
  auto [s,c]=math::sincos(a);
  auto ew=native::math::exp(w);
  auto [sw,cw]=native::math::sincos(w);
  return e.empty() && f.empty() && s.empty() && c.empty() && ew.registers.empty() &&
    sw.registers.empty() && cw.registers.empty();
}
static_assert(empty_shapes());
constexpr bool exp_corners() {
  constexpr std::array<std::uint32_t,4> words{0xff800000u,0x7f800000u,0x7f800001u,0xffc12345u};
  std::array<float,4> output{}, small{}, flush{};
  math::exp(vector::load_bits(words.data())).store(output.data());
  math::exp(vector(-90.f,-104.f,-103.9f,-87.33654022216796875f)).store(small.data());
  math::exp<true>(vector(-90.f,-104.f,-103.9f,-87.33654022216796875f)).store(flush.data());
  return output[0]==0.f && std::bit_cast<std::uint32_t>(output[1])==0x7f800000u &&
    (std::bit_cast<std::uint32_t>(output[2])&0x7fffffffu)>0x7f800000u &&
    (std::bit_cast<std::uint32_t>(output[3])&0x7fffffffu)>0x7f800000u &&
    small[0]>0.f && small[0]<std::numeric_limits<float>::min() && small[1]==0.f && small[2]>0.f &&
    flush[0]==0.f && flush[1]==0.f && flush[2]==0.f && flush[3]==small[3];
}
static_assert(exp_corners());

static void exact_vector(vector a, vector b) {
  std::array<float,4> x{}, y{}; a.store(x.data()); b.store(y.data());
  for(unsigned i=0;i<4;++i) exact(x[i],y[i]);
}
static void shapes(batch const & x) {
  registers w{x};
  auto e=math::exp(x), f=math::exp<true>(x), ae=native::exp(x), af=native::exp<true>(x);
  auto we=native::math::exp(w), wf=native::math::exp<true>(w);
  auto pe=math::exp(w), pf=math::exp<true>(w);
  auto [s,c]=math::sincos(x);
  auto [as,ac]=native::sincos(x);
  auto [ws,wc]=native::math::sincos(w);
  auto [ps,pc]=math::sincos(w);
  auto standalone_s=math::sin(x), standalone_c=math::cos(x);
  auto ns=native::sin(w), nc=native::cos(w);
  for(unsigned j=0;j<3;++j) {
    exact_vector(e[j],math::exp(x[j])); exact_vector(e[j],native::math::exp(x[j]));
    exact_vector(f[j],native::math::exp<true>(x[j]));
    exact_vector(e[j],ae[j]); exact_vector(f[j],af[j]);
    exact_vector(e[j],we.registers[j]); exact_vector(f[j],wf.registers[j]);
    exact_vector(e[j],pe.registers[j]); exact_vector(f[j],pf.registers[j]);
    auto [vs,vc]=native::math::sincos(x[j]);
    exact_vector(s[j],vs); exact_vector(c[j],vc);
    exact_vector(s[j],as[j]); exact_vector(c[j],ac[j]);
    exact_vector(s[j],ws.registers[j]); exact_vector(c[j],wc.registers[j]);
    exact_vector(s[j],ps.registers[j]); exact_vector(c[j],pc.registers[j]);
    exact_vector(standalone_s[j],ns.registers[j]); exact_vector(standalone_c[j],nc.registers[j]);
  }
}
#pragma clang attribute pop

// Independent scalar spelling with a volatile product enforces two roundings
// even if the compiler or target supports contraction.
static float madd(float a,float b,float c) {
  volatile float product=a*b;
  return product+c;
}
static float exp_reference(float x,bool flush) {
  if (x < (flush ? -87.33654022216796875f : -104.f)) return 0.f;
  // Keep the independent nonfused graph below the accepted early-overflow edge.
  if (x >= 88.3762664794921875f) return std::numeric_limits<float>::infinity();
  float r=88.72283935546875f < x ? 88.72283935546875f : x;
  float n=std::nearbyint(r*1.4426950408889634f);
  r=madd(n,-0x1.62e400p-1f,r); r=madd(n,-0x1.7f7d1cp-20f,r);
  float y=madd(r,0x1.6d55f4p-10f,0x1.123e2cp-7f);
  y=madd(r,y,0x1.5554ep-5f); y=madd(r,y,0x1.55548ap-3f);
  y=madd(r,y,0.5f); y=madd(r,y,1.f); y=madd(r,y,1.f);
  return std::isnan(n) ? n : std::ldexp(y,int(n));
}
static std::uint32_t distance(float a,float b) {
  auto x=std::bit_cast<std::uint32_t>(a), y=std::bit_cast<std::uint32_t>(b);
  return x>y ? x-y : y-x;
}
static void neighbors(std::vector<float> & bank,float x) {
  auto word=std::bit_cast<std::uint32_t>(x);
  for(int i=-4;i<=4;++i) bank.push_back(std::bit_cast<float>(word+std::uint32_t(i)));
}
volatile std::uint32_t runtime_seed=0x47ca198bu;

// New public cancellation-safe/logarithmic kernels, including compatibility
// wrappers, follow the SIMD128 separate-rounding graph at compile and run time.
#pragma clang attribute push(__attribute__((target("simd128"))), apply_to=function)
struct new_math_result {
  std::array<float, 12> expm1{}, gain{}, log{}, log1p{};
};
constexpr new_math_result evaluate_new_math(std::array<float, 12> const & input) {
  batch x{vector::load(input.data()), vector::load(input.data()+4), vector::load(input.data()+8)};
  auto e=math::expm1(x), g=math::damping_gain(x), l=math::log(x), p=math::log1p(x);
  new_math_result result;
  for (unsigned j=0; j<3; ++j) {
    e[j].store(result.expm1.data()+4*j); g[j].store(result.gain.data()+4*j);
    l[j].store(result.log.data()+4*j); p[j].store(result.log1p.data()+4*j);
  }
  return result;
}
constexpr std::array<float, 12> new_math_input{-0.f, 0.f, 0x1p-149f, -0x1p-149f,
  INFINITY, -INFINITY, std::bit_cast<float>(0x7fc12345u), std::bit_cast<float>(0xff800001u),
  1.f, -1.f, 0x1p-25f, -0x1p-25f};
constexpr auto new_math_constant=evaluate_new_math(new_math_input);
static_assert(std::bit_cast<std::uint32_t>(new_math_constant.expm1[0])==0x80000000u);
static_assert(std::bit_cast<std::uint32_t>(new_math_constant.gain[0])==0x80000000u);
static_assert(std::bit_cast<std::uint32_t>(new_math_constant.log1p[0])==0x80000000u);
static_assert(new_math_constant.expm1[2]==0x1p-149f && new_math_constant.expm1[3]==-0x1p-149f);
static_assert(new_math_constant.gain[2]==0x1p-149f && new_math_constant.gain[3]==-0x1p-149f);
static_assert(new_math_constant.log1p[2]==0x1p-149f && new_math_constant.log1p[3]==-0x1p-149f);
static_assert(new_math_constant.expm1[4]==INFINITY && new_math_constant.expm1[5]==-1.f);
static_assert(new_math_constant.gain[4]==1.f && new_math_constant.gain[5]==-INFINITY);
static_assert(new_math_constant.log[0]==-INFINITY && new_math_constant.log[2]==-INFINITY);
static_assert(new_math_constant.log[3]==-INFINITY && new_math_constant.log[4]==INFINITY);
static_assert(new_math_constant.log[8]==0.f && new_math_constant.log1p[9]==-INFINITY);
static_assert((std::bit_cast<std::uint32_t>(new_math_constant.log[6])&0x7fffffffu)>0x7f800000u);
static_assert((std::bit_cast<std::uint32_t>(new_math_constant.log1p[7])&0x7fffffffu)>0x7f800000u);
static_assert(math::expm1(std::array<vector,0>{}).empty());
static_assert(math::damping_gain(std::array<vector,0>{}).empty());
static_assert(math::log(std::array<vector,0>{}).empty());
static_assert(math::log1p(std::array<vector,0>{}).empty());

static void new_math_shapes(batch const & x) {
  registers w{x};
#define CHECK_NEW_MATH(name) \
  { auto a=math::name(x), b=native::name(x); \
    auto c=math::name(w), d=native::math::name(w); \
    for (unsigned j=0; j<3; ++j) { \
      exact_vector(a[j], math::name(x[j])); \
      exact_vector(a[j], native::name(x[j])); \
      exact_vector(a[j], b[j]); exact_vector(a[j], c.registers[j]); \
      exact_vector(a[j], d.registers[j]); \
    } }
  CHECK_NEW_MATH(expm1)
  CHECK_NEW_MATH(damping_gain)
  CHECK_NEW_MATH(log)
  CHECK_NEW_MATH(log1p)
#undef CHECK_NEW_MATH
}

static void check_new_math(std::vector<float> bank) {
  std::array<float,12> input=new_math_input;
  volatile std::uint32_t zero=0;
  for (auto & x:input) x=std::bit_cast<float>(std::bit_cast<std::uint32_t>(x)^zero);
  auto r=evaluate_new_math(input);
  for (unsigned i=0; i<12; ++i) {
    exact(r.expm1[i],new_math_constant.expm1[i]); exact(r.gain[i],new_math_constant.gain[i]);
    exact(r.log[i],new_math_constant.log[i]); exact(r.log1p[i],new_math_constant.log1p[i]);
  }
  new_math_shapes(batch{vector::load(input.data()),vector::load(input.data()+4),vector::load(input.data()+8)});
  for (int n=-26; n<=127; ++n)
    for (double residual:{-0.5,0.,0.5}) neighbors(bank,float((double(n)+residual)*std::log(2.)));
  for (float x:{0x1p-25f,-0x1p-25f,0x1p-126f,-0x1p-126f}) neighbors(bank,x);
  std::uint32_t seed=runtime_seed;
  for (unsigned i=0; i<65536; ++i) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5;
    bank.push_back(std::bit_cast<float>(seed));
  }
  unsigned max_expm1_ulp=0,max_gain_ulp=0;
  for (std::size_t base=0; base<bank.size(); base+=4) {
    std::array<float,4> x{},e{},g{};
    for (unsigned i=0; i<4; ++i) x[i]=bank[(base+i)%bank.size()];
    auto v=vector::load(x.data());
    math::expm1(v).store(e.data()); math::damping_gain(v).store(g.data());
    if (base%1024==0) new_math_shapes(batch{v,-v,vector(0.25f)});
    for (unsigned i=0; i<4; ++i) {
      for (unsigned gain=0; gain<2; ++gain) {
        double argument=gain ? -double(x[i]) : double(x[i]);
        float expected=argument>=88.3762664794921875 ? INFINITY : float(std::expm1(argument));
        if (gain) expected=-expected;
        auto actual=gain ? g[i] : e[i];
        if (!std::isfinite(expected) || expected==0.f) exact(actual,expected);
        else {
          auto ulp=distance(actual,expected);
          auto & maximum=gain ? max_gain_ulp : max_expm1_ulp;
          maximum=std::max(maximum,ulp);
          if (ulp>2) {
            std::fprintf(stderr,"SIMD128 %s input=%a actual=%a expected=%a ULP=%u\n",
              gain?"damping_gain":"expm1",x[i],actual,expected,ulp);
            std::abort();
          }
        }
      }
    }
  }
  std::printf("SIMD128 expm1/damping_gain %zu inputs: max %u/%u ULP; four-operation shapes/constexpr/corners passed\n",
    bank.size(),max_expm1_ulp,max_gain_ulp);
}
#pragma clang attribute pop

__attribute__((target("simd128"),noinline)) static int run() {
  std::array<float,12> runtime_input=constant_input;
  // Force inputs through runtime storage, excluding a constant-folded execution.
  volatile float runtime_zero=0.f;
  for(float & x:runtime_input) if(x!=0.f) x+=runtime_zero;
  auto r=evaluate(runtime_input);
  for(unsigned i=0;i<12;++i) {
    exact(r.exponential[i],constant_result.exponential[i]); exact(r.flushed[i],constant_result.flushed[i]);
    exact(r.sine[i],constant_result.sine[i]); exact(r.cosine[i],constant_result.cosine[i]);
  }
  batch shape_input{vector::load(runtime_input.data()),vector::load(runtime_input.data()+4),
    vector::load(runtime_input.data()+8)};
  shapes(shape_input); require(empty_shapes(),"empty shapes"); require(exp_corners(),"exp corners");

  std::vector<float> exp_bank{0.f,-0.f,INFINITY,-INFINITY,NAN,178.f,
    std::bit_cast<float>(0x7f800001u),std::bit_cast<float>(0xffc12345u),
    std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),0x1p-149f,-0x1p-149f};
  for(float x:{-104.f,-103.97208404541015625f,-87.33654022216796875f,88.3762664794921875f,88.72283935546875f}) neighbors(exp_bank,x);
  std::vector<float> trig_bank{0.f,-0.f,0x1p-149f,-0x1p-149f,0x1.fffffcp-127f,-0x1.fffffcp-127f,
    8191.99951171875f,-8191.99951171875f};
  for(unsigned k=1;k<10430;++k) {
    float x=float(double(k)*0.78539816339744830962);
    if(x<8191.99f) { neighbors(trig_bank,x); neighbors(trig_bank,-x); }
  }
  std::uint32_t seed=runtime_seed;
  for(unsigned i=0;i<65536;++i) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5;
    exp_bank.push_back(-104.f+float(seed>>8)*(193.f/16777216.f));
    trig_bank.push_back(-8192.f+float(seed>>8)*(16384.f/16777216.f));
    float bits=std::bit_cast<float>(seed);
    if(std::isfinite(bits) && std::abs(bits)<8192.f) trig_bank.push_back(bits);
  }
  for(float & x:trig_bank) if(x==-8192.f) x=-8191.99951171875f;
  check_new_math(exp_bank);
  unsigned max_exp_ulp=0; double max_trig_error=0;
  for(std::size_t base=0;base<exp_bank.size();base+=4) {
    std::array<float,4> x{},e{},f{};
    for(unsigned i=0;i<4;++i)x[i]=exp_bank[(base+i)%exp_bank.size()];
    math::exp(vector::load(x.data())).store(e.data());
    math::exp<true>(vector::load(x.data())).store(f.data());
    for(unsigned i=0;i<4;++i) {
      exact(e[i],exp_reference(x[i],false)); exact(f[i],exp_reference(x[i],true));
      float expected=float(std::exp(double(x[i])));
      if(std::isnan(expected)) require(std::isnan(e[i]),"exp NaN");
      else if(x[i]>=88.3762664794921875f || std::isinf(expected))
        require(std::isinf(e[i]) && e[i]>0,"exp overflow/infinity");
      else {
        max_exp_ulp=std::max(max_exp_ulp,distance(e[i],expected));
        require(distance(e[i],expected)<=2,"exp exceeds 2 ULP");
      }
    }
  }
  for(std::size_t base=0;base<trig_bank.size();base+=4) {
    std::array<float,4> x{},s{},c{};
    for(unsigned i=0;i<4;++i)x[i]=trig_bank[(base+i)%trig_bank.size()];
    auto v=vector::load(x.data());
    auto pair=math::sincos(v); pair.first.store(s.data());pair.second.store(c.data());
    std::array<float,4> separate_s{},separate_c{};
    math::sin(v).store(separate_s.data());math::cos(v).store(separate_c.data());
    if(base%1024==0) shapes(batch{v,-v,vector(0.25f)});
    for(unsigned i=0;i<4;++i) {
      double error=std::max(std::abs(double(s[i])-std::sin(double(x[i]))),
        std::abs(double(c[i])-std::cos(double(x[i]))));
      error=std::max({error,std::abs(double(separate_s[i])-std::sin(double(x[i]))),
        std::abs(double(separate_c[i])-std::cos(double(x[i])))});
      max_trig_error=std::max(max_trig_error,error);
      require(error<=0x1p-22,"sincos exceeds 2^-22 absolute error");
    }
  }
  // A stronger caller must retain the exact same separate-rounding graph.
  using relaxed_vector=native::simd<float,4,relaxed_architecture>;
  auto relaxed=math::exp(relaxed_vector::load(runtime_input.data()));
  std::array<float,4> output{}; relaxed.store(output.data());
  for(unsigned i=0;i<4;++i) exact(output[i],r.exponential[i]);
  std::printf("SIMD128 exp %zu inputs: max %u ULP; sincos %zu inputs: max absolute %.9g\n",
    exp_bank.size(),max_exp_ulp,trig_bank.size(),max_trig_error);
  return 0;
}
int main() { return run(); }
