#pragma once
#include <fmt/format.h>

namespace util {
	template<typename... Args>
	void log(const char* fmt, const Args&... args) {
		auto msg = fmt::format(fmt, args...);
		log(msg.c_str());
	}

	void log(const char* msg);
}