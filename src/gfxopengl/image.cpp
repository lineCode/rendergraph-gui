#include "gfxopengl/formatinfo.h"
#include "gfxopengl/opengl.h"
#include "gfxopengl/image.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>

namespace gfxopengl {

using namespace gfx;

gl::GLuint createTexture(const ImageDesc &d) {
  gl::GLenum target;
  switch (d.dimensions) {
  case ImageDimensions::Image1D:
    target = gl::TEXTURE_1D;
    break;
  case ImageDimensions::Image2D:
    if (d.sampleCount > 0) {
      target = gl::TEXTURE_2D_MULTISAMPLE;
    } else {
      target = gl::TEXTURE_2D;
    }
    break;
  case ImageDimensions::Image3D:
    target = gl::TEXTURE_3D;
    break;
  }

  const auto &glfmt = getGLImageFormatInfo(d.format);
  gl::GLuint tex_obj;
  gl::CreateTextures(target, 1, &tex_obj);

  switch (target) {
  case gl::TEXTURE_1D:
    gl::TextureStorage1D(tex_obj, d.mipMapCount, glfmt.internalFormat, d.width);
    break;
  case gl::TEXTURE_2D:
    gl::TextureStorage2D(tex_obj, d.mipMapCount, glfmt.internalFormat, d.width,
                         d.height);
    break;
  case gl::TEXTURE_2D_MULTISAMPLE:
    gl::TextureStorage2DMultisample(
        tex_obj, d.sampleCount, glfmt.internalFormat, d.width, d.height, true);
    break;
  case gl::TEXTURE_3D:
    gl::TextureStorage3D(tex_obj, 1, glfmt.internalFormat, d.width, d.height,
                         d.depth);
    break;
  }
  // set sensible defaults
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_WRAP_R, gl::CLAMP_TO_EDGE);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
  gl::TextureParameteri(tex_obj, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
  // fill desc
  return tex_obj;
}


Image::Image(const ImageDesc& desc) {
	// TODO prefer allocating a renderbuffer if the intended usage is only render target
	texObj_ = createTexture(desc);
}

/*
void Texture::upload(void *src, int mipLevel) {
  auto gl_fmt = getGLImageFormatInfo(desc_.fmt);
  switch (desc_.dims) {
  case ImageDimensions::Image1D:
    gl::TextureSubImage1D(obj_.get(), mipLevel, 0, desc_.width,
                          gl_fmt.external_fmt, gl_fmt.type, src);
    break;
  case ImageDimensions::Image2D:
    gl::TextureSubImage2D(obj_.get(), mipLevel, 0, 0, desc_.width, desc_.height,
                          gl_fmt.external_fmt, gl_fmt.type, src);
    break;
  case ImageDimensions::Image3D:
    gl::TextureSubImage3D(obj_.get(), mipLevel, 0, 0, 0, desc_.width,
                          desc_.height, desc_.depth, gl_fmt.external_fmt,
                          gl_fmt.type, src);
    break;
  }
}*/

int getTextureMipMapCount(int width, int height) {
  // 1000 is the default value of GL_TEXTURE_MAX_LEVEL
  return std::min((int)std::floor(std::log2(std::max(width, height))), 1000) -
         1;
}

} // namespace gfxopengl
