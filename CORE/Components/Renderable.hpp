#pragma once

#include <string>

#include "CORE/Systems/Renderer/MeshRenderer.hpp"


struct RenderableComponent {

	RenderableComponent(MeshRenderer::RenderableHandle handle);
	RenderableComponent(MeshRenderer::Primitive t);
	RenderableComponent(const std::string& model_path);


	MeshRenderer::RenderableHandle rhandle{ MeshRenderer::INVALID_HANDLE };

};
