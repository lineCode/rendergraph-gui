#pragma once
#include "gfx/gfx.h"
#include "gfx/image.h"
#include "img/imgnetwork.h"
#include "img/imgnode.h"
#include <vector>

namespace img {

struct SharedRenderTarget {
  gfx::ImageHandle image;
};

struct ImgNodeData {
  struct RenderTarget {
    std::string                         name;
    gfx::ImageDesc                      desc;
    std::shared_ptr<SharedRenderTarget> shared;
  };

  std::vector<RenderTarget> renderTargets;
};

class ImgEvaluator {
public:
  ImgEvaluator(gfx::GraphicsBackend &gfx, ImgNetwork &network);
  ~ImgEvaluator();

  void defaultImageSize(int &width, int &height) const;
  void setDefaultImageSize(int width, int height);

  gfx::Format           defaultImageFormat() const;
  void                  setDefaultImageFormat(gfx::Format format);
  gfx::GraphicsBackend &gfx() const { return gfx_; }

  void evaluate();

private:
  void allocateRenderTargets();

  std::vector<node::Node *> sortedNodes_;
  ImgNetwork &              network_;
  gfx::GraphicsBackend &    gfx_;
  int                       defaultWidth_;
  int                       defaultHeight_;
  gfx::Format               defaultFormat_;
  std::vector<ImgNodeData>  nodeData_;
  double                    currentTime_;
  int                       currentFrame_;
  gfx::ConstantBufferView   commonParameters_;
  gfx::VertexBufferView     quadVertices_;
};

class ImgContext {
public:
  ImgContext(ImgEvaluator &evaluator, ImgNode &node, ImgNodeData &nodeData);

  void defaultImageSize(int &width, int &height) const {
    evaluator_.defaultImageSize(width, height);
  }
  gfx::Format defaultImageFormat() const {
    return evaluator_.defaultImageFormat();
  }

  gfx::ImageHandle getInputImage(node::Input *input);

  //------ Render targets ------

  /// Sets the description of a render target identified by name.
  void setRenderTargetDesc(util::StringRef name, const gfx::ImageDesc &desc);
  /// Returns the image description of a render target, or nullptr if it's not
  /// the name of an existing render target.
  const gfx::ImageDesc *getRenderTargetDesc(util::StringRef renderTarget) const;
  /// Deletes the specified registered render target.
  void deleteRenderTarget(util::StringRef renderTarget);
  /// Returns a "RenderTargetView" object suitable for rendering to the
  /// specified render target.
  gfx::RenderTargetView getRenderTargetView(util::StringRef renderTarget);

  //------ graphics ------

  /// Returns an interface to the graphics backend.
  gfx::GraphicsBackend &gfx() const { return evaluator_.gfx(); }

private:
  ImgNodeData::RenderTarget *findRenderTarget(util::StringRef name) const;
  ImgNodeData::RenderTarget *findOrCreateRenderTarget(util::StringRef name);

  ImgEvaluator &evaluator_;
  ImgNode &     node_;
  ImgNodeData & nodeData_;
};
} // namespace img