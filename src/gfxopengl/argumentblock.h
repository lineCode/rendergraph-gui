#pragma once
#include "gfx/argumentblock.h"

namespace gfxopengl {

class ArgumentBlock : public gfx::ArgumentBlock {
public:
  void setArgumentBlock(int index, const ArgumentBlock *block);
  void setShaderResource(int resourceIndex,
                                 SampledImageView imgView);
  void setShaderResource(int resourceIndex, ConstantBufferView buf);
  void setShaderResource(int resourceIndex, StorageBufferView buf);
  void setVertexBuffer(int index, VertexBufferView buf);
  void setIndexBuffer(int index, IndexBufferView buf);

private:
};

} // namespace gfxopengl