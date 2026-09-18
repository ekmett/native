# Historical per-ISA compiler fixture

This standalone compiler probe retains the layout used before the single-hub
migration. Its local `simd.avx2` and `simd.avx512` modules are synthetic test
definitions, not modules provided by the installed package. It is outside the
root build and native package CI. For the current package contract, use the
[installed hub consumer](../omnibus/README.md) and the
[source target-list guide](../../docs/omnibus.md).

This standalone fixture uses small SIMD and wide implementations to exercise
native operations, CTAD, ADL, module imports and archive linkage. It does not
replace or claim complete coverage of the production API.

The `simd::vec<T,N,Arch>` fixture specializations have
private ISA flags.
One generic `fixture::engine<Arch,Lanes>` supplies the architecture type
and finds arithmetic through ADL. The dispatcher has no ISA imports or IPO;
its guard permits the configured minimum and rejects AVX-512 flags unless
minimal feature metadata explicitly enables them. It checks
CPUID, OSXSAVE and XCR0 before entering a bridge.
ThinLTO can be enabled for the profile objects while preserving that boundary.
ISA-dependent definitions must retain their declared profile identity when
imported into a consumer compiled with broader target features.

Use Windows Clang, CMake 4.4 and Ninja in an initialized compiler environment.
Run through the host's CPU resource gate:

```powershell
python tests/isa_modules/run.py --build-dir build/isa-no-lto --compiler C:/path/to/clang-cl.exe --check-incompatible-imports
python tests/isa_modules/run.py --build-dir build/isa-thin --compiler C:/path/to/clang-cl.exe --thin-lto --check-incompatible-imports
```

Output directories must be new or empty. The runner records source hashes,
commands, output and exits. Direct consumers run only after the dispatcher
admits their CPU/OS profile; expected incompatible-BMI failures are separate
compiler controls. The dispatcher may run before stronger-profile admission on a host that
already meets its compilation baseline.
This fixture does not establish production-library accuracy, ARM support or
performance. Optional wait instructions are not exercised.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
