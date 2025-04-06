#pragma once

#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include <iostream>


class CubeMap : public RenderComponent {
protected:
	std::vector<std::string> faces = {};
public:
	void init() override;
	void draw(const glm::mat4& view, const glm::mat4& projection) override;

	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;

	void bindTextures() {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textures[0]->id);
		m_shader->setInt("skybox", 0);
	}

	void setTexture(const std::vector<std::string>& faces) {
		addTexture(TextureManager::loadCubemap(faces));
	}

	void setHDRTexture(const std::string& path) {
		// Load with dimension info
		auto hdrInfo = TextureManager::loadHDRTextureWithInfo(path);
		if (!hdrInfo.texture || hdrInfo.width == 0) {
			std::cerr << "Failed to load HDR texture: " << path << std::endl;
			return;
		}

		// Convert with automatic resolution
		auto cubemap = TextureManager::convertHDRToCubemap(hdrInfo);
		if (!cubemap) {
			std::cerr << "Failed to convert HDR to cubemap" << std::endl;
			return;
		}

		// Store result
		m_textures.clear();
		addTexture(cubemap);
	}
};