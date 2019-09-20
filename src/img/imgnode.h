#pragma once
#include "gfx/gfx.h"
#include "node/network.h"
#include "node/node.h"
#include "util/stringref.h"
#include <memory>
#include <string>

namespace img {

class ImgContext;
class ImgNetwork;

/// A node representing a screen space operation.
///
/// In addition to inputs, outputs, and parameters, IMG nodes also keep track
/// of eventual render targets needed by the operation.
class ImgNode : public node::Node {
public:
  ImgNode(node::Network &parent, util::StringRef name);

  //------ outputs -------
  
  /// Returns the image handle associated to the specified output.
  /// This method should be overriden in derived classes.
  /// The default implementation returns the image associated with the render target of the same name.
  virtual gfx::ImageHandle getOutputImage(ImgContext& ctx, util::StringRef outputName);

  //------ execution ------

  /// Called before rendering so that the system knows what render targets are going to be used.
  /// Implementors should call setRenderTargetDesc() within this function for each target that the node needs.
  virtual void prepare(ImgContext& ctx) = 0;
  /// Executes the node. Nodes should call operations on the graphics context here.
  virtual void execute(ImgContext& ctx) = 0;

protected:
  ImgNetwork &parent_;
};

} // namespace img
