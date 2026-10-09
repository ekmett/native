# syntax=docker/dockerfile:1
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
FROM ubuntu:24.04 AS toolchain
SHELL ["/bin/bash", "-euo", "pipefail", "-c"]
ARG LLVM_VERSION=23
ARG CMAKE_SPEC="cmake>=4.4,<4.5"
RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates curl wget gnupg lsb-release software-properties-common \
      python3-venv git make sudo \
 && curl --fail --location https://apt.llvm.org/llvm.sh --output /tmp/llvm.sh \
 && bash /tmp/llvm.sh "${LLVM_VERSION}" \
 && apt-get install -y --no-install-recommends \
      "clang-tools-${LLVM_VERSION}" "clang-tidy-${LLVM_VERSION}" \
      "clang-format-${LLVM_VERSION}" "clangd-${LLVM_VERSION}" "lld-${LLVM_VERSION}" \
 && python3 -m venv /opt/build-tools \
 && /opt/build-tools/bin/python -m pip install --no-cache-dir "${CMAKE_SPEC}" 'ninja>=1.12' \
 && rm /tmp/llvm.sh \
 && apt-get clean && rm -rf /var/lib/apt/lists/*
ENV PATH="/opt/build-tools/bin:/usr/lib/llvm-${LLVM_VERSION}/bin:${PATH}"
ENV CC=clang CXX=clang++
RUN clang++ --version && clang-scan-deps --version && clang-tidy --version \
 && clang-format --version && clangd --version && cmake --version && ninja --version

FROM toolchain AS build
WORKDIR /src/native
COPY . .
RUN cmake -S . -B /tmp/native-build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/native \
      -DNATIVE_BUILD_TESTS=OFF \
 && cmake --build /tmp/native-build --parallel \
 && cmake --install /tmp/native-build

FROM toolchain
COPY --from=build /opt/native /opt/native
ENV CMAKE_PREFIX_PATH=/opt/native
# Exercise the installed package, including Hint and regenerated module BMIs.
RUN --mount=type=bind,source=tests/api,target=/tmp/native-api \
    cmake -S /tmp/native-api -B /tmp/native-check -G Ninja -DCMAKE_BUILD_TYPE=Release \
 && cmake --build /tmp/native-check --parallel \
 && ctest --test-dir /tmp/native-check --output-on-failure \
 && rm -rf /tmp/native-check
WORKDIR /workspace
LABEL org.opencontainers.image.source="https://github.com/ekmett/native" \
      org.opencontainers.image.title="native" \
      org.opencontainers.image.description="Native and Hint with LLVM 23, CMake 4.4 and Ninja for downstream C++26 builds" \
      org.opencontainers.image.licenses="BSD-2-Clause OR Apache-2.0"
