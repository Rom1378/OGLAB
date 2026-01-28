#pragma once

#include <string>

#include "CORE/Systems/Renderer/MeshRenderer.hpp"

#include "CORE/Transform.hpp"

struct RenderableComponent {

	RenderableComponent(MeshRenderer::RenderableHandle handle);
	RenderableComponent(MeshRenderer::Primitive t);
	RenderableComponent(std::string model_path);


	MeshRenderer::RenderableHandle rhandle{ MeshRenderer::INVALID_HANDLE };
	Transform localTransform{ };
};
