#pragma once
#include "gfx/image.h"
#include "gfx/sampler.h"
#include "gfx/shader.h"
#include "util/stringref.h"

namespace render {
	/// Identifies a value in a shader generator context.
	using SVal = int;
	using SType = int;
	using SCtx = int;
	using SFunCtx = int;

	enum class PrimitiveType {
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Matrix44
	};


	class ShaderGenerator {
	public:
		enum class Target {
			OpenGL_GLSL,
			// TODO other targets
		};

		ShaderGenerator(gfx::ShaderStageFlags stage, Target target);

		/// Returns the type of the value
		SType typeOf(SVal val);

		SType primitiveType(PrimitiveType primty);

		/// Adds a global texture resource
		SVal addTextureResource(gfx::ImageDimensions dim);
		/// Adds a global uniform value that goes in the given buffer identifier.
		SVal addGlobalParameter(util::StringRef name, SType ty, int buffer = 0);

		/// generates a `sample` expression in the current context 
		SVal emitSample2D(SFunCtx f, SVal texture, gfx::SamplerDesc sampler, SVal x, SVal y);

		
		SVal addInput(util::StringRef name, SType ty, int location);
		SVal addOutput(util::StringRef name, SType ty, int location);
		
		SFunCtx beginVertexShader();
		void endVertexShader(SFunCtx ctx);
		
		SFunCtx beginFunction();
		void addFunctionArgument(SFunCtx f, util::StringRef name, SType ty);
		void endFunction(SFunCtx f);

	private:

	};
}