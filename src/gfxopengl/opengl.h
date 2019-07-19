#pragma once
#include "gfx/gfx.h"
#include "gfxopengl/glcore45.h"
#include <vector>
#include <cstdint>

namespace gfxopengl {

struct OpenGLContextInfo {
  int uniformBufferOffsetAlignment;
};

class Arena;

class Image : public gfx::Image {
	friend class OpenGLGraphicsBackend;
public:
	~Image();

	gl::GLuint obj() const { return obj_;  }

private:
	gl::GLuint obj_;
	int width_;
	int height_;
	int depth_;
};

class Buffer : public gfx::Buffer {
	friend class OpenGLGraphicsBackend;
public:
	gl::GLuint obj() const { return obj_; }
private:
	gl::GLuint obj_;
	size_t size_;
};

class ArgumentBlock : public gfx::ArgumentBlock {
public:
private:
};

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