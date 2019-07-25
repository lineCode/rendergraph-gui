#pragma once
#include "util/arrayref.h"
#include "gfx/format.h"
#include "gfx/shader.h"

namespace gfx {

struct FragmentOutputDescription {};

/*pub struct VertexInputAttributeDescription<'tcx> {
pub location: Option<u32>,
pub ty: &'tcx TypeDesc<'tcx>,
pub semantic: Option<Semantic<'tcx>>,
}

#[derive(Copy, Clone, Debug, Eq, PartialEq, Hash)]
        pub struct VertexLayoutElement < 'tcx> {
                pub semantic : Option < Semantic < 'tcx>>,
                pub format : Format,
                pub offset : u32,
}*/

struct Semantic {
	const char *name;
	/// Index relative to the base semantic index of the VertexInputBinding
	int index;
};

/// Description of a vertex attribute within a vertex layout.
struct VertexLayoutElement {
	Semantic semantic;
	Format format;
	uint32_t offset;
};

/// Describes the layout of vertex data inside a single vertex buffer.
/// Created by the user, must match struct layout of vertices.
struct VertexLayout {
  /// Description of individual vertex attributes inside the buffer.
  util::ArrayRef<VertexLayoutElement> elements;
  /// Number of bytes to go to the next element.
  size_t stride;
};

/*/// Description of one vertex input attribute of a vertex shader.
/// Should match what's in the vertex shader.
/// Could be automatically generated from the vertex shader.
/// TODO unused right now since we don't do shader reflection
struct VertexInputAttributeDescription {
        int location;

};*/

enum class VertexInputRate {
  Vertex,
  Instance,
};

enum class IndexFormat {
	/// 16-bit unsigned integer indices
	U16,
	/// 32-bit unsigned integer indices
	U32,
};

/// This describes a vertex buffer input from a graphics pipeline.
///
/// Note: we consider that all elements from the vertex layout correspond to one
/// attribute in the shader. All elements from the layout are assigned an
/// "attribute location" sequentially, starting with "baseLocation".
struct VertexInputBinding {
  ///
  VertexLayout layout;
  VertexInputRate rate;
  int baseLocation = -1;
};

enum class ResourceShape {
	R1d,
	R1dArray,
	R2d,
	R2dArray,
	R2dMultisample,
	R2dMultisampleArray,
	R3d,
	RCube,
	RBuffer,
};

enum class ResourceBindingType {
	Sampler,
	Texture,
	TextureSampler,
	RwImage,
	ConstantBuffer,
	RwBuffer,
	TexelBuffer,
	RwTexelBuffer,
};

/// Defines a shader resource binding point (constant buffer, storage buffer, sampled texture, sampler, image, etc.).
struct ResourceBinding {
	/// Binding index 
	/// Note: there is also a "set index", but it is inferred from the hierarchy of signatures.
	int32_t index;
	/// Descriptor type
	ResourceBindingType ty;
	ResourceShape shape;
	/// Which shader stages will see this descriptor
	ShaderStageFlags visibility;
	/// TODO How many descriptors in the binding? Should be 1
	int32_t count;

	/*/// Precise description of the expected data type (image format).
	///
	/// Can be None if no type information is available for this binding.
	pub data_ty : Option<&'tcx TypeDesc<'tcx>>,
	/// Data layout.
	pub data_layout : Option<&'tcx Layout<'tcx >> ,
	/// Data format for r/w images & texel buffers.
	/// `Format::UNDEFINED` if not applicable (all other binding types)
	pub data_format : Format,*/
};

/// TODO
struct SignatureDesc {
  /// Signatures of inherited argument blocks.
  ///
  /// The length of this slice defines the number of _inherited argument
  /// blocks_.
  util::ArrayRef<const SignatureDesc *> inherited;

  /// Descriptors in the block.
  ///
  /// The length of this slice defines the number of _descriptors_ in a block.
  /// 
  util::ArrayRef<ResourceBinding> shaderResources;

  /// Layouts of all vertex buffers in the block.
  ///
  /// The length of this slice defines the number of _vertex buffers_ in a
  /// block.
  util::ArrayRef<VertexInputBinding> vertexInputs;

  /// (Color) outputs of the fragment shader. The block contains one _render
  /// target_ image for each entry.
  ///
  /// The length of this slice defines the number of _render targets_ in a
  /// block.
  util::ArrayRef<FragmentOutputDescription> fragmentOutputs;

  /// Depth-stencil output of the fragment shader. If not `nullptr` then the
  /// block contains a depth-stencil render target image.
  bool hasdepthStencilFragmentOutput;
  FragmentOutputDescription depthStencilFragmentOutput;

  /// The format of the index buffer. If not `None`, then the block contains an
  /// index buffer.
  bool hasIndexFormat;
  IndexFormat indexFormat;

  /// The number of viewports defined in the block.
  ///
  /// At most one signature in a signature tree can have a non-zero number of
  /// viewports. Equivalently, you cannot split the definition of viewports
  /// across several argument blocks, and when an argument block defines
  /// viewports, it must define all of them at once.
  ///
  /// FIXME: actually check that
  size_t viewportsCount;

  /// The number of scissors defined in the block.
  ///
  /// This follows the same rule as `num_viewports`.
  size_t scissorsCount;

  /*/// Indicates that this block and its inherited blocks fully define the
  outputs of a fragment shader.
  ///
  /// An inheriting block must not define additional fragment outputs in the
  `fragment_outputs`
  /// and `depth_stencil_fragment_output` members.
  ///
  /// The purpose of this flag is to allow backends that need _framebuffer
  objects_ (e.g. OpenGL or Vulkan)
  /// to create them in advance and store them inside long-lived argument blocks
  /// instead of creating them on-the-fly.
  bool isRootFragmentOutputSignature;

  /// Indicates that this block and its inherited blocks fully define the inputs
  of a vertex shader.
  ///
  /// An inheriting block must not define additional vertex inputs in
  `vertex_layouts`. bool isRootVertexInputSignature;*/
};


} // namespace gfx