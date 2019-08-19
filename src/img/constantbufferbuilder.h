#pragma once
#include "gfx/gfx.h"
#include <vector>

namespace img {

enum class Std140BaseType {
  Float,
  Int,
};

class ConstantBufferBuilder {
public:
  size_t push(float val);
  size_t push(int val);
  size_t size() const {
	  return buf_.size();
  }
  // TODO vectors and matrices

  gfx::Buffer create(gfx::GraphicsBackend *gfx);

private:
  std::vector<char> buf_;
};

inline size_t ConstantBufferBuilder::push(float val) {
  auto off = buf_.size();
  // should already be properly aligned
  buf_.resize(buf_.size() + 4);
  *(float *)(buf_.data() + off) = val;
  return off;
}

inline size_t ConstantBufferBuilder::push(int val) {
  auto off = buf_.size();
  buf_.resize(buf_.size() + 4);
  *(int *)(buf_.data() + off) = val;
  return off;
}

inline gfx::Buffer ConstantBufferBuilder::create(gfx::GraphicsBackend *gfx) {
  return gfx::Buffer{gfx, buf_.data(), buf_.size()};
}

} // namespace render
