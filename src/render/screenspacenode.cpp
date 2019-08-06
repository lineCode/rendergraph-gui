#include "fmt/format.h"
#include "gfx/pipeline.h"
#include "gfx/signature.h"
#include "render/node.h"
#include "render/image.h"

namespace render {

static const char VERT_SRC_TEMPLATE[] = R"(
#version 450
layout(location=0) in vec2 a_position;
layout(location=1) in vec2 texcoord;
layout(location=0) out vec2 uv;
void main() {
    uv = texcoord;
    gl_Position = vec4(a_position, 0.0, 1.0);
	// <<<< insert additional interpolated attribs here >>>>
}
)";

static const char FRAG_SRC_TEMPLATE[] = R"(
#version 450
layout(location=0) in vec2 a_position;
layout(location=1) in vec2 texcoord;
layout(location=0) out vec2 uv;



void main() {
	// <<<< fetch all parameters >>>>
	// <<<< paste snippet here >>>> 
}
)";

static const gfx::VertexLayoutElement POSITION_LAYOUT_ELEMENTS[] = {
    {{"POSITION", 0}, gfx::Format::R32G32_SFLOAT, 0},
    {{"TEXCOORD", 1}, gfx::Format::R32G32_SFLOAT, 8}};

static const gfx::VertexLayout POSITION_LAYOUT = {
    util::makeArrayRef(POSITION_LAYOUT_ELEMENTS), 16};

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
};

///
/// A node representing a screen space operation.
///
/// `ScreenSpaceNode`s execute in the "screen space" context.
class ScreenSpaceNode : public Node {
public:
  /// Returns the body of the fragment shader.
  util::StringRef fragCode() const;
  /// Sets the body of the fragment shader.
  void setFragCode(std::string code);

  ///
  bool compilationSucceeded() const { return compilationSuccess_; }

  void execute(gfx::GraphicsBackend *gfx, const ScreenSpaceContext& ctx);

private:
  bool compile(gfx::GraphicsBackend *gfx);

  // Fragment code to execute
  std::string fragCode_;
  // Generated shader pipeline. Can be null if not generated yet.
  gfx::GraphicsPipeline pipeline_;
  // Messages emitted by the last compilation step.
  std::string compilationMessages_;
  // True if last compile succeeded.
  bool compilationSuccess_ = false;
  // True if shaders should be recompiled.
  bool shaderDirty_ = true;
};

util::StringRef ScreenSpaceNode::fragCode() const {}

void ScreenSpaceNode::setFragCode(std::string code) {
  fragCode_ = std::move(code);
  shaderDirty_ = true;
  compilationSuccess_ = false;
}

void ScreenSpaceNode::execute(gfx::GraphicsBackend *gfx, const ScreenSpaceContext& ctx) {
	// allocate output image from the cache


	// handling of output i
}

bool ScreenSpaceNode::compile(gfx::GraphicsBackend *gfx) {

  if (shaderDirty_ == false) {
    // no need to recompile the pipeline.
    return compilationSuccess_;
  }

  // create the signature
  gfx::SignatureDesc sigDesc;
  const gfx::ResourceBinding resources[2] = {
      gfx::ResourceBinding::makeConstantBuffer(0),
      gfx::ResourceBinding::makeConstantBuffer(1),
  };
  const gfx::FragmentOutputDescription fragOut[1] = {};
  const gfx::VertexInputBinding vtxIn[1] = {POSITION_LAYOUT,
                                            gfx::VertexInputRate::Vertex, 0};
  sigDesc.fragmentOutputs = util::makeArrayRef(fragOut);
  sigDesc.shaderResources = util::makeArrayRef(resources);
  sigDesc.vertexInputs = util::makeArrayRef(vtxIn);
  sigDesc.hasdepthStencilFragmentOutput = false;
  sigDesc.hasIndexFormat = false;
  sigDesc.viewportsCount = 1;
  sigDesc.scissorsCount = 1;
  gfx::Signature sig{gfx, sigDesc};

  // render pass
  const gfx::RenderPassTargetDesc targets[1] = {};
  gfx::RenderPassDesc rpDesc {util::makeArrayRef(targets), nullptr};
  gfx::RenderPass rp{gfx, rpDesc};

  try {
    // shaders
    gfx::ShaderModule vertexShader{gfx, VERT_SRC_TEMPLATE,
                                   gfx::ShaderStageFlags::VERTEX};
    gfx::ShaderModule fragmentShader{gfx, FRAG_SRC_TEMPLATE,
                                     gfx::ShaderStageFlags::FRAGMENT};

    // pipeline
    gfx::GraphicsPipelineDesc desc;
    desc.shaderStages.vertex = vertexShader;
    desc.shaderStages.fragment = fragmentShader;
    desc.signature = sig;
    desc.renderPass = rp;
    pipeline_ = gfx::GraphicsPipeline{gfx, desc};
  } catch (gfx::ShaderCompilationError e) {
    // failed to compile one of the shaders, store log and bail out.
    fmt::format("Shader compilation messages: \n {}", e.what());
    compilationSuccess_ = false;
    // reset the dirty flag: we don't want to keep recompiling
    // the shader over and over if the source has errors.
    shaderDirty_ = false;
    return false;
  } catch (gfx::GraphicsPipelineCompilationError e) {
    // failed to create the pipeline, store log and bail out.
    fmt::format("Pipeline compilation messages: \n {}", e.what());
    compilationSuccess_ = false;
    shaderDirty_ = false;
    return false;
  }

  return true;
}

} // namespace render