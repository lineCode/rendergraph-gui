#include "gfxopengl/opengl.h"
#include "gfx/gfx.h"
#include "gfx/pipeline.h"
#include "gfx/signature.h"
#include "gfxopengl/argumentblock.h"
#include "gfxopengl/buffer.h"
#include "gfxopengl/formatinfo.h"
#include "gfxopengl/glcore45.h"
#include "gfxopengl/image.h"
#include "gfxopengl/shader.h"
#include "gfxopengl/sync.h"
#include "gfxopengl/uploadbuffer.h"
#include "util/log.h"
#include "util/panic.h"
#include <deque>
#include <stdexcept>
#include <unordered_map>

namespace gfxopengl {

constexpr size_t DEFAULT_UPLOAD_BUFFER_SIZE = 4 * 1024 * 1024;

/////////////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////////////
struct SignatureInner {
  std::vector<std::shared_ptr<SignatureInner>> inherited;
  std::vector<gfx::ResourceBinding> shaderResources;
  std::vector<gfx::VertexInputBinding> vertexInputs;
  gfx::IndexFormat indexFormat;
  size_t viewportsCount;
  size_t scissorsCount;
};

struct Signature {
  std::shared_ptr<SignatureInner> ptr;
};

/*
struct SyncedBuffer {
  gl::GLsync sync;
  std::unique_ptr<MappedBuffer> buffer;
};*/

struct ResourceGroup {
  std::vector<gl::GLuint> buffers;
  std::vector<gl::GLuint> textures;
  std::vector<gl::GLuint> renderbuffers;
  std::vector<gl::GLuint> framebuffers;
};

struct SyncResourceGroup {
  gl::GLsync sync;
  ResourceGroup resources;
};

gl::GLenum filterToGLenum(gfx::SamplerDesc::Filter filter,
                          gfx::SamplerDesc::MipMapMode mipMapMode) {

  switch (filter) {
  case gfx::SamplerDesc::Filter::Linear:
    switch (mipMapMode) {
    case gfx::SamplerDesc::MipMapMode::None:
      return gl::LINEAR;
    case gfx::SamplerDesc::MipMapMode::Nearest:
      return gl::LINEAR_MIPMAP_NEAREST;
    case gfx::SamplerDesc::MipMapMode::Linear:
      return gl::LINEAR_MIPMAP_LINEAR;
    }
    break;
  case gfx::SamplerDesc::Filter::Nearest:
    switch (mipMapMode) {
    case gfx::SamplerDesc::MipMapMode::None:
      return gl::NEAREST;
    case gfx::SamplerDesc::MipMapMode::Nearest:
      return gl::NEAREST_MIPMAP_NEAREST;
    case gfx::SamplerDesc::MipMapMode::Linear:
      return gl::NEAREST_MIPMAP_LINEAR;
    }
    break;
  }
  UT_UNREACHABLE;
}

gl::GLenum textureAddressModeToGLenum(gfx::SamplerDesc::AddressMode mode) {
  switch (mode) {
  case gfx::SamplerDesc::AddressMode::Repeat:
    return gl::REPEAT;
  case gfx::SamplerDesc::AddressMode::Clamp:
    return gl::CLAMP_TO_EDGE;
  case gfx::SamplerDesc::AddressMode::Border:
    return gl::CLAMP_TO_BORDER;
  case gfx::SamplerDesc::AddressMode::Mirror:
    return gl::MIRRORED_REPEAT;
  }
  UT_UNREACHABLE;
}

gl::GLuint createSampler(const gfx::SamplerDesc &desc) {
  gl::GLuint sampler_obj;
  gl::CreateSamplers(1, &sampler_obj);
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MIN_FILTER,
                        filterToGLenum(desc.minFilter, desc.mipMapMode));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_MAG_FILTER,
                        filterToGLenum(desc.magFilter, desc.mipMapMode));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_R,
                        textureAddressModeToGLenum(desc.addrU));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_S,
                        textureAddressModeToGLenum(desc.addrV));
  gl::SamplerParameteri(sampler_obj, gl::TEXTURE_WRAP_T,
                        textureAddressModeToGLenum(desc.addrW));
  float borderColor[4] = {desc.borderColor.r, desc.borderColor.g,
                          desc.borderColor.b, desc.borderColor.a};
  gl::SamplerParameterfv(sampler_obj, gl::TEXTURE_BORDER_COLOR, borderColor);
  return sampler_obj;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
