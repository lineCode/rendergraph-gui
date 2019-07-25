#pragma once
#include <cstddef>

namespace util {
template <typename T> struct ArrayRef {
  T *data = nullptr;
  size_t len = 0;

  ArrayRef() = default;
  ArrayRef(size_t len_, T* data_) : data{ data_ }, len{ len_ } {}
  ArrayRef(std::nullptr_t) : data{ nullptr }, len{ 0 } {}

  T &operator[](std::size_t idx) { return data[idx]; }
  const T &operator[](std::size_t idx) const { return data[idx]; }
};

} // namespace util