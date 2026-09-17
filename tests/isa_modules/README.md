# Per-ISA module fixture

This standalone fixture uses small SIMD and wide implementations to exercise
native operations, CTAD, ADL, module imports and archive linkage. It does not
replace or claim complete coverage of the production API.

The `simd::vec<T,N,Arch>` fixture specializations have
private ISA flags.
One generic `fixture::engine<Arch,Lanes>` supplies the architecture type
and finds arithmetic through ADL. The baseline dispatcher has no ISA imports,
ISA flags or IPO; it checks CPUID, OSXSAVE and XCR0 before entering a bridge.
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
compiler controls. Only the dispatcher is safe to invoke before admission.
This fixture does not establish production-library accuracy, ARM support or
performance. Optional wait instructions are not exercised.

<!-- SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com> -->
<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
