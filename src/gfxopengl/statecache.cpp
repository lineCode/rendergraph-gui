#include "gfxopengl/statecache.h"
#include "glcore45.h"
#include "util/panic.h"

namespace gfxopengl {
namespace {

gl::GLenum primitiveTopologyToGLenum(gfx::PrimitiveTopology topo) {
  switch (topo) {
  case gfx::PrimitiveTopology::TriangleList:
    return gl::TRIANGLES;
  case gfx::PrimitiveTopology::LineList:
    return gl::LINES;
  case gfx::PrimitiveTopology::PointList:
    return gl::POINTS;
  }
  UT_UNREACHABLE;
}

gl::GLenum stencilOpToGLenum(gfx::StencilOp op) {
  switch (op) {
  case gfx::StencilOp::Keep:
    return gl::KEEP;
  case gfx::StencilOp::Zero:
    return gl::ZERO;
  case gfx::StencilOp::Replace:
    return gl::REPLACE;
  case gfx::StencilOp::IncrementAndClamp:
    return gl::INCR;
  case gfx::StencilOp::DecrementAndClamp:
    return gl::DECR;
  case gfx::StencilOp::Invert:
    return gl::INVERT;
  case gfx::StencilOp::IncrementAndWrap:
    return gl::INCR_WRAP;
  case gfx::StencilOp::DecrementAndWrap:
    return gl::DECR_WRAP;
  }
  UT_UNREACHABLE;
}

gl::GLenum compareOpToGLenum(gfx::CompareOp op) {
  switch (op) {
  case gfx::CompareOp::Never:
    return gl::NEVER;
  case gfx::CompareOp::Less:
    return gl::LESS;
  case gfx::CompareOp::Equal:
    return gl::EQUAL;
  case gfx::CompareOp::LessOrEqual:
    return gl::LEQUAL;
  case gfx::CompareOp::Greater:
    return gl::GREATER;
  case gfx::CompareOp::NotEqual:
    return gl::NOTEQUAL;
  case gfx::CompareOp::GreaterOrEqual:
    return gl::GEQUAL;
  case gfx::CompareOp::Always:
    return gl::ALWAYS;
  }
  UT_UNREACHABLE;
}

gl::GLenum blendFactorToGLenum(gfx::BlendFactor f) {
  switch (f) {
  case gfx::BlendFactor::Zero:
    return gl::ZERO;
  case gfx::BlendFactor::One:
    return gl::ONE;
  case gfx::BlendFactor::SrcColor:
    return gl::SRC_COLOR;
  case gfx::BlendFactor::OneMinusSrcColor:
    return gl::ONE_MINUS_SRC_COLOR;
  case gfx::BlendFactor::DstColor:
    return gl::DST_COLOR;
  case gfx::BlendFactor::OneMinusDstColor:
    return gl::ONE_MINUS_DST_COLOR;
  case gfx::BlendFactor::SrcAlpha:
    return gl::SRC_ALPHA;
  case gfx::BlendFactor::OneMinusSrcAlpha:
    return gl::ONE_MINUS_SRC_ALPHA;
  case gfx::BlendFactor::DstAlpha:
    return gl::DST_ALPHA;
  case gfx::BlendFactor::OneMinusDstAlpha:
    return gl::ONE_MINUS_DST_ALPHA;
  case gfx::BlendFactor::ConstantColor:
    return gl::CONSTANT_COLOR;
  case gfx::BlendFactor::OneMinusConstantColor:
    return gl::ONE_MINUS_CONSTANT_COLOR;
  case gfx::BlendFactor::ConstantAlpha:
    return gl::CONSTANT_ALPHA;
  case gfx::BlendFactor::OneMinusConstantAlpha:
    return gl::ONE_MINUS_CONSTANT_ALPHA;
  case gfx::BlendFactor::SrcAlphaSaturate:
    return gl::SRC_ALPHA_SATURATE;
  case gfx::BlendFactor::Src1Color:
    return gl::SRC1_COLOR;
  case gfx::BlendFactor::OneMinusSrc1Color:
    return gl::ONE_MINUS_SRC1_COLOR;
  case gfx::BlendFactor::Src1Alpha:
    return gl::SRC1_ALPHA;
  case gfx::BlendFactor::OneMinusSrc1Alpha:
    return gl::ONE_MINUS_SRC1_ALPHA;
  }
  UT_UNREACHABLE;
}

gl::GLenum blendOpToGLenum(gfx::BlendOp op) {
  switch (op) {
  case gfx::BlendOp::Add:
    return gl::FUNC_ADD;
  case gfx::BlendOp::Subtract:
    return gl::FUNC_SUBTRACT;
  case gfx::BlendOp::ReverseSubtract:
    return gl::FUNC_REVERSE_SUBTRACT;
  case gfx::BlendOp::Min:
    return gl::MIN;
  case gfx::BlendOp::Max:
    return gl::MAX;
  }
  UT_UNREACHABLE;
}

} // namespace

void StateCache::setAllBlendStates(gfx::ColorBlendAttachmentState blend) {
  for (int i = 0; i < MAX_VIEWPORTS; ++i) {
    updateCached(colorBlend[i].known, colorBlend[i].state, blend, [&]() {
      gl::Enablei(gl::BLEND, i);
      gl::BlendEquationSeparatei(i,
                                 blendOpToGLenum(colorBlend[i].state.colorOp),
                                 blendOpToGLenum(colorBlend[i].state.alphaOp));
      gl::BlendFuncSeparatei(i,
                             blendFactorToGLenum(colorBlend[i].state.srcColor),
                             blendFactorToGLenum(colorBlend[i].state.dstColor),
                             blendFactorToGLenum(colorBlend[i].state.srcAlpha),
                             blendFactorToGLenum(colorBlend[i].state.dstAlpha));
    });
  }
}

} // namespace gfxopengl