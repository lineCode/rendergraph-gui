#include "render/screenspacenode.h"
#include "fmt/format.h"
#include "gfx/pipeline.h"
#include "gfx/signature.h"
#include "render/constantbufferbuilder.h"
#include "render/image.h"
#include "render/node.h"
#include <map>

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

util::StringRef ScreenSpaceNode::fragCode() const {
  return util::StringRef{fragCode_.c_str(), fragCode_.size()};
}

void ScreenSpaceNode::setFragCode(std::string code) {
  fragCode_ = std::move(code);
  shaderDirty_ = true;
  compilationSuccess_ = false;
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
  signature_ = gfx::Signature{gfx, sigDesc};

  // render pass
  const gfx::RenderPassTargetDesc targets[1] = {};
  gfx::RenderPassDesc rpDesc{util::makeArrayRef(targets), nullptr};
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
    desc.signature = signature_;
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

void ScreenSpaceNode::execute(gfx::GraphicsBackend *gfx,
                              const ScreenSpaceContext &ctx) {
  // build the constant (uniform) buffer
  ConstantBufferBuilder b;
  // TODO push all parameters in the buffer
  b.push(0.0f);
  b.push(0.2f);
  b.push(0.4f);
  auto constantBuffer = b.create(0);
  auto constantBufferView =
      gfx::ConstantBufferView{constantBuffer, 0, b.size()};

  // create the argblock
  gfx::ArgumentBlock args{gfx, signature_};
  args.setShaderResource(0, ctx.commonParameters);
  args.setShaderResource(1, constantBufferView);
  args.setVertexBuffer(0, ctx.quadVertices);
  // TODO textures

  // check if the framebuffer needs updating
  if (!framebuffer_) {
    gfx::FramebufferDesc fbDesc;
    gfx::RenderTargetView rtv;
    rtv.image = outputs_[0]->getImage(ctx);
    gfx::RenderTargetView rtvs[1] = {rtv};
    fbDesc.colorTargets = util::makeConstArrayRef(rtvs);
    framebuffer_ = gfx::Framebuffer{gfx, fbDesc};
  }

  // draw stuff
  gfx::DrawParams params;
  params.firstVertex = 0;
  params.vertexCount = 6;
  params.firstInstance = 0;
  params.instanceCount = 1;
  gfx->draw(pipeline_, framebuffer_, args, params);

  // mark our outputs as dirty so that other passes that depend on them are
  // updated.
  // renderTargets_[0]->markDirty();
}

// Container::value_type must be unique_ptr<T> or whatever
template <typename Container, typename T>
void eraseRemoveUniquePtr(Container &container, const T *val) {
  static_assert(
      std::is_same<typename Container::value_type, std::unique_ptr<T>>::value,
      "Container value type must be unique_ptr<T>");
  auto it = std::remove_if(container.begin(), container.end(),
                           [val](const std::unique_ptr<T> &ptr) {
                             if (ptr.get() == val) {
                               return true;
                             }
                             return false;
                           });
  container.erase(it, container.end());
}

template <typename Container, typename T>
T *pushUniquePtr(Container &container, std::unique_ptr<T> ptr) {
  static_assert(
      std::is_same<typename Container::value_type, std::unique_ptr<T>>::value,
      "Container value type must be unique_ptr<T>");
  auto p = ptr.get();
  container.push_back(std::move(ptr));
  return p;
}

RenderTargetOutput *ScreenSpaceNode::addOutput(std::string name,
                                               const gfx::ImageDesc &desc) {
  return pushUniquePtr(outputs_, RenderTargetOutput::make(this, name));
}

/*void ScreenSpaceNode::setOutputDesc(util::StringRef name, const gfx::ImageDesc
&desc) {

}*/

void ScreenSpaceNode::deleteOutput(RenderTargetOutput *output) {
  eraseRemoveUniquePtr(outputs_, output);
}

Input *ScreenSpaceNode::addInput(std::string name, std::string initPath) {
  return pushUniquePtr(inputs_, Input::make(this, name));
}

void ScreenSpaceNode::deleteInput(Input *input) {
  eraseRemoveUniquePtr(inputs_, input);
}

Param *ScreenSpaceNode::addParam(std::string name, std::string description,
                                 double initValue) {
  return pushUniquePtr(params_, Param::make(this, std::move(name),
                                            std::move(description), initValue));
}

void ScreenSpaceNode::deleteParam(Param *param) {
  eraseRemoveUniquePtr(params_, param);
}

ScreenSpaceNode::ScreenSpaceNode(Network *parent, std::string name)
    : Node{std::move(name), parent} {}

ScreenSpaceNode *ScreenSpaceNode::make(Network *parent, std::string name) {
  return static_cast<ScreenSpaceNode *>(parent->addChild(
      std::make_unique<ScreenSpaceNode>(parent, std::move(name))));
}

} // namespace render
