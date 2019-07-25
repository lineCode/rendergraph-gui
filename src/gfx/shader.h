#pragma once
#include "util/bitflags.h"

namespace gfx {
	enum class ShaderStageFlags {
		VERTEX = (1 << 0),
		GEOMETRY = (1 << 1),
		FRAGMENT = (1 << 2),
		TESS_CONTROL = (1 << 3),
		TESS_EVAL = (1 << 4),
		COMPUTE = (1 << 5),
	};

	ENUM_BIT_FLAGS_OPERATORS(ShaderStageFlags)
}