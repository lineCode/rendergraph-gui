#pragma once
#include "gfx/image.h"
#include "gfx/pipeline.h"
#include "node/node.h"
#include "node/network.h"
#include "img/screenspacenetwork.h"
#include "img/rendertarget.h"
#include <memory>
#include <string>

namespace img {

enum class ImageOutputScale {
  /// output size is size defined in the current project
  ProjectSize,
  /// output size is a fraction of the size defined in the current stylization
  /// project
  FractionOfProjectSize,
  /// output size is the size of the first input
  SizeOfFirstInput,
  /// output size is a fraction of the first input
  FractionOfFirstInput,
  /// custom output size
  CustomSize,
  /// output size is input size rescaled to the given aspect ratio
  CustomAspectRatio,
};

///
/// A node representing a screen space operation.
///
/// `ImgNode`s execute in the "screen space" context.
class ImgNode : public node::Node {
public:
  ImgNode(ImgNetwork &parent, std::string name);
  //------ Fragment shader ------

  /// Returns the body of the fragment shader.
  util::StringRef fragCode() const {
	  return fragCode_;
  }
  /// Sets the body of the fragment shader.
  void setFragCode(std::string code);

  //------ Render target management ------
  RenderTarget *createRenderTarget(const gfx::ImageDesc &desc);
  void setRenderTargetDesc(const gfx::ImageDesc &desc);
  void deleteRenderTarget(RenderTarget *output);

  //------ Output ------
  /// Assign a render target to the specified output
  void assignOutputTarget(util::StringRef output, RenderTarget* target);
  RenderTarget* getOutputTarget(util::StringRef output);
  gfx::ImageHandle getOutputImage(util::StringRef output);

  bool compilationSucceeded() const { return compilationSuccess_; }
  void execute(gfx::GraphicsBackend *gfx, const ScreenSpaceContext &ctx);

  static ImgNode *make(ImgNetwork &parent, std::string name);

private:

  bool compile(gfx::GraphicsBackend *gfx);

  struct OutputTarget {
	  node::Output* output;
	  RenderTarget* target;
  };

  ImgNetwork& parent_;
  std::vector<RenderTarget*> renderTargets_;
  std::vector<OutputTarget> outputMap_;

  uint64_t lastFramebufferUpdate_ = 0;
  std::string fragCode_;
  std::string compilationMessages_;
  gfx::Framebuffer framebuffer_;
  gfx::GraphicsPipeline pipeline_;
  gfx::Signature signature_;
  bool compilationSuccess_ = false;
  bool shaderDirty_ = true;
};

} // namespace render
