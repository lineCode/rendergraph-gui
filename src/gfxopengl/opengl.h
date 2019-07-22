#pragma once
#include "gfx/gfx.h"
#include <vector>
#include <cstdint>

namespace gfxopengl {

struct OpenGLContextInfo {
  int uniformBufferOffsetAlignment;
};

class Arena;


class OpenGLGraphicsBackend : public gfx::GraphicsBackend {
public:
  /// Attempts to create an OpenGL graphics context from the current context.
  /// Throws an error if no context exists on the current thread.
  OpenGLGraphicsBackend();

  ~OpenGLGraphicsBackend();

  const OpenGLContextInfo &contextInfo() const { return ctxInfo; }

private:
  OpenGLContextInfo ctxInfo;
  int maxFramesInFlight = 1;
};

} // namespace gfxopengl