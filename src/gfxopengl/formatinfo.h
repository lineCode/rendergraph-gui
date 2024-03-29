#pragma once
#include "gfx/format.h"
#include "gfxopengl/glcore45.h"

namespace gfxopengl {
/// Structure containing information about the OpenGL internal format
/// corresponding to an 'Format'
struct GLFormatInfo {
  gl::GLenum internalFormat; //< Corresponding internal format
  gl::GLenum externalFormat; //< Preferred external format for uploads/reads
  gl::GLenum type;           //< Preferred element type for uploads/reads
  int componentCount; //< number of components (channels) (TODO redundant)
  int size;           //< Size of one pixel in bytes
};

/// Returns information about the OpenGL internal format corresponding to the
/// specified Format
const GLFormatInfo &getGLImageFormatInfo(gfx::Format fmt);

} // namespace gfxopengl