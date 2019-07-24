#pragma once
#include "gfx/image.h"
#include "gfxopengl/glcore45.h"

namespace gfxopengl {

gl::GLuint createTexture(const gfx::ImageDesc &d, gl::GLenum& target);

struct Image {
	bool isRenderbuffer;
	gl::GLuint obj;
	gl::GLenum target;
	gfx::ImageDesc desc;
};

} // namespace gfxopengl