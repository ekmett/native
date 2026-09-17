# Convenience wrapper; CMake owns dependency scanning, building and testing.
PRESET ?= clang-release
BUILD_DIR ?= build/$(PRESET)
JOBS ?= 2
CMAKE_ARGS ?=
PREFIX ?= $(CURDIR)/build/install

.PHONY: all configure build test install docs lint format clean
all: build

configure:
	cmake --preset $(PRESET) -B "$(BUILD_DIR)" $(CMAKE_ARGS)

build: configure
	cmake --build "$(BUILD_DIR)" --parallel $(JOBS)

test: build
	ctest --test-dir "$(BUILD_DIR)" --output-on-failure

install: build
	cmake --install "$(BUILD_DIR)" --prefix "$(PREFIX)"

docs:
	cmake --preset $(PRESET) -B "$(BUILD_DIR)" $(CMAKE_ARGS) -DSIMD_BUILD_DOCS=ON
	cmake --build "$(BUILD_DIR)" --target simd_docs --parallel $(JOBS)

lint:
	git diff --check
	git diff --cached --check

format:
	bin/remove_whitespace.sh

clean:
	cmake --build "$(BUILD_DIR)" --target clean
