#pragma once
#include "gfxopengl/glcore45types.h"

namespace gfxopengl {
	gl::GLuint createBuffer(size_t byteSize, gl::GLenum flags, const void *initialData);

	struct Buffer {
		gl::GLuint obj;
		bool own;
		gl::GLenum flags;
		size_t offset;
		size_t byteSize;
	};

} // namespace gfxopengl