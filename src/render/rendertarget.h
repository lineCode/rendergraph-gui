#pragma once
#include "gfx/image.h"
#include "node.h"

namespace render {

class ScreenSpaceNode;
struct ScreenSpaceContext;

struct RenderTargetStorage {
  gfx::ImageDesc desc;
  gfx::Image image;
};

class RenderTarget : public Node {
  struct CtorTag {
    explicit CtorTag() = default;
  };

public:
  using Ptr = std::unique_ptr<RenderTarget>;

  RenderTarget(CtorTag, const gfx::ImageDesc &desc, std::string name)
      : Node{std::move(name)}, desc_{desc} {}

  static RenderTarget *make(ScreenSpaceNode *parent, std::string name,
                            const gfx::ImageDesc &desc);

  const gfx::ImageDesc &desc() const { return desc_; }

  gfx::ImageHandle getImage(const ScreenSpaceContext &ctx); // TODO

private:
  gfx::ImageDesc desc_;
  std::shared_ptr<RenderTargetStorage> storage_;
};

} // namespace render