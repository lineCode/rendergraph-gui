#pragma once
// Main header for the backend-agnostic graphics API

#include "gfx/image.h"
#include "gfx/imageformat.h"
#include "gfx/sampler.h"
#include "gfx/signature.h"
#include "util/arrayref.h"
#include "util/stringref.h"
#include <cstdint>
#include <memory>

namespace gfx {

// Forward declarations of backend types
class Buffer {};
class GraphicsPipeline;
class Signature;
class ShaderModule;
class ArgumentBlock;
class Arena {};
class CommandBuffer;

struct RenderTargetView {
  const Image *image;
};

struct DepthStencilRenderTargetView {
  const Image *image;
};

struct SampledImageView {
  const Image *image;
  SamplerDesc sampler;
};

enum class ShaderStageFlags {};

class ArgumentBlock {
public:
  virtual ~ArgumentBlock() = 0;

  virtual void setArgumentBlock(int index, const ArgumentBlock *block) = 0;
  virtual void setRenderTarget(int index, RenderTargetView rtView) = 0;
  virtual void setDepthStencilTarget(int index,
                                     DepthStencilRenderTargetView dstView) = 0;
  virtual void setSampledImage(int descriptor, SampledImageView imgView) = 0;
};

class GraphicsBackend {
public:
  virtual ~GraphicsBackend() {}

  virtual std::unique_ptr<Arena> createArena() = 0;
  virtual Image *createImage(Arena &arena, ImageDimensions dimensions,
                             ImageFormat format, int width, int height,
                             int depth, int arrayLength,
                             ImageUsageFlags flags) = 0;
  virtual void updateImageData(Image &image, int x, int y, int z, int width,
                               int height, int depth, const void *data) = 0;

  virtual const ShaderModule *createShaderModule(Arena &arena,
                                                 util::StringRef source,
                                                 ShaderStageFlags stage) = 0;
  virtual const Signature *
  createSignature(Arena &arena,
                  util::ArrayRef<const Signature *> inheritedSignatures,
                  const SignatureDesc &description) = 0;

  virtual ArgumentBlock *createArgumentBlock(Arena &arena,
                                             const Signature *signature) = 0;

  virtual CommandBuffer *createCommandBuffer(Arena &arena) = 0;

  // Commands
  virtual void clearRenderTarget(RenderTargetView view,
                                 const ColorF &clearColor) = 0;
  virtual void clearDepthStencil(DepthStencilRenderTargetView view,
                                 float clearDepth) = 0;

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
