#pragma once
#include <cstddef>

namespace util {
template <typename T> struct ArrayRef {
  size_t len;
  T *data;

  T &operator[](std::size_t idx) { return data[idx]; }
  const T &operator[](std::size_t idx) const { return data[idx]; }
};
} // namespace util