#pragma once
// Main header for the backend-agnostic graphics API

// header organization:
// one header per interface
// put large structs in separate headers and forward declare
// put small enums & structs passed by value in a common header

#include "gfx/shader.h"
#include "gfx/format.h"
#include "gfx/sampler.h"
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

class Buffer;
class GraphicsPipeline;
class ArgumentBlock;
class Arena {};
class RenderPass {};
class Framebuffer {};
class Signature;

struct ImageDesc;
struct SignatureDesc;
struct GraphicsPipelineDesc;


struct RenderPassTargetDesc {
	ColorF clearValue;
};

class GraphicsBackend {
public:
  virtual ~GraphicsBackend() {}

  /// Creates a new arena that owns graphics objects.
  virtual std::unique_ptr<Arena> createArena() = 0;

  /// Creates a new image.
  virtual Image *createImage(Arena &arena, const ImageDesc& desc) = 0;

  /// Uploads new image data to the specified image, synchronously.
  virtual void updateImageData(Image *image, int x, int y, int z, int width,
                               int height, int depth, const void *data) = 0;

  /// Creates a new shader module from the specified source code. The source language is
  /// backend-specific.
  virtual const ShaderModule *createShaderModule(Arena &arena,
                                                 util::StringRef source,
                                                 ShaderStageFlags stage) = 0;

  /// Creates a signature describing the parameters of a graphics or compute pipeline.
  virtual const Signature *
  createSignature(Arena &arena,
                  util::ArrayRef<const Signature *> inheritedSignatures,
                  const SignatureDesc &description) = 0;

  /// Creates a new argument block whose format is defined by the specified signature.
  virtual ArgumentBlock *createArgumentBlock(Arena &arena,
                                             const Signature *signature) = 0;

  /// Creates a new render pass.
  virtual RenderPass* createRenderPass(Arena& arena,
	  util::ArrayRef<RenderPassTargetDesc> colorTargets,
	  const RenderPassTargetDesc* depthTarget) = 0;

  /// Creates a new graphics pipeline.
  virtual GraphicsPipeline* createGraphicsPipeline(Arena& arena, const GraphicsPipelineDesc& desc) = 0;

  /// Creates a new framebuffer for the given render pass.
  virtual Framebuffer* createFramebuffer(Arena& arena,
	  util::ArrayRef<RenderTargetView> colorTargets,
	  DepthStencilRenderTargetView* depthTarget) = 0;

  // Commands


  virtual void clearRenderTarget(RenderTargetView view,
	  const ColorF &clearColor) = 0;
  
  virtual void clearDepthStencil(DepthStencilRenderTargetView view,
	  float clearDepth) = 0;
  
  virtual void presentToScreen(Image* img) = 0;


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
