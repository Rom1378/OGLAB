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

		/*

		// Generate and bind framebuffer
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

		// Create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// Attach depth texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "ERROR::FRAMEBUFFER:: Shadow framebuffer is not complete!" << std::endl;
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			switch (status) {
			case GL_FRAMEBUFFER_UNDEFINED: std::cerr << "GL_FRAMEBUFFER_UNDEFINED" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: std::cerr << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: std::cerr << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl; break;
			}
		}

		// Unbind framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Check for OpenGL errors
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error in LightManager::init: " << err << std::endl;
		}
		*/



	}
	/*
	void compute_shadow_mapping(Scene* scene) {
		if (s_lights.empty()) return;

		float near_plane = 0.001f;
		glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(
			s_lights[0]->getPosition(),
			glm::vec3(0.0f),
			glm::vec3(0.0, 1.0, 0.0)
		);

		storedLightSpaceMatrix = lightProjection * lightView;
		// Render scene to depth map
		auto depthShader = ShaderManager::getShader("simpleDepthShader");
		auto shader = ShaderManager::getShader("standard");
		depthShader->use();
		depthShader->setMat4("lightSpaceMatrix", storedLightSpaceMatrix);

		// 1. Render depth map
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//set depth shader for rendering
		auto& gms = scene->getGameObjects();
		//save shaders
		std::vector<std::shared_ptr<ShaderProgram>> originalShaders;
		for (auto& obj : gms) {
			if (auto renderComp = obj->getComponent<RenderComponent>()) {
				originalShaders.push_back(renderComp->getShader());
				renderComp->setShader(depthShader);
			}
		}
		glCullFace(GL_FRONT);
		scene->render();
		glCullFace(GL_BACK);
		// Restore original shaders
		size_t i = 0;
		for (auto& obj : gms) {
			if (auto renderComp = obj->getComponent<RenderComponent>()) {
				if (i < originalShaders.size()) {
					renderComp->setShader(originalShaders[i++]);
				}
			}
		}

	}
	*/

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
