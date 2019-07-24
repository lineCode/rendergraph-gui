#include "gfxopengl/buffer.h"
#include "glcore45.h"

namespace gfxopengl {

gl::GLuint createBuffer(size_t byteSize, gl::GLenum flags, const void *initialData)
{
	gl::GLuint buf_obj;
  gl::CreateBuffers(1, &buf_obj);
  gl::NamedBufferStorage(buf_obj, byteSize, initialData, flags);
  return buf_obj;
}

} // namespace gfxopengl