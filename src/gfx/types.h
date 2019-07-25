#pragma once
#include <cstdint>

namespace gfx {

typedef uintptr_t ImageHandle;
typedef uintptr_t BufferHandle;
typedef uintptr_t ShaderModuleHandle;
typedef uintptr_t GraphicsPipelineHandle;
typedef uintptr_t SignatureHandle;
typedef uintptr_t ArgumentBlockHandle;
typedef uintptr_t RenderPassHandle;
typedef uintptr_t FramebufferHandle;

struct SamplerDesc;

struct RenderTargetView {
  ImageHandle image;
};

struct DepthStencilRenderTargetView {
  ImageHandle image;
};

struct SampledImageView {
  ImageHandle image;
  const SamplerDesc &sampler;
};

struct ConstantBufferView {
  BufferHandle buffer;
  size_t offset;
  size_t size;
};

struct StorageBufferView {
  BufferHandle buffer;
  size_t offset;
  size_t size;
};

struct VertexBufferView {
  BufferHandle buffer;
  size_t offset;
  size_t size;
};

struct IndexBufferView {
  BufferHandle buffer;
  size_t offset;
  size_t size;
};
} // namespace gfx
