#include "img/imgnode.h"
#include "img/constantbufferbuilder.h"
#include "img/imgnetwork.h"
#include "img/imgevaluator.h"
#include "node/node.h"
#include "util/log.h"
#include <map>
#include <regex>

using namespace node;

namespace img {

ImgNode::ImgNode(node::Network &parent, util::StringRef name)
    : Node{&parent, std::move(name)}, parent_{
                                          static_cast<ImgNetwork &>(parent)} {}

gfx::ImageHandle ImgNode::getOutputImage(ImgContext& ctx, util::StringRef outputName) {
	return ctx.getRenderTargetView(outputName).image;
}

} // namespace img
