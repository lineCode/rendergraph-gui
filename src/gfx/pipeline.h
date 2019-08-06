#pragma once
#include "gfx/format.h"
#include "gfx/types.h"

namespace gfx {


struct GraphicsShaderStages {
  ShaderModuleHandle vertex = 0;
  ShaderModuleHandle fragment = 0;
  ShaderModuleHandle tessControl = 0;
  ShaderModuleHandle tessEval = 0;
  ShaderModuleHandle geometry = 0;
};

enum class PrimitiveTopology {
  PointList,
  LineList,
  TriangleList,
};

enum class PolygonMode { Line, Fill };

enum class CullModeFlags {
  None = 0,
  Front = 1,
  Back = 2,
  FrontAndBack = 3,
};

enum class LogicOp {
  Clear = 0,
  And = 1,
  AndReverse = 2,
  Copy = 3,
  AndInverted = 4,
  NoOp = 5,
  Xor = 6,
  Or = 7,
  Nor = 8,
  Equivalent = 9,
  Invert = 10,
  OrReverse = 11,
  CopyInverted = 12,
  OrInverted = 13,
  Nand = 14,
  Set = 15,
};

enum class CompareOp {
  Never = 0,
  Less = 1,
  Equal = 2,
  LessOrEqual = 3,
  Greater = 4,
  NotEqual = 5,
  GreaterOrEqual = 6,
  Always = 7,
};

enum class StencilOp {
  Keep = 0,
  Zero = 1,
  Replace = 2,
  IncrementAndClamp = 3,
  DecrementAndClamp = 4,
  Invert = 5,
  IncrementAndWrap = 6,
  DecrementAndWrap = 7,
};

enum class BlendFactor {
  Zero = 0,
  One = 1,
  SrcColor = 2,
  OneMinusSrcColor = 3,
  DstColor = 4,
  OneMinusDstColor = 5,
  SrcAlpha = 6,
  OneMinusSrcAlpha = 7,
  DstAlpha = 8,
  OneMinusDstAlpha = 9,
  ConstantColor = 10,
  OneMinusConstantColor = 11,
  ConstantAlpha = 12,
  OneMinusConstantAlpha = 13,
  SrcAlphaSaturate = 14,
  Src1Color = 15,
  OneMinusSrc1Color = 16,
  Src1Alpha = 17,
  OneMinusSrc1Alpha = 18,
};

enum class BlendOp {
  Add = 0,
  Subtract = 1,
  ReverseSubtract = 2,
  Min = 3,
  Max = 4,
};

struct StencilOpState {
  StencilOp fail;
  StencilOp pass;
  StencilOp depthFail;
  CompareOp compare;
  uint32_t compareMask;
  uint32_t writeMask;
  uint32_t reference;
};

struct DepthStencilState {
  bool depthTestEnable = false;
  bool depthWriteEnable = false;
  CompareOp depthCompareOp;
  bool depthBoundsTestEnable = false;
  float minDepthBounds = 0.0;
  float maxDepthBounds = 0.0;
  bool stencilTestEnable = false;
  StencilOpState stencilFrontOps;
  StencilOpState stencilBackOps;
};

struct ColorBlendAttachmentState {
  bool enabled = false;
  BlendFactor srcColor;
  BlendFactor dstColor;
  BlendOp colorOp;
  BlendFactor srcAlpha;
  BlendFactor dstAlpha;
  BlendOp alphaOp;

  /// Comparison operator
  constexpr bool operator==(const ColorBlendAttachmentState &rhs) const {
    return (enabled == false && rhs.enabled == false) ||
           (enabled == rhs.enabled && srcColor == rhs.srcColor &&
            dstColor == rhs.dstColor && colorOp == rhs.colorOp &&
            srcAlpha == rhs.srcAlpha && dstAlpha == rhs.dstAlpha &&
            alphaOp == rhs.alphaOp);
  }
  constexpr bool operator!=(const ColorBlendAttachmentState &rhs) const {
	  return !this->operator==(rhs);
  }
};

struct ScissorState {};
struct ViewportState {};
struct RasterizationState {};
struct MultisampleState {};
struct InputAssemblyState {};
struct ColorBlendState {};

struct GraphicsPipelineDesc {
  /// RenderPass that the pipeline conforms to. This pipeline will only be able
  /// to render to framebuffers created with this renderpass.
  RenderPassHandle renderPass = 0;
  /// Signature of the pipeline. This pipeline will only accept argument blocks
  /// created with this signature.
  SignatureHandle signature = 0;
  GraphicsShaderStages shaderStages;
  ViewportState viewportState;
  RasterizationState rasterizationState;
  MultisampleState multisampleState;
  DepthStencilState depthStencilState;
  InputAssemblyState inputAssemblyState;
  ColorBlendState colorBlendState;
};

} // namespace gfx