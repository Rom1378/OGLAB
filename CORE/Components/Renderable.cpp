#include "Renderable.hpp"

#include "CORE/Systems/Renderer/MeshRenderer.hpp"
#include "CORE/Debug.hpp"




RenderableComponent::RenderableComponent(MeshRenderer::RenderableHandle handle) {
	if (MeshRenderer::isValidHandle(handle))
		this->rhandle = handle;
	else
		this->rhandle = MeshRenderer::INVALID_HANDLE;
	
}

RenderableComponent::RenderableComponent(std::string model_path) {
	this->rhandle = MeshRenderer::createRenderableFromFile(model_path);
}

RenderableComponent::RenderableComponent(MeshRenderer::Primitive t){

	switch (t)
	{
	case MeshRenderer::Primitive::CUBE:
		rhandle = MeshRenderer::createCube();
		break;
	case MeshRenderer::Primitive::SPHERE:
		rhandle = MeshRenderer::createSphere();
		break;
	default:
		LOG_ERROR("Mesh type does not exist");
		rhandle = MeshRenderer::INVALID_HANDLE;
		break;
	}
}
