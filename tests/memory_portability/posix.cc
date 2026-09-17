#include <cstdio>
#include <cstdlib>
#include <utility>
#include <unistd.h>
#include <sys/mman.h>
import simd.memory;

int main() {
  auto page = ::sysconf(_SC_PAGESIZE);
  if (page <= 0) return 1;
  auto size = static_cast<std::size_t>(page);
  void * raw = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (raw == MAP_FAILED) return 2;
  auto owned = simd::make_mmap_ptr(raw, size);
  static_cast<unsigned char *>(owned.get())[size - 1] = 42;
  auto moved = std::move(owned);
  if (owned || moved.get() != raw || moved.get_deleter().size != size) return 3;
  if (static_cast<unsigned char *>(moved.get())[size - 1] != 42) return 4;
  moved.reset();
  if (moved) return 5;
  std::puts("POSIX mapping move/reset passed");
}

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
