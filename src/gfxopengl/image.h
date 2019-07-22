#pragma once
#include "gfx/image.h"
#include "gfxopengl/glcore45.h"
#include "gfxopengl/glhandle.h"

namespace gfxopengl {

//////////////////////////////////////////////
struct TextureDeleter {
  void operator()(gl::GLuint obj) { gl::DeleteTextures(1, &obj); }
};
struct RenderbufferDeleter {
  void operator()(gl::GLuint obj) { gl::DeleteRenderbuffers(1, &obj); }
};

class Image : public gfx::Image {
  friend class OpenGLGraphicsBackend;

public:
  Image(const gfx::ImageDesc &desc);

  gl::GLuint texObj() const { return texObj_.get(); }
  gl::GLuint renderbufferObj() const { return rbObj_.get(); }

private:
  GLHandle<TextureDeleter> texObj_;
  GLHandle<RenderbufferDeleter> rbObj_;
  int width_;
  int height_;
  int depth_;
};

} // namespace gfxopengl