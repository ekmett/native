// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
import native.polyfill_relay;
int main(int argc,char **) {
  std::array<float,16> a{},b{},output{};
  for(unsigned i=0;i<16;++i) { a[i]=float(i+unsigned(argc)); b[i]=float(31-i); }
  polyfill_relay::scalar_add(a.data(),b.data(),output.data());
  for(auto value:output) if(value!=float(31+argc)) return 1;
  if(native::classify_isa(native::observe_cpu(),polyfill_relay::hardware).admitted()) {
    polyfill_relay::native_add(a.data(),b.data(),output.data());
    for(auto value:output) if(value!=float(31+argc)) return 2;
  }
}