struct OpenGLGraphicsBackend::Private {
  ResourceGroup frameResources;
  std::vector<SyncResourceGroup> pendingResources;
  // std::deque<SyncedBuffer> uploadBuffers;
  std::unordered_map<gfx::SamplerDesc, gl::GLuint, SamplerHash> samplerCache;
  int maxFramesInFlight = 2;
  SyncTimeline frameTimeline;

  Private() {}

  gl::GLuint getSamplerObject(const gfx::SamplerDesc &desc) {
    auto it = samplerCache.find(desc);
    if (it != samplerCache.end()) {
      return it->second;
    } else {
      auto obj = createSampler(desc);
      auto result = samplerCache.emplace(desc, obj);
      return result.first->second;
    }
  }

  /*size_t writeToArena(Arena &a, const void *data, size_t len, size_t align,
                      gl::GLuint &outBufferObj) {
    auto offset = alignOffset(len, align, a.uploadCurrentOffset_,
                              a.uploadStartOffset_ + a.uploadSize_);

    if (offset == -1) {
      // not enough space in current buffer, recycle
      // put current buffer in the recycling list
      addSyncedBuffer(std::move(a.buffer_));
      // request a new one
      a.buffer_ = requestMappedBuffer();
      // reset pointers
      a.uploadCurrentOffset_ = 0;
      a.uploadStartOffset_ = 0;
      a.uploadSize_ = 0;
      // try again
      offset = alignOffset(len, align, a.uploadCurrentOffset_,
                           a.uploadStartOffset_ + a.uploadSize_);
    }

    a.buffer_->writeAt(offset, data, len);
    outBufferObj = a.buffer_->object();
    a.uploadCurrentOffset_ = offset + len;
    return offset;
  }*/
};

/////////////////////////////////////////////////////////////////////////////////////////////////
OpenGLGraphicsBackend::OpenGLGraphicsBackend() {
  auto ctx = wglGetCurrentContext();
  if (!ctx) {
    throw std::runtime_error{"no current context"};
  }

  if (!gl::sys::LoadFunctions()) {
    throw std::runtime_error{"could not load OpenGL function pointers"};
  }

  setDebugCallback();
  d = std::make_unique<Private>();
}

OpenGLGraphicsBackend::~OpenGLGraphicsBackend() {}

gfx::ImageHandle
OpenGLGraphicsBackend::createImage(const gfx::ImageDesc &desc) {
  gl::GLenum target;
  gl::GLuint tex = createTexture(desc, target);
  auto img = new Image;
  img->isRenderbuffer = false;
  img->obj = tex;
  img->target = target;
  img->desc = desc;
  return (gfx::ImageHandle)img;
}

void OpenGLGraphicsBackend::deleteImage(gfx::ImageHandle handle) {
  auto img = (Image *)handle;
  gl::DeleteTextures(1, &img->obj);
  delete img;
}

void OpenGLGraphicsBackend::updateImageData(gfx::ImageHandle image, int x,
                                            int y, int z, int width, int height,
                                            int depth, const void *data) {
  // TODO
}

gfx::ShaderModuleHandle
OpenGLGraphicsBackend::createShaderModule(util::StringRef source,
                                          gfx::ShaderStageFlags stage) {
  std::string log;
  gl::GLenum stageGl = shaderStageToGLenum(stage);
  gl::GLuint obj = createShader(stageGl, source, log);
  return (gfx::ShaderModuleHandle)obj;
}

void OpenGLGraphicsBackend::deleteShaderModule(gfx::ShaderModuleHandle handle) {
  gl::DeleteShader((gl::GLuint)handle);
}

gfx::SignatureHandle OpenGLGraphicsBackend::createSignature(
    util::ArrayRef<gfx::SignatureHandle> inherited,
    const gfx::SignatureDesc &desc) {
  auto sig = std::make_shared<SignatureInner>();
  for (int i = 0; i < inherited.len; ++i) {
    sig->inherited.push_back(((const Signature *)inherited[i])->ptr);
  }
  sig->shaderResources.assign(desc.shaderResources.data,
                              desc.shaderResources.data +
                                  desc.shaderResources.len);
  sig->vertexInputs.assign(desc.vertexInputs.data,
                           desc.vertexInputs.data + desc.vertexInputs.len);
  sig->indexFormat = desc.indexFormat;
  sig->viewportsCount = desc.viewportsCount;
  sig->scissorsCount = desc.scissorsCount;
  auto s = new Signature;
  s->ptr = std::move(sig);
  return (gfx::SignatureHandle)s;
}

void OpenGLGraphicsBackend::deleteSignature(gfx::SignatureHandle handle) {
  auto s = (Signature *)handle;
  delete s;
}

