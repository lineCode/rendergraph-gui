#pragma once
#include <cstdlib>
#include <iostream>
#include <fmt/format.h>

namespace util {

template <typename... Args>
[[noreturn]] void panic(const char *file, int line, const char *fmt,
                        const Args &... args) noexcept {
  std::string msg;
  if (fmt)
    msg = fmt::format(fmt, args...);
  std::cerr << "Panic: at " << file << "(" << line << ") " << msg << "\n";
  std::abort();
}

} // namespace util

#define UT_PANIC_MSG(...) util::panic(__FILE__, __LINE__, __VA_ARGS__)
#define UT_PANIC util::panic(__FILE__, __LINE__, nullptr)
#define UT_UNREACHABLE util::panic(__FILE__, __LINE__, "unreachable code")
#define UT_UNIMPLEMENTED util::panic(__FILE__, __LINE__, "unimplemented")

