#pragma once
#include "node/network.h"

namespace img {
class ScreenSpaceNetwork : public node::Network {
public:
	ScreenSpaceNetwork(std::string name, Network *parent);

private:
  void allocateRenderTargets();
};
} // namespace img