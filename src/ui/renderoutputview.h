#pragma once
#include "gfx/image.h"
#include "gfx/pipeline.h"
#include "gfx/signature.h"
#include "gfxopengl/opengl.h"
#include "util/log.h"
#include <QOpenGLWindow>
#include <QSurfaceFormat>

namespace ui {

static const char BACKGROUND_VERT[] = R"(
#version 450
layout(location = 0) in vec2 a_position;
out vec2 v_position;
void main() {
	gl_Position = vec4(a_position, 1.0, 1.0);
	v_position = a_position;
}
)";

static const char BACKGROUND_FRAG[] = R"(
#version 450
/*layout(binding=0,std140) uniform Globals {
    vec2 u_resolution;
    vec2 u_scroll_offset;
    float u_zoom;
};*/
in vec2 v_position;
layout(location=0) out vec4 out_color;
void main() {
	vec2 u_resolution = vec2(640, 480);
	vec2 u_scroll_offset = vec2(0, 0);
	float u_zoom = 1.0;
    vec2 px_position = v_position * vec2(1.0, -1.0) * u_resolution * 0.5;
    // #005fa4
    float vignette = clamp(0.7 * length(v_position), 0.0, 1.0);
    out_color = mix(
        vec4(0.0, 0.47, 0.9, 1.0),
        vec4(0.0, 0.1, 0.64, 1.0),
        vignette
    );
    // TODO: properly adapt the grid while zooming in and out.
    float grid_scale = 5.0;
    if (u_zoom < 2.5) {
        grid_scale = 1.0;
    }
    vec2 pos = px_position + u_scroll_offset * u_zoom;
    if (mod(pos.x, 20.0 / grid_scale * u_zoom) <= 1.0 ||
        mod(pos.y, 20.0 / grid_scale * u_zoom) <= 1.0) {
        out_color *= 1.2;
    }
    if (mod(pos.x, 100.0 / grid_scale * u_zoom) <= 2.0 ||
        mod(pos.y, 100.0 / grid_scale * u_zoom) <= 2.0) {
        out_color *= 1.2;
    }
}
)";

class RenderOutputView : public QOpenGLWindow {
  Q_OBJECT
public:
  RenderOutputView() {
    QSurfaceFormat fmt;
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setOption(QSurfaceFormat::DebugContext);
    setFormat(fmt);
    // check for a 4.5 core profile context
    auto f = format();
    util::log("OpenGL context version: {}.{}", f.version().first,
              f.version().second);
    if (f.profile() != QSurfaceFormat::CoreProfile) {
      util::log("OpenGL context is not a core profile context.");
    }
  }

  void initializeGL() override {
    util::log("initializeGL");
    g_ = std::make_unique<gfxopengl::OpenGLGraphicsBackend>();

	gfx::ShaderModule vert{ g_.get(), BACKGROUND_VERT, gfx::ShaderStageFlags::VERTEX };
	gfx::ShaderModule frag{ g_.get(), BACKGROUND_FRAG, gfx::ShaderStageFlags::FRAGMENT };

	gfx::RenderPassTargetDesc rptDesc[1] = {
		gfx::RenderPassTargetDesc{ gfx::ColorF{0.0, 1.0, 0.0, 1.0} }
	};

	gfx::RenderPassDesc rpDesc;
	rpDesc.colorTargets = util::makeConstArrayRef(rptDesc);
	rpDesc.depthTarget = nullptr;

	renderPass_ = gfx::RenderPass{ g_.get(), rpDesc };
	
	gfx::VertexLayoutElement layoutElements[1] = {
		gfx::VertexLayoutElement {
			gfx::Semantic { "POSITION", 0 },
			gfx::Format::R32G32_SFLOAT,
			0
		}
	};

	gfx::VertexInputBinding binding;
	binding.layout.elements.data = layoutElements;
	binding.layout.elements.len = 1;
	binding.layout.stride = 8;
	binding.baseLocation = 0;
	binding.rate = gfx::VertexInputRate::Vertex;

	gfx::SignatureDesc sigDesc;
	sigDesc.vertexInputs.data = &binding;
	sigDesc.vertexInputs.len = 1;
	signature_ = gfx::Signature{ g_.get(), sigDesc };

	gfx::GraphicsPipelineDesc desc;
	desc.signature = signature_;
	desc.renderPass = renderPass_;
	desc.shaderStages.vertex = vert;
	desc.shaderStages.fragment = frag;
	pipeline_ = gfx::GraphicsPipeline{ g_.get(), desc };

	
  }

  void resizeGL(int w, int h) override { util::log("resizeGL {} {}", w, h); }

  void paintGL() override {
	util::log("paintGL");
    gfx::ImageDesc desc;
    desc.width = width();
    desc.height = height();
    desc.format = gfx::Format::R16G16B16A16_SFLOAT;


	float left = -1.0f, top = 1.0f, right = 1.0f, bottom = -1.0f;
	float vertexData[12] = { left, top, right, top, left, bottom, left, bottom, right, top, right, bottom };

	gfx::BufferHandle vtx = g_->createConstantBuffer(vertexData, 12 * sizeof(float));

	gfx::ArgumentBlockHandle args = g_->createArgumentBlock(signature_);
	g_->argumentBlockSetVertexBuffer(args, 0, gfx::VertexBufferView{vtx, 0, 12 * sizeof(float) });

	gfx::Image img{ g_.get(), desc };
	gfx::RenderTargetView imgRTV = img.asRenderTargetView();

	gfx::FramebufferDesc fbDesc;
	fbDesc.colorTargets = { 1, &imgRTV };
	fbDesc.depthTarget = nullptr;

	gfx::Framebuffer fbo{ g_.get(), fbDesc };

    g_->clearRenderTarget(gfx::RenderTargetView{img},
                          gfx::ColorF{0.5f, 0.5f, 0.7f, 1.0f});

	gfx::DrawParams params;
	params.firstVertex = 0;
	params.vertexCount = 6;
	params.firstInstance = 0;
	params.instanceCount = 1;
	g_->draw(pipeline_, fbo, args, params);

    g_->presentToScreen(img, width(), height());

  }

private:
  std::unique_ptr<gfxopengl::OpenGLGraphicsBackend> g_;

  gfx::Framebuffer framebuffer_;
  gfx::RenderPass renderPass_;
  gfx::GraphicsPipeline pipeline_;
  gfx::Signature signature_;
};

} // namespace ui