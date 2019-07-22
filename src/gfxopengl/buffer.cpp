#include "gfxopengl/buffer.h"

namespace gfxopengl {

Buffer::Buffer(size_t byteSize, gl::GLenum flags,
               const void *initialData )
    : flags_{flags}, byteSize_{byteSize} {
  gl::GLuint buf_obj;
  gl::CreateBuffers(1, &buf_obj);
  gl::NamedBufferStorage(buf_obj, byteSize, initialData, flags);
  obj_ = buf_obj;
}

} // namespace gfxopengl