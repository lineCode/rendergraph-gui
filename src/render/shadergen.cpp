#include "render/shadergen.h"
#include <vector>

namespace render {
	struct ShaderGeneratorPrivate {

		struct SamplerUniform {
			int tex;
			gfx::SamplerDesc desc;
		};

		std::vector<TextureBinding> textures_;
		std::vector<SamplerUniform> samplers_;
	};
}