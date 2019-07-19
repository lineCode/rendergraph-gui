#include "opengl.h"
#include "gfxopengl/glcore45.h"
#include "util/log.h"
#include <stdexcept>

namespace gfxopengl {

static void APIENTRY debugCallback(gl::GLenum source, gl::GLenum type,
                                   gl::GLuint id, gl::GLenum severity,
                                   gl::GLsizei length, const gl::GLubyte *msg,
                                   void *data) {
  if (severity != gl::DEBUG_SEVERITY_LOW &&
      severity != gl::DEBUG_SEVERITY_NOTIFICATION)
    util::log("GL: {}", msg);
}

static void setDebugCallback() {
  gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
  gl::DebugMessageCallback((gl::GLDEBUGPROC)debugCallback, nullptr);
  gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0,
                          nullptr, true);
  gl::DebugMessageInsert(gl::DEBUG_SOURCE_APPLICATION, gl::DEBUG_TYPE_MARKER,
                         1111, gl::DEBUG_SEVERITY_NOTIFICATION, -1,
                         "Started logging OpenGL messages");
}

static void getContextInfo(OpenGLContextInfo &out) {
  // TODO query all implementation limits
  gl::GetIntegerv(gl::UNIFORM_BUFFER_OFFSET_ALIGNMENT,
                  &out.uniformBufferOffsetAlignment);
}

OpenGLGraphicsBackend::OpenGLGraphicsBackend() {
  if (!gl::sys::LoadFunctions()) {
    throw std::runtime_error{"could not load OpenGL function pointers"};
  }

  setDebugCallback();
}

OpenGLGraphicsBackend::~OpenGLGraphicsBackend() {
}

//////////////////////////////////////////////////////////////////////////////
class Arena : public gfx::Arena {
	friend class OpenGLGraphicsBackend;
public:

private:
	std::vector<std::unique_ptr<Image>> images;
	std::vector<std::unique_ptr<Buffer>> buffers;
	std::vector<std::unique_ptr<ArgumentBlock>> argBlocks;
};

/*
static void pushDebugGroup(const char *message) {
        gl::PushDebugGroup(gl::DEBUG_SOURCE_APPLICATION, 0,
                static_cast<gl::GLsizei>(std::strlen(message)), message);
}

static void popDebugGroup() { gl::PopDebugGroup(); }
*/

} // namespace gfxopengl
