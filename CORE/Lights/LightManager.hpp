#pragma once
#include "../ShaderProgram.hpp"
#include "Light.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include "../Scene.hpp"
#include <memory>

class Camera;

namespace LightManager {

	class ShadowMapper {
		GLuint depthMapFBO;
		GLuint depthMapTexture;
		glm::mat4 lightSpaceMatrix;
		int resolution;
		float farPlane = 900.0f; // Increased for larger scenes
		float nearPlane = 0.001f; // Adjust based on your scene size
		float orthoSize = 100.0f; // Adjust based on your scene size

	public:



		float getFarPlane() {
			return farPlane;
		}
		void setFarPlane(float farPlane) {
			this->farPlane = farPlane;
		}
		float getNearPlane() {
			return nearPlane;
		}
		void setNearPlane(float nearPlane)
		{
			this->nearPlane = nearPlane;
		}
		float getOrthoSize()
		{
			return orthoSize;
		}
		void setOrthoSize(float orthoSize) {
			this->orthoSize = orthoSize;
		}

		glm::mat4 getLightSpaceMatrix() {
			return lightSpaceMatrix;
		}

		ShadowMapper() : depthMapFBO(0), depthMapTexture(0), resolution(2048) {}

		~ShadowMapper() {
			if (depthMapFBO) {
				glDeleteFramebuffers(1, &depthMapFBO);
			}
			if (depthMapTexture) {
				glDeleteTextures(1, &depthMapTexture);
			}
		}
		GLuint getDepthMapFBO() const { return depthMapFBO; }
		GLuint getDepthMapTexture() const { return depthMapTexture; }
		glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }

		void initialize(int resolution = 2048) {
			// Create depth texture
			glGenTextures(1, &depthMapTexture);
			glBindTexture(GL_TEXTURE_2D, depthMapTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			resolution = resolution;

			// Set texture parameters (important for shadows)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


			// Create FBO
			glGenFramebuffers(1, &depthMapFBO);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
			glDrawBuffer(GL_NONE); // No color buffer
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void renderShadowPass(Scene* scene, const std::shared_ptr<Light> light) {
			// 1. Configure render target
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glViewport(0, 0, resolution, resolution);
			glClear(GL_DEPTH_BUFFER_BIT);

			// 2. Calculate light matrices
			glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
			glm::mat4 lightView = glm::lookAt(light->getPosition(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
			lightSpaceMatrix = lightProjection * lightView;

			// 3. Render all shadow casters
			glCullFace(GL_FRONT);
			scene->renderShadowCasters(lightSpaceMatrix);
			glCullFace(GL_BACK);

			// 4. Reset state
			Window::bind_framebuffer();
		}

	};

	extern std::vector<std::shared_ptr<Light>> s_lights;


	void init();
	void shutdown();


	//void compute_shadow_mapping(Scene* scene);
	void bindShadowMap(std::shared_ptr<ShaderProgram> shader);

	//unsigned int getDepthMap();
	ShadowMapper* getShadowMapper();



	std::vector<std::shared_ptr<Light>> getRelevantLights(const std::shared_ptr<Camera> cam, int maxLights);
	void addLight(const std::shared_ptr<Light> light); //TODO: added map and names to light so you can ask lightmanager for the light you want
	//std::shared_ptr getLight(const char * name...
	void clearLights();
	const std::vector<std::shared_ptr<Light>>& getLights();


}