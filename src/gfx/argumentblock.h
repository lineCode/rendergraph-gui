#pragma once
#include "gfx/types.h"

namespace gfx {

class ArgumentBlock {
public:
  virtual ~ArgumentBlock() = 0;

  virtual void setArgumentBlock(int index, const ArgumentBlock *block) = 0;
  virtual void setShaderResource(int resourceIndex,
                                 SampledImageView imgView) = 0;
  virtual void setShaderResource(int resourceIndex, ConstantBufferView buf) = 0;
  virtual void setShaderResource(int resourceIndex, StorageBufferView buf) = 0;
  virtual void setVertexBuffer(int index, VertexBufferView buf) = 0;
  virtual void setIndexBuffer(int index, IndexBufferView buf) = 0;
};

} // namespace gfx