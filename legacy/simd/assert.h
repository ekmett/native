#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief assert even during release

    \def simd_assert

      \brief An `assert` with \c spdlog message formatting and logging.

      \details

        Arguments other than the condition are only evaluated if the condition \p cond is `false`.

        \note Present even in release mode.

      \hideinitializer

      \hideinlinesource */

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/core.h>

#define simd_assert(cond,...)                                           \
  do {                                                                 \
    if (!cond) [[unlikely]] {                                          \
      []<typename ... Args> (Args && ... args) noexcept {              \
        if constexpr (sizeof...(Args) == 0)                            \
          spdlog::error(                                               \
            "{}:{}: assertion failed ({}) in function {}",             \
            __FILE__,__LINE__,#cond,__PRETTY_FUNCTION__);              \
        else if constexpr (sizeof...(Args) == 1)                       \
          spdlog::error(                                               \
            "{}:{}: assertion failed ({}) in function {}: {}",         \
            __FILE__,__LINE__,#cond,__PRETTY_FUNCTION__,               \
            fmt::format(std::forward<Args>(args)...));                 \
        else                                                           \
          spdlog::error(                                               \
            "{}:{}: assertion failed ({}) in function {}: {}",         \
            __FILE__,__LINE__,#cond,__PRETTY_FUNCTION__,               \
            fmt::format(std::forward<Args>(args)...));                 \
        std::abort();                                                  \
      }(__VA_ARGS__);                                                  \
    }                                                                  \
  } while (0)


#if defined(SIMD_TESTING) || defined(SIMD_TESTING_WAIT)
#include <unistd.h> // fork
//#include <sys/wait.h> // wait

TEST_CASE("assert","[assert]") {
  simd_assert(true);
  // REQUIRE_EXIT(simd_assert(false));
}

// TEST_CASE("simd_assert(false) should exit","[assert]") {
//   if (fork() == 0) {
//     simd_assert(false,"this should exit");
//     _exit(0);
//   }
//   else {
//     int status=0;
//     ::wait(&status);
//     REQUIRE(WIFEXITED(status));
//     REQUIRE(WEXITSTATUS(status) == 1);
//   }
// }

#endif
