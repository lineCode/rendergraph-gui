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

namespace render {
	
}