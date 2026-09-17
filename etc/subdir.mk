# Directory where the .git directory is expected to be located
EXPECTED_GIT_DIR := simd

# Shell command to find the nearest containing folder with a .git directory, using EXPECTED_GIT_DIR
# as a backstop

SUBDIR := $(shell \
  dir=$$(pwd); dir=$$(dirname "$$dir"); \
  while [ "$$dir" != "/" ]; do \
    if [ -d "$$dir/.git" ]; then \
      echo "$$dir"; exit 0; \
    else \
      if [ "$$(basename "$$dir")" != "$(EXPECTED_GIT_DIR)" ]; then \
        dir=$$(dirname "$$dir"); \
      else \
        exit 1; \
      fi; \
    fi; \
  done; \
  exit 1)

default: all

# Delegate all targets, arguments, and flags to the Makefile in SUBDIR
%:
	$(MAKE) -C $(SUBDIR) $@

.PHONY: default
