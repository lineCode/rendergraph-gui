#pragma once
#include <vector>
#include "gfx/argumentblock.h"
#include "glcore45types.h"

namespace gfxopengl {

struct ArgumentBlock {
	std::vector<gl::GLuint> textures;
	std::vector<gl::GLuint> samplers;
	std::vector<gl::GLuint> images;
	std::vector<gl::GLuint> uniformBuffers;
	std::vector<gl::GLsizeiptr> uniformBufferSizes;
	std::vector<gl::GLintptr> uniformBufferOffsets;
	std::vector<gl::GLuint> shaderStorageBuffers;
	std::vector<gl::GLsizeiptr> shaderStorageBufferSizes;
	std::vector<gl::GLintptr> shaderStorageBufferOffsets;
	std::vector<gl::GLuint> vertexBuffers;
	std::vector<gl::GLintptr> vertexBufferOffsets;
	std::vector<gl::GLsizei> vertexBufferStrides;
	gl::GLuint indexBuffer;
	gl::GLsizeiptr indexBufferOffset;
	gl::GLenum indexBufferType;
};

} // namespace gfxopengl