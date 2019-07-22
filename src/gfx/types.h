#pragma once

namespace gfx {
class Buffer;
class Image;
struct SamplerDesc;

struct RenderTargetView {
  Image *image;
};

struct DepthStencilRenderTargetView {
  Image *image;
};

struct SampledImageView {
  Image *image;
  const SamplerDesc &sampler;
};

struct ConstantBufferView {
  Buffer *buffer;
  size_t offset;
  size_t size;
};

struct StorageBufferView {
  Buffer *buffer;
  size_t offset;
  size_t size;
};

struct VertexBufferView {
  Buffer *buffer;
  size_t offset;
  size_t size;
};

struct IndexBufferView {
  Buffer *buffer;
  size_t offset;
  size_t size;
};
} // namespace gfx