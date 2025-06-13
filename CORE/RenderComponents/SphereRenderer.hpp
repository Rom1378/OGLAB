#pragma once
#include "RenderComponent.hpp"
#include "../GameObject.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <array>

constexpr float M_PI = 3.14159265359f;  // Fixed M_PI definition

class SphereRenderer : public RenderComponent {
public:
	SphereRenderer(float radius = 1.0f, unsigned int sectorCount = 36, unsigned int stackCount = 18)
		: RenderComponent(), radius(radius), sectorCount(sectorCount), stackCount(stackCount) {
	}

	~SphereRenderer();

	void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override;
	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;
	void init() override;
	void draw(const std::shared_ptr<Camera> cam) override;


private:

	float radius;
	unsigned int sectorCount, stackCount;

	void initBuffers();
	void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount);

};
