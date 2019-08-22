#pragma once
// Main header for the backend-agnostic graphics API
#include "gfx/format.h"
#include "gfx/handle.h"
#include "gfx/sampler.h"
#include "gfx/shader.h"
#include "gfx/types.h"
#include "util/arrayref.h"
#include "util/stringref.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace gfx {

struct ImageDesc;
struct SignatureDesc;
struct GraphicsPipelineDesc;

struct RenderPassTargetDesc {
  ColorF clearValue;
};

struct RenderPassDesc {
  util::ArrayRef<const RenderPassTargetDesc> colorTargets;
  const RenderPassTargetDesc *depthTarget;
};

struct FramebufferDesc {
  util::ArrayRef<const RenderTargetView> colorTargets;
  DepthStencilRenderTargetView *depthTarget;
};

/// Parameters for non-indexed draw commands.
struct DrawParams {
  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t firstVertex;
  uint32_t firstInstance;
};

/// Compilation log of a shader
struct ShaderCompilationMessages {
  std::string messages;
};

/// Compilation log of a shader
struct GraphicsPipelineCompilationMessages {
  std::string messages;
};

class ShaderCompilationError : public std::exception {
public:
  ShaderCompilationError() = default;
  ShaderCompilationError(const char *message) : std::exception{message} {}
};

class GraphicsPipelineCompilationError : public std::exception {
public:
  GraphicsPipelineCompilationError() = default;
  GraphicsPipelineCompilationError(const char *message)
      : std::exception{message} {}
};

class GraphicsBackend {
public:
  virtual ~GraphicsBackend() {}

  /// Creates a new image.
  virtual ImageHandle createImage(const ImageDesc &desc) = 0;
  virtual void deleteImage(ImageHandle handle) = 0;

  /// Uploads new image data to the specified image, synchronously.
  virtual void updateImageData(ImageHandle image, int x, int y, int z,
                               int width, int height, int depth,
                               const void *data) = 0;

  /// Creates a new shader module from the specified source code. The source
  /// language is backend-specific.
  virtual ShaderModuleHandle createShaderModule(util::StringRef source,
                                                ShaderStageFlags stage) = 0;
  virtual void deleteShaderModule(ShaderModuleHandle handle) = 0;

  /// Creates a signature describing the parameters of a graphics or compute
  /// pipeline.
  virtual SignatureHandle
  createSignature(util::ArrayRef<SignatureHandle> inheritedSignatures,
                  const SignatureDesc &description) = 0;
  virtual void deleteSignature(SignatureHandle handle) = 0;

  /// Creates a new argument block whose format is defined by the specified
  /// signature.
  virtual ArgumentBlockHandle
  createArgumentBlock(SignatureHandle signature) = 0;
  virtual void deleteArgumentBlock(ArgumentBlockHandle handle) = 0;

  virtual void argumentBlockSetArgumentBlock(ArgumentBlockHandle argBlock,
                                             int index,
                                             ArgumentBlockHandle block) = 0;
  virtual void argumentBlockSetShaderResource(ArgumentBlockHandle argBlock,
                                              int resourceIndex,
                                              SampledImageView imgView) = 0;
  virtual void argumentBlockSetShaderResource(ArgumentBlockHandle argBlock,
                                              int resourceIndex,
                                              ConstantBufferView buf) = 0;
  virtual void argumentBlockSetShaderResource(ArgumentBlockHandle argBlock,
                                              int resourceIndex,
                                              StorageBufferView buf) = 0;
  virtual void argumentBlockSetVertexBuffer(ArgumentBlockHandle argBlock,
                                            int index,
                                            VertexBufferView buf) = 0;
  virtual void argumentBlockSetIndexBuffer(ArgumentBlockHandle argBlock,
                                           IndexBufferView buf) = 0;

  /// Creates a new render pass.
  virtual RenderPassHandle createRenderPass(const RenderPassDesc &desc) = 0;
  virtual void deleteRenderPass(RenderPassHandle handle) = 0;

  /// Creates a new graphics pipeline.
  virtual GraphicsPipelineHandle
  createGraphicsPipeline(const GraphicsPipelineDesc &desc) = 0;
  virtual void deleteGraphicsPipeline(GraphicsPipelineHandle handle) = 0;

  /// Creates a new framebuffer for the given render pass.
  virtual FramebufferHandle createFramebuffer(const FramebufferDesc &desc) = 0;
  virtual void deleteFramebuffer(FramebufferHandle handle) = 0;

  /// Upload some constant data to a GPU buffer
  virtual BufferHandle createConstantBuffer(const void *data, size_t len) = 0;
  virtual void deleteBuffer(BufferHandle handle) = 0;

  // Commands
  virtual void clearRenderTarget(RenderTargetView view,
                                 const ColorF &clearColor) = 0;

  virtual void clearDepthStencil(DepthStencilRenderTargetView view,
                                 float clearDepth) = 0;

  virtual void presentToScreen(ImageHandle img, unsigned width,
                               unsigned height) = 0;

  virtual void draw(GraphicsPipelineHandle pipeline,
                    FramebufferHandle framebuffer,
                    ArgumentBlockHandle arguments, DrawParams drawCommand) = 0;

private:
};

////////////////////////////////////////////////////////////////////////////////
struct ImageDeleter {
  void operator()(GraphicsBackend &backend, ImageHandle handle) {
    backend.deleteImage(handle);
  }
};

class Image {
public:
  Image() = default;
  Image(GraphicsBackend &backend, const ImageDesc &desc)
      : image{backend, backend.createImage(desc)} {}

