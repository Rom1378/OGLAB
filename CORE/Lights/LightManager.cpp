#include "LightManager.hpp"
#include <iostream>
#include "../Cameras/Camera.hpp"
#include <glm/glm.hpp>


namespace LightManager
{



	ShadowMapper shadowMapper;

	ShadowMapper* getShadowMapper() {
		return &shadowMapper;
	}

	std::vector<std::shared_ptr<Light>> s_lights;


	void init() {
		//init shadow mapper
		shadowMapper.initialize();

	}

	void shutdown() {
		for (auto& light : s_lights) {
			light.reset();
		}
		s_lights.clear();
		std::cout << "LightManager shutdown complete." << std::endl;
	}

	void bindShadowMap(std::shared_ptr<ShaderProgram> shader) {
		if (!shader) {
			std::cerr << "Error: Invalid shader program in bindShadowMap" << std::endl;
			return;
		}

		shader->use();

		// Set the shadow map uniform
		GLint shadowMapLoc = glGetUniformLocation(shader->getProgram(), "shadowMap");
		if (shadowMapLoc == -1) {
			std::cerr << "Warning: shadowMap uniform not found in shader" << std::endl;
			return;
		}

		// Activate texture unit 1 and bind the depth map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
		shader->setInt("shadowMap", LightManager::getShadowMapper()->getDepthMapTexture());  // Use texture unit 1

		// Set light space matrix uniform
		shader->setMat4("lightSpaceMatrix", glm::value_ptr(LightManager::getShadowMapper()->getLightSpaceMatrix()));

		// Set light position for shadow calculations
		if (!s_lights.empty()) {
			shader->setVec3("lightPos", s_lights[0]->getPosition());
		}
	}

	std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights) {

		std::vector<std::shared_ptr<Light>> relevantLights;
		for (const auto& light : s_lights) {
			// Add logging to verify lights are present

			float distance = glm::distance(light->getPosition(), cam->getPosition());
			if (distance < cam->getMaxLightDistance()) {
				relevantLights.push_back(light);
			}
			if (relevantLights.size() >= maxLights) break;
		}
		return relevantLights;
	}

	void addLight(const std::shared_ptr<Light> light) {
		// Add logging to verify light addition
		std::cout << "Adding light. Current size before: " << s_lights.size() << std::endl;
		s_lights.push_back(light);
		std::cout << "Adding light. Current size after: " << s_lights.size() << std::endl;
	}

	void clearLights() {
		s_lights.clear();
	}

	const std::vector<std::shared_ptr<Light>>& getLights() {
		return s_lights;
	}
}
