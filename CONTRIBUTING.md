# Contribution Guidelines <!-- GITHUB {#contributing_page} GITHUB -->

<!--
SPDX-FileType: TEXT
SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
-->

<!-- GITHUB
[TOC]
GITHUB -->

All code committed to this repository should be compatible with the current `BSD-2-Clause OR Apache 2.0` license
seen in [LICENSE.md](LICENSE.md).

## Exceptions

Exceptions can be granted in exceptional circumstances, but explicit consent of the primary author, Edward Kmett,
is required for any pull-request or patch supplied to the primary repository at https://github.com/ekmett/simd.git
that restricts these licensing terms.

In particular limitations may make sense for concrete assets that form part of the distribution, but not the final
library form.

You are of course free to do whatever you like to your own copies of this code, subject to the limitations of the
license mentioned above, with whatever exceptions are tracked near the assets in question.

Note: `Doxygen` itself is GPL v2, but the assets it produces are unencumbered. Therefore this is currently not an exception.

For example, the current exceptions are:

- The `HaskLig` web font used in the documentation is licensed under SIL Open Font License 1.1.

Several library dependencies are not offered under these terms:

- `Catch2` is licensed under the Boost Software License 1.0
- `doxygen-awesome-css` is MIT licensed.
- `spdlog` is MIT licensed.
- `mathjax` is licensed via Apache 2.0

## SPDX Documentation

All files should contain [SPDX identifiers](https://spdx.dev/learn/handling-license-info/) specifying the license and
tracking authorship and provenance of assets where possible.

An example short-form identifier in local style:

```
# SPDX-FileType: Source
# SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
```

If you edit a source file, please feel free to add yourself in a line of

```
SPDX-FileCopyrightText: [year] [author] <contact email>
```

as this is the clearest attribution that we have that you consent to your code being used under the current licensing.

If you feel it is warranted, then please add yourself in the LICENSE.md file explicitly as well

SPDX license slugs should be placed either near the very top of the file or the very bottom of the file.

If it is not possible to place the license in the source of the asset then that should be tracked in the nearest
location that a `LICENSE.spdx` file can be placed.

## Software Bill of Materials

When adding assets, where possible the SHA256 of the asset or the git hash of the repository should be tracked in a nearby
LICENSE.spdx file, so we can find it later.

Eventually this will be used to feed some sort of automated Software Bill of Materials. Contributions to help enable that
are very welcome.

## Policy Questions

Please feel free to contact me if you have any questions or concerns about the current policies.

-Edward Kmett (Oct, 2024)


<!-- GITHUB
<div class="section_buttons">

| Previous                                     |                                              Next |
|:---------------------------------------------|--------------------------------------------------:|
| [Code of Conduct](@ref code_of_conduct_page) |      [Security Policy](@ref security_policy_page) |

</div>
GITHUB -->
