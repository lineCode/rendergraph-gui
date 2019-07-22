#pragma once
#include "gfx/buffer.h"
#include "gfxopengl/glhandle.h"

namespace gfxopengl {
//////////////////////////////////////////////
struct BufferDeleter {
  void operator()(gl::GLuint obj) { gl::DeleteBuffers(1, &obj); }
};

class Buffer : public gfx::Buffer {
public:
  struct Slice {
    gl::GLuint obj;
    size_t offset;
    size_t size;
  };

  Buffer(size_t byteSize, gl::GLenum flags, const void *initialData = nullptr);

  gl::GLuint object() const { return obj_.get(); }
  size_t size() const { return byteSize_; }

private:
  gl::GLenum flags_;
  size_t byteSize_;
  GLHandle<BufferDeleter> obj_;
};
} // namespace gfxopengl