# syntax=docker/dockerfile:1
# Optional local Linux toolchain. CI uses the same tools directly on its runner.
FROM ubuntu:24.04
SHELL ["/bin/bash", "-euo", "pipefail", "-c"]
ARG LLVM_VERSION=23
ARG CMAKE_SPEC="cmake>=4.4,<4.5"
RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates curl wget gnupg lsb-release software-properties-common \
      python3-venv git make sudo \
 && curl --fail --location https://apt.llvm.org/llvm.sh --output /tmp/llvm.sh \
 && bash /tmp/llvm.sh "${LLVM_VERSION}" \
 && apt-get install -y --no-install-recommends "clang-tools-${LLVM_VERSION}" "lld-${LLVM_VERSION}" \
 && python3 -m venv /opt/build-tools \
 && /opt/build-tools/bin/python -m pip install --no-cache-dir "${CMAKE_SPEC}" 'ninja>=1.12' \
 && rm /tmp/llvm.sh \
 && apt-get clean
ENV PATH="/opt/build-tools/bin:/usr/lib/llvm-${LLVM_VERSION}/bin:${PATH}"
RUN clang++ --version && clang-scan-deps --version && cmake --version && ninja --version
WORKDIR /workspace
LABEL org.opencontainers.image.description="LLVM 23 / CMake 4.4 toolchain for simd"
