#include "Light.hpp"
#include "LightManager.hpp"
#include "CORE/Scene.hpp"

#include <iostream>
#include "../Cameras/Camera.hpp"
#include <glm/glm.hpp>


namespace LightManager
{
	ShadowMapper shadowMapper;

	ShadowMapper* getShadowMapper() {
		return &shadowMapper;
	}

	void ShadowMapper::renderShadowPass(Scene* scene, const std::shared_ptr<Light> light) {
		// 1. Configure render target
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, resolution, resolution);
		glClear(GL_DEPTH_BUFFER_BIT);

		// 2. Calculate light matrices
		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
		glm::mat4 lightView = glm::lookAt(light->getWorldPosition(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
		lightSpaceMatrix = lightProjection * lightView;

		// 3. Render all shadow casters
		glCullFace(GL_FRONT);
		scene->renderShadowCasters(lightSpaceMatrix);
		glCullFace(GL_BACK);

		// 4. Reset state
		Window::bind_framebuffer();
	}

	std::vector<std::shared_ptr<Light>> lights;
	//std::unordered_map<std::string, std::shared_ptr<Light>> lights; //TODO: update imple it use this instead of s_lights


	void init() {
		//init shadow mapper
		shadowMapper.initialize();

	}

	void shutdown() {
		clear();
		//s_lights.clear();
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
		if (!lights.empty()) {
			//shader->setVec3("lightPos", lights[0]->getPosition());
			shader->setVec3("lightPos", lights[0]->getWorldPosition());
		}
	}

	std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights) { //TODO UPDATE FOR WEAK PTR USAGE

		std::vector<std::shared_ptr<Light>> relevantLights;
		for (const auto& light : lights) {
			// Add logging to verify lights are present

			float distance = glm::distance(light->getWorldPosition(), cam->getPosition());
			if (distance < cam->getMaxLightDistance()) {
				relevantLights.push_back(light);
			}
			if (relevantLights.size() >= maxLights) break;
		}
		return relevantLights;
	}

	/*void addLight(const std::shared_ptr<Light> light) {
		// Add logging to verify light addition
		std::cout << "Adding light. Current size before: " << s_lights.size() << std::endl;
		s_lights.push_back(light);
		std::cout << "Adding light. Current size after: " << s_lights.size() << std::endl;
	}
	*/
	void addLight(const std::shared_ptr<Light> light) {
		lights.push_back(light);
	
	}

	void removeLight(const std::shared_ptr<Light> light) {

		lights.erase(std::remove_if(lights.begin(), lights.end(), 
			[&light](const std::weak_ptr<Light>& weak) {
				return weak.lock() == light;
			}), lights.end());
	}


	void clear() {
		//s_lights.clear();
		lights.clear();
	}

	//const std::vector<std::shared_ptr<Light>>& getLights() {
	//std::unordered_map<std::string, std::shared_ptr<Light>> const& getLights() {
	//	return lights;
		//return s_lights;
	//}
	const std::vector<std::shared_ptr<Light>>& getLights() {
		return lights;
	}

	//std::shared_ptr<Light> getLight(const char* name) {
	//	auto it = lights.find(name);
	//	if (it != lights.end()) {
	//		return it->second;
	//	}
	//	return nullptr; // Return nullptr if light not found
	//}
}
