#include "img/screenspacenetwork.h"
#include "img/rendertarget.h"
#include "img/screenspacenode.h"
#include "gfx/gfx.h"

using node::Network;

namespace img {

ScreenSpaceNetwork::ScreenSpaceNetwork(std::string name, Network *parent)
    : Network{name, parent} {}

// allocate render targets
void ScreenSpaceNetwork::allocateRenderTargets(gfx::GraphicsBackend* gfx) {

	auto nodes = findChildrenByType<ScreenSpaceNode>();

	// go through every node
	for (auto n : nodes) {
		// go through every render target


	}
}

} // namespace img