struct ArgumentBlock {
  std::shared_ptr<SignatureInner> sig;
  std::vector<gl::GLuint> textures;
  std::vector<gl::GLuint> samplers;
  std::vector<gl::GLuint> images;
  std::vector<gl::GLuint> uniformBuffers;
  std::vector<gl::GLsizeiptr> uniformBufferSizes;
  std::vector<gl::GLintptr> uniformBufferOffsets;
  std::vector<gl::GLuint> shaderStorageBuffers;
  std::vector<gl::GLsizeiptr> shaderStorageBufferSizes;
  std::vector<gl::GLintptr> shaderStorageBufferOffsets;
  std::vector<gl::GLuint> vertexBuffers;
  std::vector<gl::GLintptr> vertexBufferOffsets;
  std::vector<gl::GLsizei> vertexBufferStrides;
  gl::GLuint indexBuffer = 0;
  gl::GLsizeiptr indexBufferOffset = 0;
  gl::GLenum indexBufferType = 0;
};

gfx::ArgumentBlockHandle
OpenGLGraphicsBackend::createArgumentBlock(gfx::SignatureHandle signature) {
  auto argblock = new ArgumentBlock;
  Signature *sig = (Signature *)signature;
  argblock->sig = sig->ptr;
  return (gfx::ArgumentBlockHandle)argblock;
}

void OpenGLGraphicsBackend::deleteArgumentBlock(
    gfx::ArgumentBlockHandle handle) {
  ArgumentBlock *argblock = (ArgumentBlock *)handle;
  // can delete now
  delete argblock;
}

void OpenGLGraphicsBackend::argumentBlockSetArgumentBlock(
    gfx::ArgumentBlockHandle handle, int index,
    gfx::ArgumentBlockHandle block) {
  throw std::logic_error{"unimplemented"};
}

void OpenGLGraphicsBackend::argumentBlockSetShaderResource(
    gfx::ArgumentBlockHandle handle, int resourceIndex,
    gfx::SampledImageView imgView) {
  // set texture
  ArgumentBlock *argblock = (ArgumentBlock *)handle;
  Image *img = (Image *)handle;
  if (argblock->textures.size() <= resourceIndex)
    argblock->textures.resize(resourceIndex + 1, 0);
  if (argblock->samplers.size() <= resourceIndex)
    argblock->samplers.resize(resourceIndex + 1, 0);
  if (img->isRenderbuffer)
    throw std::logic_error{"image cannot be bound as a texture"};
  argblock->textures[resourceIndex] = img->obj;
  argblock->samplers[resourceIndex] = img->obj;
}

void OpenGLGraphicsBackend::argumentBlockSetShaderResource(
    gfx::ArgumentBlockHandle argBlock, int resourceIndex,
    gfx::ConstantBufferView buf) {
  // TODO
}

void OpenGLGraphicsBackend::argumentBlockSetShaderResource(
    gfx::ArgumentBlockHandle argBlock, int resourceIndex,
    gfx::StorageBufferView buf) {
  // TODO
}

void OpenGLGraphicsBackend::argumentBlockSetVertexBuffer(
    gfx::ArgumentBlockHandle argBlock, int index, gfx::VertexBufferView vbv) {
  ArgumentBlock *a = (ArgumentBlock *)argBlock;

  if (a->vertexBuffers.size() <= index)
    a->vertexBuffers.resize(index + 1, 0);
  if (a->vertexBufferOffsets.size() <= index)
    a->vertexBufferOffsets.resize(index + 1, 0);
  if (a->vertexBufferStrides.size() <= index)
    a->vertexBufferStrides.resize(index + 1, 0);

  Buffer *buf = (Buffer *)vbv.buffer;
  a->vertexBuffers[index] = buf->obj;
  a->vertexBufferOffsets[index] = buf->offset + vbv.offset;
  a->vertexBufferStrides[index] = a->sig->vertexInputs[index].layout.stride;
}

void OpenGLGraphicsBackend::argumentBlockSetIndexBuffer(
    gfx::ArgumentBlockHandle argBlock, gfx::IndexBufferView buf) {
  // TODO
}

struct RenderPass {};

gfx::RenderPassHandle OpenGLGraphicsBackend::createRenderPass(
    util::ArrayRef<gfx::RenderPassTargetDesc> colorTargets,
    const gfx::RenderPassTargetDesc *depthTarget) {
  RenderPass *renderPass = new RenderPass;
  return (gfx::RenderPassHandle)renderPass;
}

void OpenGLGraphicsBackend::deleteRenderPass(gfx::RenderPassHandle handle) {

  RenderPass *renderPass = (RenderPass *)handle;
  delete renderPass;
}

