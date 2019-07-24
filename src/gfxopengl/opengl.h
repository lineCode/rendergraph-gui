#pragma once
#include "gfx/gfx.h"
#include "util/hash.h"
#include <cstdint>
#include <vector>

namespace gfxopengl {

struct OpenGLContextInfo {
  int uniformBufferOffsetAlignment;
};

struct SamplerHash {
	constexpr std::size_t operator()(gfx::SamplerDesc const &s) const {
		std::size_t res = 0;
		util::hashCombine(res, s.addrU);
		util::hashCombine(res, s.addrV);
		util::hashCombine(res, s.addrW);
		util::hashCombine(res, s.minFilter);
		util::hashCombine(res, s.magFilter);
		util::hashCombine(res, s.borderColor.r);
		util::hashCombine(res, s.borderColor.g);
		util::hashCombine(res, s.borderColor.b);
		util::hashCombine(res, s.borderColor.a);
		return res;
	}
};

class OpenGLGraphicsBackend : public gfx::GraphicsBackend {
public:
  /// Attempts to create an OpenGL graphics context from the current context.
  /// Throws an error if no context exists on the current thread.
  OpenGLGraphicsBackend();
  ~OpenGLGraphicsBackend();


private:
  struct Private;
  std::unique_ptr<Private> d;

  // Inherited via GraphicsBackend
  virtual gfx::ImageHandle createImage(const gfx::ImageDesc & desc) override;
  virtual void deleteImage(gfx::ImageHandle handle) override;
  virtual void updateImageData(gfx::ImageHandle image, int x, int y, int z, int width, int height, int depth, const void * data) override;
  virtual gfx::ShaderModuleHandle createShaderModule(util::StringRef source, gfx::ShaderStageFlags stage) override;
  virtual void deleteShaderModule(gfx::ShaderModuleHandle handle) override;
  virtual gfx::SignatureHandle createSignature(util::ArrayRef<gfx::SignatureHandle> inheritedSignatures, const gfx::SignatureDesc & description) override;
  virtual void deleteSignature(gfx::SignatureHandle handle) override;
  virtual gfx::ArgumentBlockHandle createArgumentBlock(gfx::SignatureHandle signature) override;
  virtual void deleteArgumentBlock(gfx::ArgumentBlockHandle handle) override;
  virtual void argumentBlockSetArgumentBlock(gfx::ArgumentBlockHandle argBlock, int index, gfx::ArgumentBlockHandle block) override;
  virtual void argumentBlockSetShaderResource(gfx::ArgumentBlockHandle argBlock, int resourceIndex, gfx::SampledImageView imgView) override;
  virtual void argumentBlockSetShaderResource(gfx::ArgumentBlockHandle argBlock, int resourceIndex, gfx::ConstantBufferView buf) override;
  virtual void argumentBlockSetShaderResource(gfx::ArgumentBlockHandle argBlock, int resourceIndex, gfx::StorageBufferView buf) override;
  virtual void argumentBlockSetVertexBuffer(gfx::ArgumentBlockHandle argBlock, int index, gfx::VertexBufferView buf) override;
  virtual void argumentBlockSetIndexBuffer(gfx::ArgumentBlockHandle argBlock, gfx::IndexBufferView buf) override;
  virtual gfx::RenderPassHandle createRenderPass(util::ArrayRef<gfx::RenderPassTargetDesc> colorTargets, const gfx::RenderPassTargetDesc * depthTarget) override;
  virtual void deleteRenderPass(gfx::RenderPassHandle handle) override;
  virtual gfx::GraphicsPipelineHandle createGraphicsPipeline(const gfx::GraphicsPipelineDesc & desc) override;
  virtual void deleteGraphicsPipeline(gfx::GraphicsPipelineHandle handle) override;
  virtual gfx::FramebufferHandle createFramebuffer(util::ArrayRef<gfx::RenderTargetView> colorTargets, gfx::DepthStencilRenderTargetView * depthTarget) override;
  virtual void deleteFramebuffer(gfx::FramebufferHandle handle) override;
  virtual gfx::BufferHandle createConstantBuffer(const void * data, size_t len) override;
  virtual void deleteBuffer(gfx::BufferHandle handle) override;
  virtual void clearRenderTarget(gfx::RenderTargetView view, const gfx::ColorF & clearColor) override;
  virtual void clearDepthStencil(gfx::DepthStencilRenderTargetView view, float clearDepth) override;
  virtual void presentToScreen(gfx::ImageHandle img, unsigned width, unsigned height) override;
};

} // namespace gfxopengl