  RenderTargetView asRenderTargetView() {
    return RenderTargetView{image.get()};
  }

  operator ImageHandle() { return image.get(); }

private:
  Handle<ImageHandle, ImageDeleter> image;
};

////////////////////////////////////////////////////////////////////////////////
struct ShaderModuleDeleter {
  void operator()(GraphicsBackend &backend, ShaderModuleHandle handle) {
    backend.deleteShaderModule(handle);
  }
};

class ShaderModule {
public:
  ShaderModule() = default;
  ShaderModule(GraphicsBackend &backend, util::StringRef source,
               gfx::ShaderStageFlags stage)
      : shader{backend, backend.createShaderModule(source, stage)} {}

  operator ShaderModuleHandle() { return shader.get(); }

private:
  Handle<ShaderModuleHandle, ShaderModuleDeleter> shader;
};

////////////////////////////////////////////////////////////////////////////////
struct GraphicsPipelineDeleter {
  void operator()(GraphicsBackend &backend, GraphicsPipelineHandle handle) {
    backend.deleteGraphicsPipeline(handle);
  }
};

class GraphicsPipeline {
public:
  GraphicsPipeline() = default;
  GraphicsPipeline(GraphicsBackend &backend, gfx::GraphicsPipelineDesc &desc)
      : pipeline{backend, backend.createGraphicsPipeline(desc)} {}

  operator GraphicsPipelineHandle() { return pipeline.get(); }

private:
  Handle<GraphicsPipelineHandle, GraphicsPipelineDeleter> pipeline;
};

////////////////////////////////////////////////////////////////////////////////
struct FramebufferDeleter {
  void operator()(GraphicsBackend &backend, FramebufferHandle handle) {
    backend.deleteFramebuffer(handle);
  }
};

class Framebuffer {
public:
  Framebuffer() = default;
  Framebuffer(GraphicsBackend &backend, const FramebufferDesc &desc)
      : framebuffer{backend, backend.createFramebuffer(desc)} {}

  operator FramebufferHandle() { return framebuffer.get(); }

private:
  Handle<FramebufferHandle, FramebufferDeleter> framebuffer;
};

////////////////////////////////////////////////////////////////////////////////
struct RenderPassDeleter {
  void operator()(GraphicsBackend &backend, RenderPassHandle handle) {
    backend.deleteRenderPass(handle);
  }
};

class RenderPass {
public:
  RenderPass() = default;
  RenderPass(GraphicsBackend &backend, const RenderPassDesc &desc)
      : renderPass{backend, backend.createRenderPass(desc)} {}

  operator RenderPassHandle() { return renderPass.get(); }

private:
  Handle<RenderPassHandle, RenderPassDeleter> renderPass;
};

////////////////////////////////////////////////////////////////////////////////
struct SignatureDeleter {
  void operator()(GraphicsBackend &backend, SignatureHandle handle) {
    backend.deleteSignature(handle);
  }
};

class Signature {
public:
  Signature() = default;
  Signature(GraphicsBackend &backend, const SignatureDesc &desc)
      : signature{backend, backend.createSignature(nullptr, desc)} {}

  operator SignatureHandle() { return signature.get(); }

private:
  Handle<SignatureHandle, SignatureDeleter> signature;
};

////////////////////////////////////////////////////////////////////////////////
struct BufferDeleter {
  void operator()(GraphicsBackend &backend, BufferHandle handle) {
    backend.deleteBuffer(handle);
  }
};

class Buffer {
public:
  Buffer() = default;
  Buffer(GraphicsBackend &backend, const void *data, size_t size)
      : buffer{backend, backend.createConstantBuffer(data, size)} {}

  operator BufferHandle() { return buffer.get(); }

private:
  Handle<BufferHandle, BufferDeleter> buffer;
};

////////////////////////////////////////////////////////////////////////////////
struct ArgumentBlockDeleter {
  void operator()(GraphicsBackend &backend, ArgumentBlockHandle handle) {
    backend.deleteArgumentBlock(handle);
  }
};

class ArgumentBlock {
public:
  ArgumentBlock() = default;
  ArgumentBlock(GraphicsBackend &backend, gfx::SignatureHandle signature)
      : argblock{backend, backend.createArgumentBlock(signature)} {}

  operator ArgumentBlockHandle() { return argblock.get(); }

  void setArgumentBlock(int index, ArgumentBlockHandle block) {
    argblock.backend().argumentBlockSetArgumentBlock(argblock.get(), index,
                                                     block);
  }
  void setShaderResource(int resourceIndex, SampledImageView imgView) {
    argblock.backend().argumentBlockSetShaderResource(argblock.get(),
                                                      resourceIndex, imgView);
  }
  void setShaderResource(int resourceIndex, ConstantBufferView buf) {
    argblock.backend().argumentBlockSetShaderResource(argblock.get(),
                                                      resourceIndex, buf);
  }
  void setShaderResource(int resourceIndex, StorageBufferView buf) {
    argblock.backend().argumentBlockSetShaderResource(argblock.get(),
                                                      resourceIndex, buf);
  }
  void setVertexBuffer(int index, VertexBufferView buf) {
    argblock.backend().argumentBlockSetVertexBuffer(argblock.get(), index, buf);
  }
  void setIndexBuffer(ArgumentBlockHandle argBlock, IndexBufferView buf) {
    argblock.backend().argumentBlockSetIndexBuffer(argblock.get(), buf);
  }

private:
  Handle<ArgumentBlockHandle, ArgumentBlockDeleter> argblock;
};

} // namespace gfx
