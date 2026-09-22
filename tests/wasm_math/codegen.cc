// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
import native;
import native.math;
constexpr auto architecture=native::feature_closure(native::wasm_feature::simd128);
using vector=native::simd<float,4,architecture>;
#define ENTRY __attribute__((target("simd128"),noinline))
extern "C" {
ENTRY void promoted_exp_single(float * out,float const * in) { math::exp(vector::load(in)).store(out); }
ENTRY void promoted_exp_batch(float * out,float const * in) {
  native::wide<vector,3> x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::exp(x);
  for(unsigned i=0;i<3;++i) r.registers[i].store(out+4*i);
}
ENTRY void array_exp_batch(float * out,float const * in) {
  std::array x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::exp(x);
  for(unsigned i=0;i<3;++i) r[i].store(out+4*i);
}
ENTRY void promoted_sincos_batch(float * s,float * c,float const * in) {
  native::wide<vector,3> x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::sincos(x);
  for(unsigned i=0;i<3;++i) {r.first.registers[i].store(s+4*i);r.second.registers[i].store(c+4*i);}
}
ENTRY void exp_single(float * out,float const * in) { native::math::exp(vector::load(in)).store(out); }
ENTRY void exp_batch(float * out,float const * in) {
  native::wide<vector,3> x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=native::math::exp(x);
  for(unsigned i=0;i<3;++i) r.registers[i].store(out+4*i);
}
ENTRY void exp_flush(float * out,float const * in) { math::exp<true>(vector::load(in)).store(out); }
ENTRY void sin_single(float * out,float const * in) { native::math::sin(vector::load(in)).store(out); }
ENTRY void cos_single(float * out,float const * in) { native::math::cos(vector::load(in)).store(out); }
ENTRY void sincos_single(float * s,float * c,float const * in) {
  auto r=native::math::sincos(vector::load(in)); r.first.store(s);r.second.store(c);
}
ENTRY void sincos_batch(float * s,float * c,float const * in) {
  native::wide<vector,3> x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=native::math::sincos(x);
  for(unsigned i=0;i<3;++i) {r.first.registers[i].store(s+4*i);r.second.registers[i].store(c+4*i);}
}
__attribute__((target("simd128,relaxed-simd"),noinline))
void exp_relaxed_caller(float * out,float const * in) {
  constexpr auto stronger=native::feature_closure(native::wasm_feature::relaxed_simd);
  math::exp(native::simd<float,4,stronger>::load(in)).store(out);
}
}

extern "C" {
ENTRY void promoted_expm1_single(float * out,float const * in) { math::expm1(vector::load(in)).store(out); }
ENTRY void native_expm1_single(float * out,float const * in) { native::expm1(vector::load(in)).store(out); }
ENTRY void promoted_expm1_batch(float * out,float const * in) {
  std::array x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::expm1(x);
  for(unsigned i=0;i<3;++i) r[i].store(out+4*i);
}
ENTRY void promoted_damping_gain_single(float * out,float const * in) { math::damping_gain(vector::load(in)).store(out); }
ENTRY void native_damping_gain_single(float * out,float const * in) { native::damping_gain(vector::load(in)).store(out); }
ENTRY void promoted_damping_gain_batch(float * out,float const * in) {
  std::array x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::damping_gain(x);
  for(unsigned i=0;i<3;++i) r[i].store(out+4*i);
}
ENTRY void promoted_log_single(float * out,float const * in) { math::log(vector::load(in)).store(out); }
ENTRY void native_log_single(float * out,float const * in) { native::log(vector::load(in)).store(out); }
ENTRY void promoted_log_batch(float * out,float const * in) {
  std::array x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::log(x);
  for(unsigned i=0;i<3;++i) r[i].store(out+4*i);
}
ENTRY void promoted_log1p_single(float * out,float const * in) { math::log1p(vector::load(in)).store(out); }
ENTRY void native_log1p_single(float * out,float const * in) { native::log1p(vector::load(in)).store(out); }
ENTRY void promoted_log1p_batch(float * out,float const * in) {
  std::array x{vector::load(in),vector::load(in+4),vector::load(in+8)};
  auto r=math::log1p(x);
  for(unsigned i=0;i<3;++i) r[i].store(out+4*i);
}
}
