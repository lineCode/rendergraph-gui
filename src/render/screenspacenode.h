#pragma once
#include "gfx/image.h"
#include "gfx/pipeline.h"
#include "render/network.h"
#include "render/input.h"
#include "render/output.h"
#include "render/rendertarget.h"
#include <memory>

namespace render {

enum class ImageOutputScale {
  /// output size is size defined in the current stylization project
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

struct ScreenSpaceContext {
  /// Width of the viewport in pixels
  int width;
  /// Height of the viewport in pixels
  int height;
  /// Current time
  double currentTime;
  /// Current frame
  int currentFrame;

  /// Constant buffer containing the common parameters (camera, matrices, etc.)
  gfx::ConstantBufferView commonParameters;
  gfx::VertexBufferView quadVertices;
};


///
/// A node representing a screen space operation.
///
/// `ScreenSpaceNode`s execute in the "screen space" context.
class ScreenSpaceNode : public Node {
public:
	ScreenSpaceNode(Network* parent, std::string name);
  //------ Fragment shader ------

  /// Returns the body of the fragment shader.
  util::StringRef fragCode() const;
  /// Sets the body of the fragment shader.
  void setFragCode(std::string code);

  //------ Inputs ------
  Input* addInput(std::string name, std::string initPath = "");
  void deleteInput(Input* input);
  Output* addOutput(std::string name);
  void deleteOutput(Output* input);

  //------ Parameters ------
  Param* addParam(std::string name, std::string description, double initValue);
  void deleteParam(Param* param);

  //------ Render target management ------
  RenderTarget *addRenderTarget(std::string name, const gfx::ImageDesc &desc);
  void setRenderTargetDesc(util::StringRef name, const gfx::ImageDesc &desc);
  void deleteRenderTarget(util::StringRef name);

  bool compilationSucceeded() const { return compilationSuccess_; }
  void execute(gfx::GraphicsBackend *gfx, const ScreenSpaceContext &ctx);

  static ScreenSpaceNode* make(Network* parent, std::string name);

private:
  bool compile(gfx::GraphicsBackend *gfx);

  std::vector<Input::Ptr> inputs_;
  std::vector<Output::Ptr> outputs_;
  std::vector<Param::Ptr> params_;

  std::vector<RenderTarget::Ptr> renderTargets_;
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