#pragma once
#include "gfx/shader.h"
#include "glcore45types.h"
#include "util/stringref.h"
#include <string>

namespace gfxopengl {

gl::GLenum shaderStageToGLenum(gfx::ShaderStageFlags stage);

gl::GLuint createShader(gl::GLenum stage, util::StringRef source,
                        std::string &log);

bool linkProgram(gl::GLuint program, std::string &log);

} // namespace gfxopengl