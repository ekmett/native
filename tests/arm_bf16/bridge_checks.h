// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
namespace bf16_fixture {
  native_target("bf16") bool bridges() {
    environment saved; fpcr(0);
    using B=native::simd<native::bf16,8,native::neon_bf16>;
    using F=native::simd<float,4,native::neon_bf16>;
    halves bits; bits.fill(0x3f80);
    auto a=B::load_bits(bits.data());
    auto c=F(0.f);
    std::array<float,4> output;
    F::from_native(native::bfdot<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=2.f) return false;
    F::from_native(native::bfmmla<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=4.f) return false;
    F::from_native(native::bfmlalt<arch>(c.to_native(),a.to_native(),a.to_native())).store(output.data());
    for(auto value:output) if(value!=1.f) return false;
    return true;
  }
}
