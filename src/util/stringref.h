#pragma once
#include <cstring>

namespace util {

struct StringRef {
  StringRef() : ptr{nullptr}, len{0} {}

  StringRef(const char *s) : ptr{s}, len{std::strlen(s)} {}
  StringRef(const char *s, size_t len) : ptr{ s }, len{ std::strlen(s) } {}

  size_t len;
  const char *ptr;
};

} // namespace util