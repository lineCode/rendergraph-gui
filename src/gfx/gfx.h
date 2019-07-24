#pragma once
// Main header for the backend-agnostic graphics API

// header organization:
// one header per interface
// put large structs in separate headers and forward declare
// put small enums & structs passed by value in a common header

#include "gfx/format.h"
#include "gfx/sampler.h"
#include "gfx/shader.h"
#include "gfx/types.h"
#include "util/arrayref.h"
#include "util/stringref.h"
#include <cstdint>
#include <memory>

// image -> image.h
// pipeline -> pipeline.h
// backend -> gfx.h
// RenderTargetView -> types.h (via gfx.h)

namespace gfx {

struct ImageDesc;
struct SignatureDesc;
struct GraphicsPipelineDesc;

struct RenderPassTargetDesc {
  ColorF clearValue;
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
  virtual RenderPassHandle
  createRenderPass(util::ArrayRef<RenderPassTargetDesc> colorTargets,
                   const RenderPassTargetDesc *depthTarget) = 0;
  virtual void deleteRenderPass(RenderPassHandle handle) = 0;

  /// Creates a new graphics pipeline.
  virtual GraphicsPipelineHandle
  createGraphicsPipeline(const GraphicsPipelineDesc &desc) = 0;
  virtual void deleteGraphicsPipeline(GraphicsPipelineHandle handle) = 0;

  /// Creates a new framebuffer for the given render pass.
  virtual FramebufferHandle
  createFramebuffer(util::ArrayRef<RenderTargetView> colorTargets,
                    DepthStencilRenderTargetView *depthTarget) = 0;
  virtual void deleteFramebuffer(FramebufferHandle handle) = 0;

  /// Upload some constant data to a GPU buffer
  virtual BufferHandle createConstantBuffer(const void *data, size_t len) = 0;
  virtual void deleteBuffer(BufferHandle handle) = 0;

  // Commands
  virtual void clearRenderTarget(RenderTargetView view,
                                 const ColorF &clearColor) = 0;

  virtual void clearDepthStencil(DepthStencilRenderTargetView view,
                                 float clearDepth) = 0;

  virtual void presentToScreen(ImageHandle img, unsigned width, unsigned height) = 0;

private:
};

//
class GraphicsApi {
public:
  GraphicsApi();
  ~GraphicsApi();

private:
};

} // namespace gfx
