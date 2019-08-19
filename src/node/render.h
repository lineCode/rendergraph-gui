#pragma once
// The renderer and the render graph nodes

// "Values" are defined in a context, or "space". Values are communicated between nodes.
// - Vertex-space values (a.k.a. geometry)
// - Fragment-space values (fragment streams)
// - Screen-space values (a.k.a. pixels)
// - Full images

// Nodes represent an operation on "values" 
// - Screen-space nodes accept values or parameters in screen-space f(x,y) and produce other values in screen-space g(x,y)
// - parameters can be constants, 2D functions (shader code snippets with u,v,t in context), and have dependencies on other values (and nodes).
//		- parameters are "evaluated in context"
// - Parameter
//		- ConstantParameter
//      - CodeSnippet
//		- Texture (another screen-space value)
// - These parameters know how to "insert" themselves into a shader.
// - For convenience, parameters are nodes themselves (owned by the parent node)		

/// Base class for node parameters.
/// 
/// Parameters have a type, and have can be associated to an "expression". 
///
/// E.g. if user wants an "image" or "texture" parameter, can set the parameter to:
/// - a file: file:///./stuff.png
/// - a file over the network: https://example.org/stuff.png
/// - a reference to a node output: op:../../node/output_0
/// - an expression that produces pixel values: 
/// 
/// If the user wants a scalar value, can set the parameter to:
/// - the scalar itself: `3.14`
/// - an expression that evaluates to a scalar: `2.0 * $time`
/// - an expression involving a texture: `sample(tex, $x,$y)`
///
/// There is what the parameter is expected to be, and what the user inputs.
/// Implicit conversions are possible:
/// e.g. scalar -> image 
///      expr with $x,$y -> image
///      op path -> image if the output is an image
///
/// Image
/// + data on GPU (if requested, possibly shared)
/// + data on CPU (if requested, possibly shared)
/// + source:
///		- FileBacked -> load image data from a file
///		- Procedural -> load image data from an expression
///		- Op -> image produced by an Op (Op has shared ref to this image)
/// 
/// + render pipeline interface (component attached to the image object)
///		- get GPU texture object
///		- get GPU render target object
///     - mark CPU side data as dirty (require CPU->GPU upload)
///		- mark GPU side data as dirty (require GPU->CPU upload)
///
/// In the end, an image has several components
/// - Description (Image)
/// - CPU data (ImageBuffer), possibly shared
/// - GPU data (gfx::Image), possibly shared
/// - Data source (ImageDataSource)

namespace render {
	
}