#pragma once
#include "gfx/image.h"
#include "gfx/pipeline.h"
#include "img/imgnetwork.h"
#include "img/rendertarget.h"
#include "node/network.h"
#include "node/node.h"
#include <memory>
#include <string>

namespace img {

enum class ImageOutputScale {
  /// output size is size defined in the current project
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

///
/// A node representing a screen space operation.
///
/// Contains methods to manage render targets and bind render targets and images
/// to outputs.
///
class ImgNode : public node::Node {
public:
  ImgNode(node::Network &parent, std::string name, node::Blueprint& blueprint);

  //------ Render targets ------

  /// Registers a new render target for this node with the given image
  /// description.
  RenderTarget *createRenderTarget(const gfx::ImageDesc &desc);
  /// Sets the image description for a registered render target.
  void setRenderTargetDesc(RenderTarget *renderTarget,
                           const gfx::ImageDesc &desc);
  /// Returns the image description of a registered render target.
  const gfx::ImageDesc &getRenderTargetDesc(RenderTarget *renderTarget);
  /// Returns the image allocated for a registered render target.
  gfx::ImageHandle getRenderTargetImage(RenderTarget *renderTarget);
  /// Deletes the specified registered render target.
  void deleteRenderTarget(RenderTarget *output);

  //------ Output ------

  /// Assigns a render target to the specified output
  void assignOutputImage(node::Output *output, RenderTarget *renderTarget);
  /// Assigns an image to the specified output.
  void assignOutputImage(node::Output *output, const gfx::ImageDesc &desc,
                         gfx::ImageHandle target);
  /// Returns the image handle of the specified output, along with the image description.
  /// May return a null handle if no image was assign to the specified output.
  gfx::ImageHandle getOutputImage(node::Output *output, gfx::ImageDesc &outDesc);


  virtual void execute(gfx::GraphicsBackend &gfx,
                       const ScreenSpaceContext &ctx) = 0;

  static RenderTargetCache &renderTargetCache();

protected:
  void assignOutputImagePrivate(node::Output *output, RenderTarget *renderTarget,
                                const gfx::ImageDesc &desc,
                                gfx::ImageHandle target);

  struct OutputTarget {
    // corresponding output
    node::Output *output;
    // The render target that was assigned to this output. nullptr if this
    // output was not assigned a render target.
    RenderTarget *renderTarget;
    gfx::ImageDesc desc;
    gfx::ImageHandle image;
  };

  ImgNetwork &parent_;
  std::vector<RenderTarget *> renderTargets_;
  std::vector<OutputTarget> outputMap_;
};

} // namespace img
