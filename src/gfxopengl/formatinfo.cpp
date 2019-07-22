#include "gfxopengl/formatinfo.h"
#include <stdexcept>

namespace gfxopengl {

static GLFormatInfo glfmt_rgba8_unorm{gl::RGBA8, gl::RGBA, gl::UNSIGNED_BYTE, 4,
                                      4};
static GLFormatInfo glfmt_r8_unorm{gl::R8, gl::RED, gl::UNSIGNED_BYTE, 1, 1};
static GLFormatInfo glfmt_r32_float{gl::R32F, gl::RED, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_rg32_float{gl::RG32F, gl::RG, gl::FLOAT, 2, 8};
static GLFormatInfo glfmt_rgba16_float{gl::RGBA16F, gl::RGBA, gl::FLOAT, 4, 8};
static GLFormatInfo glfmt_rgba32_float{gl::RGBA32F, gl::RGBA, gl::FLOAT, 4, 16};
static GLFormatInfo glfmt_rgba32_uint{gl::RGBA32UI, gl::RGBA, gl::UNSIGNED_INT,
                                      4, 16};
static GLFormatInfo glfmt_depth32_float{gl::DEPTH_COMPONENT32F,
                                        gl::DEPTH_COMPONENT, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_argb_10_10_10_2_unorm{
    gl::RGB10_A2, gl::DEPTH_COMPONENT, gl::FLOAT, 1, 4};
static GLFormatInfo glfmt_rgba8_unorm_srgb{gl::SRGB8_ALPHA8, gl::RGBA,
                                           gl::UNSIGNED_BYTE, 4, 4};
static GLFormatInfo glfmt_rg16_float{gl::RG16F, gl::RG, gl::FLOAT, 2, 4};
static GLFormatInfo glfmt_rg16_sint{gl::RG16I, gl::RG, gl::INT, 2, 4};

const GLFormatInfo &getGLImageFormatInfo(gfx::Format fmt) {
  switch (fmt) {
  case gfx::Format::R32G32B32A32_UINT:
    return glfmt_rgba32_uint;
  case gfx::Format::R16G16B16A16_SFLOAT:
    return glfmt_rgba16_float;
  case gfx::Format::R8G8B8A8_UNORM:
    return glfmt_rgba8_unorm;
  case gfx::Format::R8G8B8A8_SNORM:
    return glfmt_r8_unorm;
  case gfx::Format::R32_SFLOAT:
    return glfmt_r32_float;
  case gfx::Format::R32G32_SFLOAT:
    return glfmt_rg32_float;
  case gfx::Format::R32G32B32A32_SFLOAT:
    return glfmt_rgba32_float;
  case gfx::Format::D32_SFLOAT:
    return glfmt_depth32_float;
  case gfx::Format::A2R10G10B10_UNORM_PACK32:
    return glfmt_argb_10_10_10_2_unorm;
  case gfx::Format::R8G8B8A8_SRGB:
    return glfmt_rgba8_unorm_srgb;
  case gfx::Format::R16G16_SFLOAT:
    return glfmt_rg16_float;
  case gfx::Format::R16G16_SINT:
    return glfmt_rg16_sint;
  case gfx::Format::A2R10G10B10_SNORM_PACK32:
  // return glfmt_argb_10_10_10_2_snorm;   // there is no signed version of this
  // format in OpenGL
  default:
    throw std::logic_error("Unsupported image format");
  }
}
} // namespace gfxopengl