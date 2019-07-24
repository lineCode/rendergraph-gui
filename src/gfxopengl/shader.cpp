#include "gfxopengl/shader.h"
#include "gfxopengl/glcore45.h"
#include "util/log.h"
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>

namespace gfxopengl {

gl::GLenum shaderStageToGLenum(gfx::ShaderStageFlags stage) {
  switch (stage) {
  case gfx::ShaderStageFlags::VERTEX:
    return gl::VERTEX_SHADER;
  case gfx::ShaderStageFlags::FRAGMENT:
    return gl::FRAGMENT_SHADER;
  case gfx::ShaderStageFlags::GEOMETRY:
    return gl::GEOMETRY_SHADER;
  case gfx::ShaderStageFlags::TESS_CONTROL:
    return gl::TESS_CONTROL_SHADER;
  case gfx::ShaderStageFlags::TESS_EVAL:
    return gl::TESS_EVALUATION_SHADER;
  case gfx::ShaderStageFlags::COMPUTE:
    return gl::COMPUTE_SHADER;
  default:
    assert(1);
  }
}

const char *getShaderStageName(gl::GLenum stage) {
  switch (stage) {
  case gl::VERTEX_SHADER:
    return "VERTEX_SHADER";
  case gl::FRAGMENT_SHADER:
    return "FRAGMENT_SHADER";
  case gl::GEOMETRY_SHADER:
    return "GEOMETRY_SHADER";
  case gl::TESS_CONTROL_SHADER:
    return "TESS_CONTROL_SHADER";
  case gl::TESS_EVALUATION_SHADER:
    return "TESS_EVALUATION_SHADER";
  case gl::COMPUTE_SHADER:
    return "COMPUTE_SHADER";
  default:
    return "<invalid>";
  }
}
/*
void dumpCompileLog(ShaderModule &sh, gl::GLenum stage, std::ostream &out,
            const char *fileHint = "<unknown>") {
auto status = sh.getCompileStatus();
auto log = sh.getCompileLog();
if (!status) {
util::log(
"===============================================================");
util::log("Shader compilation error (file: {}, stage: {})", fileHint,
      getShaderStageName(stage));
util::log("Compilation log follows:\n\n{}\n\n", log);

} else if (!log.empty()) {
util::log("Shader compilation messages (file: {}, stage: {})", fileHint,
      getShaderStageName(stage));
util::log("{}", log);
}
}

void dumpLinkLog(ProgramObject &prog, std::ostream &out,
         const char *fileHint = "<unknown>") {
auto status = prog.getLinkStatus();
auto log = prog.getLinkLog();
if (!status) {
util::log(
"===============================================================");
util::log("Program link error");
util::log("Link log follows:\n\n{}\n\n", log);
} else if (!log.empty()) {
util::log("Program link messages:");
util::log("{}", log);
}
}
*/

gl::GLuint createShader(gl::GLenum stage, util::StringRef source,
                        std::string &log) {

  gl::GLuint obj = gl::CreateShader(stage);
  std::string src{source.ptr, source.len};
  const char *shaderSources[1] = {src.c_str()};
  gl::ShaderSource(obj, 1, shaderSources, NULL);
  gl::CompileShader(obj);

  // get log
  gl::GLint logsize = 0;
  gl::GetShaderiv(obj, gl::INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    gl::GetShaderInfoLog(obj, logsize, &logsize, logbuf);
    log.assign(logbuf);
    delete[] logbuf;
  }

  // if failure, delete and return 0
  gl::GLint status = gl::TRUE_;
  gl::GetShaderiv(obj, gl::COMPILE_STATUS, &status);
  if (status != gl::TRUE_) {
    gl::DeleteShader(obj);
    return 0;
  }

  return obj;
}

bool linkProgram(gl::GLuint program, std::string &log) {
  gl::LinkProgram(program);

  gl::GLint logsize = 0;
  gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &logsize);
  if (logsize != 0) {
    char *logbuf = new char[logsize];
    gl::GetProgramInfoLog(program, logsize, &logsize, logbuf);
    log.assign(logbuf);
    delete[] logbuf;
  }

  gl::GLint status = gl::TRUE_;
  gl::GetProgramiv(program, gl::LINK_STATUS, &status);
  return status == gl::TRUE_;
}

} // namespace gfxopengl
