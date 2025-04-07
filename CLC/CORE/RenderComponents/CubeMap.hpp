#pragma once

#include "RenderComponent.hpp"
#include "../GameObject.hpp"
#include <iostream>

class CubeMap : public RenderComponent {
protected:
	std::vector<std::string> faces = {};
public:
	void init() override;
	void draw(const glm::mat4& view, const glm::mat4& projection) override;

	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;
	void setTexture(const std::vector<std::string>& faces) { addTexture(TextureManager::loadCubemap(faces)); }

	void bindTextures();
	void setHDRTexture(const std::string& path);
};