gl::GLuint
createVertexArrayObject(util::ArrayRef<gfx::VertexInputBinding> bindings) {
  gl::GLuint vao;
  gl::CreateVertexArrays(1, &vao);
  int attribindex = 0;
  for (int i = 0; i < bindings.len; ++i) {
    for (int j = 0; j < bindings.data[i].layout.elements.len; ++j) {
      const gfx::VertexLayoutElement &e =
          bindings.data[i].layout.elements.data[j];
      const GLFormatInfo &fmt = getGLImageFormatInfo(e.format);
      gl::EnableVertexArrayAttrib(vao, attribindex);
      gl::VertexArrayAttribFormat(vao, attribindex, fmt.componentCount,
                                  fmt.type, false,
                                  e.offset); // TODO normalized formats
      gl::VertexArrayAttribBinding(vao, attribindex, i);
      ++attribindex;
    }
  }

  return vao;
}

struct GraphicsPipeline {
  gl::GLuint program;
  gl::GLuint vao;
  gfx::ViewportState viewportState;
  gfx::RasterizationState rasterizationState;
  gfx::MultisampleState multisampleState;
  gfx::DepthStencilState depthStencilState;
  gfx::InputAssemblyState inputAssemblyState;
  gfx::ColorBlendState colorBlendState;
};

gfx::GraphicsPipelineHandle OpenGLGraphicsBackend::createGraphicsPipeline(
    const gfx::GraphicsPipelineDesc &desc) {
  if (!desc.shaderStages.vertex || !desc.shaderStages.fragment) {
    throw std::logic_error{"must define at least a vertex shader and a "
                           "fragment shader to create a pipeline"};
  }

  gl::GLuint program = gl::CreateProgram();
  gl::AttachShader(program, (gl::GLuint)desc.shaderStages.vertex);
  gl::AttachShader(program, (gl::GLuint)desc.shaderStages.fragment);
  if (desc.shaderStages.tessControl)
    gl::AttachShader(program, (gl::GLuint)desc.shaderStages.tessControl);
  if (desc.shaderStages.tessEval)
    gl::AttachShader(program, (gl::GLuint)desc.shaderStages.tessEval);
  if (desc.shaderStages.geometry)
    gl::AttachShader(program, (gl::GLuint)desc.shaderStages.geometry);

  std::string log;
  if (!linkProgram(program, log)) {
    util::log("failed to link program: {}", log);
    return 0;
  }

  // make VAO from signature
  Signature *signature = (Signature *)desc.signature;
  gl::GLuint vao =
      createVertexArrayObject({signature->ptr->vertexInputs.size(),
                               signature->ptr->vertexInputs.data()});

  GraphicsPipeline *gp = new GraphicsPipeline;
  gp->program = program;
  gp->vao = vao;
  gp->viewportState = desc.viewportState;
  gp->rasterizationState = desc.rasterizationState;
  gp->multisampleState = desc.multisampleState;
  gp->depthStencilState = desc.depthStencilState;
  gp->inputAssemblyState = desc.inputAssemblyState;
  gp->colorBlendState = desc.colorBlendState;
  return (gfx::GraphicsPipelineHandle)gp;
}

void OpenGLGraphicsBackend::deleteGraphicsPipeline(
    gfx::GraphicsPipelineHandle handle) {}

gfx::FramebufferHandle OpenGLGraphicsBackend::createFramebuffer(
    util::ArrayRef<gfx::RenderTargetView> colorTargets,
    gfx::DepthStencilRenderTargetView *depthTarget) {
  gl::GLuint fbo;
  gl::CreateFramebuffers(1, &fbo);
  static const gl::GLenum drawBuffers[8] = {
      gl::COLOR_ATTACHMENT0,     gl::COLOR_ATTACHMENT0 + 1,
      gl::COLOR_ATTACHMENT0 + 2, gl::COLOR_ATTACHMENT0 + 3,
      gl::COLOR_ATTACHMENT0 + 4, gl::COLOR_ATTACHMENT0 + 5,
      gl::COLOR_ATTACHMENT0 + 6, gl::COLOR_ATTACHMENT0 + 7};
  gl::NamedFramebufferDrawBuffers(fbo, (gl::GLsizei)colorTargets.len,
                                  drawBuffers);

  for (int i = 0; i < colorTargets.len; ++i) {
    Image *img = (Image *)colorTargets.data[i].image;

    if (img->isRenderbuffer) {
      gl::NamedFramebufferRenderbuffer(fbo, gl::COLOR_ATTACHMENT0 + i,
                                       gl::RENDERBUFFER, img->obj);
    } else {
      gl::NamedFramebufferTexture(fbo, gl::COLOR_ATTACHMENT0 + i, img->obj, 0);
    }
  }

  if (gl::CheckNamedFramebufferStatus(fbo, gl::DRAW_FRAMEBUFFER) !=
      gl::FRAMEBUFFER_COMPLETE) {
    gl::DeleteFramebuffers(1, &fbo);
    util::log("failed to create framebuffer");
    return 0;
  }

  return fbo;
}

