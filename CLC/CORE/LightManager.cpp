#include "LightManager.hpp"
#include <iostream>
#include "Camera.hpp"
#include "Shader.hpp"

namespace LightManager
{

	std::vector<std::shared_ptr<Light>> s_lights;




	unsigned int depthMapFBO;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMap;


	void init() {
		glGenFramebuffers(1, &depthMapFBO);

		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void compute_shadow_mapping(Scene* scene) {
		// 1. first render to depth map
		static auto simpleDepthShader = ShaderManager::getShader("simpleDepthShader");
		static float near_plane = 1.0f, far_plane = 7.5f;
		static auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		simpleDepthShader->use();

		glm::mat4 lightView;

		lightView = glm::lookAt(
			s_lights[0]->getPosition(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);


		//
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		simpleDepthShader->setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);
		scene->render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
