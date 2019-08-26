#pragma once
#include <cstddef>

namespace util {
template <typename T> struct ArrayRef {
  T *data = nullptr;
  size_t len = 0;

  constexpr ArrayRef() noexcept  = default ;
  constexpr ArrayRef(T* data_, size_t len_) noexcept : data{ data_ }, len{ len_ } {}
  constexpr ArrayRef(std::nullptr_t) noexcept : data{ nullptr }, len{ 0 } {}

  template <std::size_t N>
  constexpr ArrayRef(T (&arr)[N]) noexcept : ArrayRef<T>(arr, N) {
  }

  T &operator[](std::size_t idx) { return data[idx]; }
  const T &operator[](std::size_t idx) const { return data[idx]; }

  T* begin() const noexcept {
	  return data;
  }

  T* end() const noexcept {
	  return data + len;
  }

};

template <typename T, size_t N>
ArrayRef<T> makeArrayRef(T (&a)[N]) {
	return ArrayRef<T>(a, N);
}

template <typename T, size_t N>
ArrayRef<const T> makeConstArrayRef(T(&a)[N]) {
	return ArrayRef<const T>(a, N);
}

} // namespace util