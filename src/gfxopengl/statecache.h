#pragma once
#include "gfx/pipeline.h"
#include "glcore45types.h"
#include <array>

namespace gfxopengl {

constexpr int MAX_VIEWPORTS = 32;

struct ColorBlendCache {
  bool known = false;
  gfx::ColorBlendAttachmentState state;
};

struct ViewportCache {
  bool known = false;
  gfx::ViewportState state;
};

struct ScissorCache {
  bool known = false;
  gfx::ScissorState state;
};

namespace {
template <typename T, typename Callback>
void updateCached(bool &known, T &oldVal, T newVal, Callback callback) {
  if (!known || oldVal != newVal) {
    callback();
    oldVal = newVal;
    known = true;
  }
}
} // namespace

// We keep a cache of OpenGL states set in the pipeline so that we don't make
// unnecessary calls.
struct StateCache {
  size_t maxDrawBuffers;
  size_t maxColorAttachments;
  size_t maxViewports;
  bool cullEnable;
  bool cullMode;
  gfx::PolygonMode polygonMode;
  // front_face: Option<GLenum>,
  bool knowProgram;
  gl::GLuint program;
  bool knowVertexArray;
  gl::GLuint vertexArray;
  bool knowFramebuffer;
  gl::GLuint framebuffer;
  bool knowStencilTestEnabled = false;
  bool stencilTestEnabled;
  bool knowStencilFront;
  gfx::StencilOpState stencilFront;
  bool knowStencilBack;
  gfx::StencilOpState stencilBack;
  bool knowDepthTestEnabled = false;
  bool depthTestEnabled;
  bool knowDepthWriteEnabled = false;
  bool depthWriteEnabled;
  bool knowDepthCompareOp = false;
  gfx::CompareOp depthCompareOp;
  std::array<ColorBlendCache, MAX_VIEWPORTS> colorBlend;
  std::array<ViewportCache, MAX_VIEWPORTS> viewports;
  std::array<ScissorCache, MAX_VIEWPORTS> scissors;
  bool knowIndexBuffer = false;
  gl::GLuint indexBufferObj;
  size_t indexBufferSize;
  size_t indexBufferOffset;
  gl::GLenum indexBufferType;

  void setProgram(gl::GLuint program);
  void setVertexArray(gl::GLuint vao);
  void setDrawFramebuffer(gl::GLuint drawFramebuffer);
  void setAllBlendStates(gfx::ColorBlendAttachmentState blend);

};

} // namespace gfxopengl