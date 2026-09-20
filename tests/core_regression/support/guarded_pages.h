#include "support/failure.h"
#pragma once
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <system_error>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace native::test {
  // Test-only storage: one read/write page between two inaccessible pages.
  // The caller establishes typed object lifetimes in [begin(), end()).
  struct guarded_pages {
    guarded_pages() {
#if defined(_WIN32)
      SYSTEM_INFO info{};
      GetSystemInfo(&info);
      page_size_ = info.dwPageSize;
#else
      long const size = sysconf(_SC_PAGESIZE);
      if (size <= 0) native::test::fail(std::runtime_error("Unable to query page size"));
      page_size_ = static_cast<std::size_t>(size);
#endif
      if (page_size_ == 0 || page_size_ > std::numeric_limits<std::size_t>::max() / 3)
        native::test::fail(std::runtime_error("Invalid guarded allocation page size"));
#if defined(_WIN32)
      base_ = static_cast<std::byte *>(VirtualAlloc(nullptr, page_size_ * 3,
        MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS));
      if (!base_) native::test::fail(std::system_error(static_cast<int>(GetLastError()),
        std::system_category(), "VirtualAlloc guarded pages"));
      DWORD old_protection{};
      if (!VirtualProtect(base_ + page_size_, page_size_, PAGE_READWRITE, &old_protection)) {
        auto const error = GetLastError();
        VirtualFree(base_, 0, MEM_RELEASE);
        native::test::fail(std::system_error(static_cast<int>(error), std::system_category(),
          "VirtualProtect middle page"));
      }
#else
#if defined(MAP_ANONYMOUS)
      constexpr int anonymous = MAP_ANONYMOUS;
#else
      constexpr int anonymous = MAP_ANON;
#endif
      void * mapping = mmap(nullptr, page_size_ * 3, PROT_NONE,
        MAP_PRIVATE | anonymous, -1, 0);
      if (mapping == MAP_FAILED) native::test::fail(std::system_error(errno, std::generic_category(),
        "mmap guarded pages"));
      base_ = static_cast<std::byte *>(mapping);
      if (mprotect(base_ + page_size_, page_size_, PROT_READ | PROT_WRITE) != 0) {
        int const error = errno;
        munmap(base_, page_size_ * 3);
        native::test::fail(std::system_error(error, std::generic_category(), "mprotect middle page"));
      }
#endif
    }
    guarded_pages(guarded_pages const &) = delete;
    guarded_pages & operator=(guarded_pages const &) = delete;
    guarded_pages(guarded_pages &&) = delete;
    guarded_pages & operator=(guarded_pages &&) = delete;
    ~guarded_pages() noexcept {
#if defined(_WIN32)
      VirtualFree(base_, 0, MEM_RELEASE);
#else
      munmap(base_, page_size_ * 3);
#endif
    }
    [[nodiscard]] std::size_t page_size() const noexcept { return page_size_; }
    [[nodiscard]] std::byte * begin() noexcept { return base_ + page_size_; }
    [[nodiscard]] std::byte const * begin() const noexcept { return base_ + page_size_; }
    [[nodiscard]] std::byte * end() noexcept { return begin() + page_size_; }
    [[nodiscard]] std::byte const * end() const noexcept { return begin() + page_size_; }
  private:
    std::byte * base_ = nullptr;
    std::size_t page_size_ = 0;
  };
}
/**
 * \file
 * \license
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
 * SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
 * \endlicense
 * \author Edward Kmett <ekmett@gmail.com>
 * \brief Provides protected boundary pages for host memory tests.
 */
