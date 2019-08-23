#include "img/imgnode.h"
#include "img/constantbufferbuilder.h"
#include "img/image.h"
#include "img/imgnetwork.h"
#include "node/node.h"
#include "util/log.h"
#include <map>
#include <regex>

using namespace node;

namespace img {

RenderTarget *ImgNode::createRenderTarget(const gfx::ImageDesc &desc) {
  return renderTargetCache().createRenderTarget(desc);
}

void ImgNode::setRenderTargetDesc(RenderTarget *renderTarget,
                                  const gfx::ImageDesc &desc) {
	renderTargetCache().setRenderTargetDesc(renderTarget, desc);
  // invalidate all outputs that use this render target
  for (auto &&out : outputMap_) {
    util::log("ImgNode[{}]::setRenderTargetDesc: output `{}` invalidated",
              name().to_string(), outputName(out.output).to_string());
    if (out.renderTarget == renderTarget) {
      out.desc = desc;
      out.image = 0;
    }
  }
}

/// Returns the image description of a registered render target.
const gfx::ImageDesc &ImgNode::getRenderTargetDesc(RenderTarget *renderTarget) {
  return renderTargetCache().getRenderTargetDesc(renderTarget);
}

/// Returns the image allocated for a registered render target.
gfx::ImageHandle ImgNode::getRenderTargetImage(RenderTarget *renderTarget) {
  return renderTargetCache().getImage(renderTarget);
}

void ImgNode::deleteRenderTarget(RenderTarget *target) {
	renderTargetCache().deleteRenderTarget(target);
}

void ImgNode::assignOutputImage(Output *out, const gfx::ImageDesc &desc,
                                gfx::ImageHandle handle) {
  assignOutputImagePrivate(out, nullptr, desc, handle);
}

void ImgNode::assignOutputImage(Output *out, RenderTarget *renderTarget) {
  assignOutputImagePrivate(out, renderTarget, getRenderTargetDesc(renderTarget),
                           0);
}

void ImgNode::assignOutputImagePrivate(Output *out, RenderTarget *renderTarget,
                                       const gfx::ImageDesc &desc,
                                       gfx::ImageHandle handle) {

  for (auto &&entry : outputMap_) {
    if (entry.output == out) {
      // reassign output
      entry.renderTarget = renderTarget;
      entry.desc = desc;
      entry.image = handle;
      return;
    }
  }
  // output not assigned
  OutputTarget entry;
  entry.output = out;
  entry.renderTarget = renderTarget;
  entry.desc = desc;
  entry.image = handle;
  outputMap_.push_back(entry);
  // TODO send event
}

ImgNode::ImgNode(node::Network &parent, std::string name,
                 node::Blueprint &blueprint)
    : Node{ &parent, std::move(name), &blueprint}, parent_{
                                                     static_cast<ImgNetwork &>(
                                                         parent)} {}

RenderTargetCache &ImgNode::renderTargetCache() {
	// TODO
	return *(RenderTargetCache*)nullptr;
}

} // namespace img
