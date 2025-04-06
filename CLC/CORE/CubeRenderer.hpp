#pragma once
#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include <iostream>

class CubeRenderer : public RenderComponent {
public:

    void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override;
    void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;

    void init() override;
	void draw(const std::shared_ptr<Camera> cam) override;
    ~CubeRenderer();

};

