// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
#include <initializer_list>
import native.arm;
using namespace native;
constexpr auto both = arm_feature::neon & arm_feature::sm3 & arm_feature::sm4;
static_assert(std::uint64_t(arm_feature::sm3) == 20);
static_assert(std::uint64_t(arm_feature::sm4) == 21);
static_assert(feature_closure(arm_feature::sm3) == (neon & arm_feature::sm3));
static_assert(feature_closure(arm_feature::sm4) == (neon & arm_feature::sm4));
static_assert(target_features<arm>("sm4") == both);
static_assert(!target_features<arm>("sm3").valid());
static_assert([] {
  arm_capabilities cpu;
  cpu.present = cpu.observed = both;
  if (!classify_isa(cpu, both).admitted())
    return false;
  for (auto f : {arm_feature::neon, arm_feature::sm3, arm_feature::sm4}) {
    auto missing = cpu;
    missing.observed.set(f, false);
    if (classify_isa(missing, both).admitted())
      return false;
    missing = cpu;
    missing.present.set(f, false);
    if (classify_isa(missing, both).admitted())
      return false;
  }
  auto independent = cpu;
  independent.present.set(arm_feature::sm3, false);
  if (!classify_isa(independent, arm_feature::sm4).admitted())
    return false;
  if (classify_isa(independent, both).admitted())
    return false;
  independent = cpu;
  independent.present.set(arm_feature::sm4, false);
  if (!classify_isa(independent, arm_feature::sm3).admitted())
    return false;
  if (classify_isa(independent, both).admitted())
    return false;
  return true;
}());

static_assert([] {
  arm_capabilities::raw_observations raw;
  raw.baseline_observed = raw.fp = raw.asimd = true;
  for (auto feature : {arm_feature::sm3, arm_feature::sm4}) {
    raw.extra_features = isa(feature);
    raw.extra_observed = {};
    if (classify_isa(raw, feature).admitted())
      return false;
    raw.extra_observed = isa(feature);
    if (!classify_isa(raw, feature).admitted())
      return false;
    if (classify_isa(raw, both).admitted())
      return false;
    raw.extra_features = {};
    if (classify_isa(raw, feature).admitted())
      return false;
  }
  isa<arm> properties;
  properties.sm3 = true;
  properties.sm4 = true;
  if (properties != (arm_feature::sm3 & arm_feature::sm4))
    return false;
  properties.sm3 = false;
  return !properties.sm3 && properties.sm4;
}());

int main() {}
