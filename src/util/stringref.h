#pragma once
#include <cstring>
#include <string>

namespace util {

struct StringRef {
  StringRef() : ptr{nullptr}, len{0} {}

  StringRef(const char *s) : ptr{s}, len{std::strlen(s)} {}
  StringRef(const char *s, size_t len) : ptr{s}, len{std::strlen(s)} {}
  StringRef(const std::string &s) : ptr{s.c_str()}, len{s.size()} {}

  size_t len;
  const char *ptr;
};

} // namespace util