#pragma once
#include "img/imgnode.h"

namespace img {

// ImgClear
// - 1 output:
//		- `image`: bound to render target `rt0`
// - 1 render target:
//		- `rt0`: description bound to parameter `targetDesc`
// - 2 parameters:
//		- `targetDesc`: "Target description", RenderTarget
//      -

/*constexpr img::RenderTargetDesc rt_0 = RenderTarget("image");
constexpr img::ImgOutputDesc output_image =
ImgOutput("image").renderTarget(rt_0); constexpr img::Param<>*/

// ImgShader:
// - dynamic outputs
// - dynamic inputs
// - dynamic render targets
// - 2 parameters:
//		- `fragCode`: "Fragment Code", string
//		- `imgOutputScale`: "Image Output Scale", ImageOutputScale
class ImgClear : public img::ImgNode {
public:
  ImgClear(node::Network &parent, std::string name, node::Blueprint& blueprint)
      : ImgNode{parent, std::move(name), blueprint } {

    /*gfx::ImageDesc defaultDesc;
    defaultDesc.dimensions = gfx::ImageDimensions::Image2D;
    defaultDesc.format = gfx::Format::R8G8B8A8_SRGB;
    defaultDesc.width = 1920;
    defaultDesc.height = 1080;
    defaultDesc.depth = 1;
    defaultDesc.arrayLayerCount = 1;
    defaultDesc.mipMapCount = 1;
    defaultDesc.sampleCount = 1;
    defaultDesc.usage = gfx::ImageUsageFlags::All;
    target_ = createRenderTarget(defaultDesc);

    output_ = createOutput("image");
        assignOutputImage(output_, target_);*/

    // non-declarative way: create render target, create output, create
    // parameters, assign output to input
  }

  void execute(gfx::GraphicsBackend &gfx,
               const ScreenSpaceContext &ctx) override;

private:
  node::Output *output_;
  RenderTarget *target_;
};

} // namespace img