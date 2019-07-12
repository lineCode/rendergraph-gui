#pragma once
#include <cstddef>

namespace util {
	template<typename T> struct ArrayRef {
		size_t len;
		T* data;
	};
}