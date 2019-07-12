#pragma once
#include <cstring>

namespace util {

struct StringRef {
  StringRef() : ptr{nullptr}, len{0} {}

  StringRef(const char *s) : ptr{s}, len{std::strlen(s)} {}

  size_t len;
  const char *ptr;
};

} // namespace util