void OpenGLGraphicsBackend::deleteFramebuffer(gfx::FramebufferHandle handle) {
  gl::GLuint fbo = (gl::GLuint)handle;
  gl::DeleteFramebuffers(1, &fbo);
}

gfx::BufferHandle OpenGLGraphicsBackend::createConstantBuffer(const void *data,
                                                              size_t len) {
  gl::GLuint obj = createBuffer(len, 0, data);
  Buffer *b = new Buffer;
  b->byteSize = len;
  b->offset = 0;
  b->own = true;
  b->flags = 0;
  b->obj = obj;
  return (gfx::BufferHandle)b;
}

void OpenGLGraphicsBackend::deleteBuffer(gfx::BufferHandle handle) {
  Buffer *b = (Buffer *)handle;
  if (b->own) {
    gl::DeleteFramebuffers(1, &b->obj);
  }
  delete b;
}

void OpenGLGraphicsBackend::clearRenderTarget(gfx::RenderTargetView view,
                                              const gfx::ColorF &clearColor) {
  Image *image = (Image *)view.image;

  if (image->isRenderbuffer) {
    // TODO
    throw std::logic_error{"unimplemented"};
  } else {
    gl::GLuint texObj = image->obj;
    float color[4] = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
    gl::ClearTexImage(texObj, 0, gl::RGBA, gl::FLOAT, color);
  }
}

void OpenGLGraphicsBackend::clearDepthStencil(
    gfx::DepthStencilRenderTargetView view, float clearDepth) {
  Image *image = (Image *)view.image;

  if (image->isRenderbuffer) {
    // TODO
    throw std::logic_error{"unimplemented"};
  } else {
    gl::GLuint texObj = image->obj;
    gl::ClearTexImage(texObj, 0, gl::DEPTH_COMPONENT, gl::FLOAT, &clearDepth);
  }
}

void OpenGLGraphicsBackend::presentToScreen(gfx::ImageHandle img,
                                            unsigned width, unsigned height) {
  Image *image = (Image *)img;

  // make a framebuffer and bind the image to it
  gl::GLuint tmpfb = 0;
  gl::CreateFramebuffers(1, &tmpfb);
  if (image->isRenderbuffer) {
    // TODO
    throw std::logic_error{"unimplemented"};
  } else {
    gl::GLuint texObj = image->obj;
    gl::NamedFramebufferTexture(tmpfb, gl::COLOR_ATTACHMENT0, texObj, 0);
  }

  // TODO disable scissor test

  gl::BlitNamedFramebuffer(tmpfb, 0,
                           0,      // srcX0
                           0,      // srcY0
                           width,  // srcX1,
                           height, // srcY1,
                           0,      // dstX0
                           height, // dstY0
                           width,  // dstX1,
                           0,      // dstY1,
                           gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT,
                           gl::NEAREST);

  // destroy temp framebuffer
  gl::DeleteFramebuffers(1, &tmpfb);

  // the caller should swapBuffers afterwards (we can't do it for them)
}

void OpenGLGraphicsBackend::draw(gfx::GraphicsPipelineHandle pipeline,
                                 gfx::FramebufferHandle framebuffer,
                                 gfx::ArgumentBlockHandle arguments,
                                 gfx::DrawParams drawCommand)
{
  GraphicsPipeline *pipeline_ = (GraphicsPipeline *)pipeline;
  ArgumentBlock *args_ = (ArgumentBlock *)arguments;
  gl::GLuint fbo = (gl::GLuint)framebuffer;

  gl::BindVertexArray(pipeline_->vao);
  gl::UseProgram(pipeline_->program);

  gl::BindVertexBuffers(0, 1, args_->vertexBuffers.data(),
                        args_->vertexBufferOffsets.data(),
                        args_->vertexBufferStrides.data());
  gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);

  gl::DrawArraysInstancedBaseInstance(
      gl::TRIANGLES, drawCommand.firstVertex, drawCommand.vertexCount,
	  drawCommand.instanceCount, drawCommand.firstInstance);
}

} // namespace gfxopengl
