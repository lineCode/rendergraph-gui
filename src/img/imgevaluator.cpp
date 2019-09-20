#include "img/imgevaluator.h"
#include "util/log.h"

namespace img {

using node::Input;
using node::Node;
using node::Output;

ImgEvaluator::ImgEvaluator(gfx::GraphicsBackend &gfx, ImgNetwork &network)
    : network_{network}, gfx_{gfx} {
  network_.lock();
  // toposort nodes in the network
  sortedNodes_ = network_.sortedChildren();
  // debug
  util::log("=== Execution plan: ===");
  for (auto s : sortedNodes_) {
    util::log(" - {}", s->name().to_string());
  }
  // init node data
  nodeData_.resize(sortedNodes_.size());
  // update render target descriptions
  for (int i = 0; i < sortedNodes_.size(); ++i) {
    auto       imgNode = static_cast<ImgNode *>(sortedNodes_[i]);
    ImgContext ctx(*this, *imgNode, nodeData_[i]);
    imgNode->prepare(ctx);
  }
  // debug
  util::log("=== Render targets: ===");
  for (int i = 0; i < sortedNodes_.size(); ++i) {
    auto imgNode = static_cast<ImgNode *>(sortedNodes_[i]);
    for (auto &&rt : nodeData_[i].renderTargets) {
      util::log(" - [{}]{} {}x{}", imgNode->name().to_string(), rt.name,
                rt.desc.width, rt.desc.height);
    }
  }
}

ImgEvaluator::~ImgEvaluator() { network_.unlock(); }

void ImgEvaluator::defaultImageSize(int &width, int &height) const {}

void ImgEvaluator::setDefaultImageSize(int width, int height) {}

gfx::Format ImgEvaluator::defaultImageFormat() const {
	return defaultFormat_;
}

void ImgEvaluator::setDefaultImageFormat(gfx::Format format) {}

void ImgEvaluator::evaluate() {}

void ImgEvaluator::allocateRenderTargets() {}

//-----------------------------------------------------------------------------
ImgContext::ImgContext(ImgEvaluator &evaluator, ImgNode &node,
                       ImgNodeData &nodeData)
    : evaluator_{evaluator}, node_{node}, nodeData_{nodeData} {}

gfx::ImageHandle ImgContext::getInputImage(node::Input *input) {
  Node *  srcNode;
  Output *srcOutput;
  if (node_.inputSource(input, srcNode, srcOutput)) {
    ImgNode *imgNode = static_cast<ImgNode *>(srcNode);
    return imgNode->getOutputImage(*this, imgNode->outputName(srcOutput));
  }
  return 0;
}

//------ Render targets ------

ImgNodeData::RenderTarget *
ImgContext::findRenderTarget(util::StringRef name) const {
  for (auto &&rt : nodeData_.renderTargets) {
    if (rt.name == name) {
      return &rt;
    }
  }
  return nullptr;
}

ImgNodeData::RenderTarget *
ImgContext::findOrCreateRenderTarget(util::StringRef name) {
  if (auto rt = findRenderTarget(name)) {
    return rt;
  }
  ImgNodeData::RenderTarget rt;
  rt.name = name.to_string();
  nodeData_.renderTargets.push_back(rt);
  return &nodeData_.renderTargets.back();
}

void ImgContext::setRenderTargetDesc(util::StringRef       name,
                                     const gfx::ImageDesc &desc) {
  auto rt = findOrCreateRenderTarget(name);
  rt->desc = desc;
  rt->shared = nullptr;
}

const gfx::ImageDesc *
ImgContext::getRenderTargetDesc(util::StringRef renderTarget) const {
  auto rt = findRenderTarget(renderTarget);
  if (!rt)
    return nullptr;
  return &rt->desc;
}

void ImgContext::deleteRenderTarget(util::StringRef renderTarget) 
{

}

gfx::RenderTargetView
ImgContext::getRenderTargetView(util::StringRef renderTarget) {
	auto rt = findRenderTarget(renderTarget);
	if (!rt)
		return gfx::RenderTargetView{0};
	return gfx::RenderTargetView{ rt->shared->image };
}

} // namespace img