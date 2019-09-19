#pragma once
#include "img/imgnode.h"

namespace img {

///
/// A node representing a screen space operation.
///
/// `ImgNode`s execute in the "screen space" context.
class ImgShaderNode : public img::ImgNode {
public:
  ImgShaderNode(node::Network &parent, util::StringRef name,
                node::NodeTemplate &tpl);
  //------ Fragment shader ------

  /// Returns the body of the fragment shader.
  util::StringRef fragCode() const { return fragCode_; }
  /// Sets the body of the fragment shader.
  void setFragCode(std::string code);

  bool compilationSucceeded() const { return compilationSuccess_; }

  void execute(gfx::GraphicsBackend &gfx,
               const ScreenSpaceContext &ctx) override;

  static void registerTemplate();

private:
  bool compile(gfx::GraphicsBackend &gfx);

  uint64_t lastFramebufferUpdate_ = 0;
  std::string fragCode_;
  std::string compilationMessages_;
  gfx::Framebuffer framebuffer_;
  gfx::GraphicsPipeline pipeline_;
  gfx::Signature signature_;
  bool compilationSuccess_ = false;
  bool shaderDirty_ = true;
};

} // namespace img