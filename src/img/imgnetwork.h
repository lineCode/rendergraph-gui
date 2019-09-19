#pragma once
#include "gfx/gfx.h"
#include "img/rendertarget.h"
#include "node/network.h"
#include "node/node.h"
#include "node/template.h"

namespace img {

struct ScreenSpaceContext {
  /// Width of the viewport in pixels
  int width;
  /// Height of the viewport in pixels
  int height;
  /// Current time
  double currentTime;
  /// Current frame
  int currentFrame;
  /// Constant buffer containing the common parameters (camera, matrices, etc.)
  gfx::ConstantBufferView commonParameters;
  gfx::VertexBufferView   quadVertices;
};

struct RenderTargetDesc {
  //
};

class ImgTemplate;
class ImgNode;
class ImgNetwork;

class ImgNetwork : public node::Network {
  friend class ImgNode;
  friend class ImgOutput;

public:
  using Ptr = std::unique_ptr<ImgNetwork>;

  ImgNetwork(util::StringRef name);

  /*
  ImgNetwork(node::Network * parent, util::StringRef name, node::NodeTemplate&
  tpl) : Network{parent, name, tpl}
  {
  }*/

  void  onChildAdded(Node *node) override;
  void  onChildRemoved(Node *node) override;
  Node *createNode(util::StringRef typeName, util::StringRef name) override;

  /// Registers a template for creating an IMG node.
  static void    registerTemplate(util::StringRef name,
                                  util::StringRef friendlyName,
                                  util::StringRef description,
                                  util::StringRef icon,
                                  util::ArrayRef<const node::ParamDesc*> params,
	  util::ArrayRef<const node::InputDesc *> inputs,
	  util::ArrayRef<const node::OutputDesc *> outputs,
                                  node::Constructor constructor);

  node::TemplateTable &templates() const override { return imgTemplates_; }

private:
  static node::NodeTemplate &getTemplate();
  void                       setOutput(ImgOutput *output);

  static node::TemplateTable imgTemplates_;
  ImgOutput *                output_ = nullptr;
};

} // namespace img