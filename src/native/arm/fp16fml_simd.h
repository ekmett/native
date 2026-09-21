// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included by native.arm.fp16fml after native.simd.
#if NATIVE_HOST_NEON || defined(NATIVE_DOXYGEN)
namespace native::detail {
  template<isa<arm> Arch,bool Subtract,bool High,int Lane,std::size_t N,std::size_t M>
  consteval simd<float,N,Arch> fp16fml_value(simd<float,N,Arch> acc,
      simd<fp16,2*N,Arch> a,simd<fp16,M,Arch> b) noexcept {
    namespace cf=constexpr_float;
    std::array<std::uint32_t,N> c{};
    std::array<std::uint16_t,2*N> av{};
    std::array<std::uint16_t,M> bv{};
    acc.store_bits(c.data());a.store_bits(av.data());b.store_bits(bv.data());
    auto widen=[](std::uint16_t x) {
      return cf::is_nan<cf::binary16>(x)?cf::resize_nan<cf::binary32,cf::binary16>(x,false)
        :cf::convert_bits<cf::binary32,cf::binary16>(x);
    };
    for(std::size_t i=0;i<N;++i) {
      auto index=i+(High?N:0);
      auto x=av[index];
      if constexpr(Subtract) x^=0x8000;
      c[i]=cf::fma_bits<cf::binary32>(widen(x),widen(bv[Lane<0?index:Lane]),c[i]);
    }
    return simd<float,N,Arch>::load_bits(c.data());
  }
}
export namespace native {
  /// \defgroup arm_fp16fml FP16 widening fused multiply-add
  /// Runtime calls require arm_feature::fp16fml (FEAT_FHM), independently of FCMA/BF16.
  /// Binary16 inputs accumulate directly into binary32, with one rounding.
  /// FPCR controls and FPSR effects are those of the instruction; neither is reset.
  /// Volatile assembly retains status effects even when the result is unused.
  /// Constant evaluation uses RNE, gradual inputs/results, DN=AH=AHP=FZ=FZ16=FIZ=EBF=0,
  /// with masked exceptions and no status effects; fixed instruction rules still apply.
  /// Missing-feature overloads are consteval-only and require complete storage types.
  /// \{

  // All vector operands share Arch; native registers remain implementation details.
  /// Add products from the low 2 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,-1>(acc,a,b); } else {
      auto result = detail::fmlal<Arch>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,-1>(acc,a,b);
  }

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlal_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b);
  }

  /// FMLAL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlal_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b);
  }

  /// Add products from the low 4 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,-1>(acc,a,b); } else {
      auto result = detail::fmlal<Arch>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,-1>(acc,a,b);
  }

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlal_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b);
  }

  /// FMLAL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlal_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,false,Lane>(acc,a,b);
  }

  /// Add products from the high 2 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,-1>(acc,a,b); } else {
      auto result = detail::fmlal2<Arch>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,-1>(acc,a,b);
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlal2_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b);
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlal2_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlal2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b);
  }

  /// Add products from the high 4 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,-1>(acc,a,b); } else {
      auto result = detail::fmlal2<Arch>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,-1>(acc,a,b);
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlal2_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b);
  }

  /// FMLAL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlal2_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlal2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,false,true,Lane>(acc,a,b);
  }

  /// Subtract products from the low 2 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,-1>(acc,a,b); } else {
      auto result = detail::fmlsl<Arch>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,-1>(acc,a,b);
  }

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b);
  }

  /// FMLSL with b[Lane] broadcast; selects the low 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b);
  }

  /// Subtract products from the low 4 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,-1>(acc,a,b); } else {
      auto result = detail::fmlsl<Arch>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,-1>(acc,a,b);
  }

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b);
  }

  /// FMLSL with b[Lane] broadcast; selects the low 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,false,Lane>(acc,a,b);
  }

  /// Subtract products from the high 2 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,-1>(acc,a,b); } else {
      auto result = detail::fmlsl2<Arch>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl2(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,-1>(acc,a,b);
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl2_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b);
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 2 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl2_lane<Arch, Lane>(
        vget_low_f32(acc.to_native()),
        __builtin_bit_cast(float16x4_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 2, Arch>::from_native(vcombine_f32(result, vdup_n_f32(0.f)));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 2, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 2, Arch> fmlsl2_lane(
      simd<float, 2, Arch> acc,
      simd<fp16, 4, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b);
  }

  /// Subtract products from the high 4 half lanes of a and b.
  template<isa<arm> Arch> requires(Arch.has(arm_feature::fp16fml))
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,-1>(acc,a,b); } else {
      auto result = detail::fmlsl2<Arch>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)))
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl2(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,-1>(acc,a,b);
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 4)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl2_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x4_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && requires { sizeof(simd<fp16, 4, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 4)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 4, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b);
  }

  /// FMLSL2 with b[Lane] broadcast; selects the high 4 lanes of a.
  template<isa<arm> Arch, unsigned Lane> requires(Arch.has(arm_feature::fp16fml) && Lane < 8)
  native_nodiscard native_inline __attribute__((target("fp16fml")))
  constexpr simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    if consteval { return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b); } else {
      auto result = detail::fmlsl2_lane<Arch, Lane>(
        __builtin_bit_cast(float32x4_t, acc.to_native()),
        __builtin_bit_cast(float16x8_t, a.to_native()),
        __builtin_bit_cast(float16x8_t, b.to_native()));
      return simd<float, 4, Arch>::from_native(__builtin_bit_cast(typename simd<float, 4, Arch>::native_type, result));
    }
  }

  /// Constant-evaluation-only form when the instruction feature is absent.
  template<isa<arm> Arch, unsigned Lane> requires(requires { sizeof(simd<float, 4, Arch>); } && requires { sizeof(simd<fp16, 8, Arch>); } && !(Arch.has(arm_feature::fp16fml)) && Lane < 8)
  native_nodiscard consteval
  simd<float, 4, Arch> fmlsl2_lane(
      simd<float, 4, Arch> acc,
      simd<fp16, 8, Arch> a,
      simd<fp16, 8, Arch> b) noexcept {
    return detail::fp16fml_value<Arch,true,true,Lane>(acc,a,b);
  }

  /// \cond
  template<isa<arm> Arch, class A, class B, class C>
  void fmlal(A, B, C) = delete;
  template<isa<arm> Arch, unsigned Lane, class A, class B, class C>
  void fmlal_lane(A, B, C) = delete;
  template<isa<arm> Arch, class A, class B, class C>
  void fmlal2(A, B, C) = delete;
  template<isa<arm> Arch, unsigned Lane, class A, class B, class C>
  void fmlal2_lane(A, B, C) = delete;
  template<isa<arm> Arch, class A, class B, class C>
  void fmlsl(A, B, C) = delete;
  template<isa<arm> Arch, unsigned Lane, class A, class B, class C>
  void fmlsl_lane(A, B, C) = delete;
  template<isa<arm> Arch, class A, class B, class C>
  void fmlsl2(A, B, C) = delete;
  template<isa<arm> Arch, unsigned Lane, class A, class B, class C>
  void fmlsl2_lane(A, B, C) = delete;
  /// \endcond
  /// \}
}
#endif
