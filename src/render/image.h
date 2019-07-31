#pragma once
#include "gfx/sampler.h"
#include "gfx/gfx.h"
#include "util/filesystem.h"

namespace render {

	class ShaderGenerator;
	class SVal;

	class Image {
	public:

		/// Returns a buffer containing the image data 
		//ImageBuffer* getImageData();

		/// Returns the GPU image associated with this image. If no GPU resource has been created for this image,
		/// then it will create one on the fly.
		gfx::Image* getGPUImage();


		/// Generates a `sample()` expression.
		SVal genSample2D(ShaderGenerator& gen, SVal x, SVal y, gfx::SamplerDesc samplingParams, SVal& outVal, SBinding& outBinding);




	private:
		gfx::Image gpu_;
		// Path to the file containing the image if the image has been loaded from a file
		util::path path_;
		// TODO generating expression		
	};

}