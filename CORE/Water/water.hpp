#pragma once


#include "RenderComponents/RenderComponent.hpp"




class WaterRenderer : public RenderComponent {
public:

	void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override;
	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;

	void init() override;
	void draw(const std::shared_ptr<Camera> cam) override;
	~CubeRenderer();

	const char* getName() override { return "WaterRenderer"; }

};

