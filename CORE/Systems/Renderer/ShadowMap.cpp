//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
#include "ShadowMap.hpp"
#include "CORE/Scene.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"
#include "CORE/Systems/Renderer/MeshRenderer.hpp"
#include "CORE/ShaderProgram.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Renderable.hpp"
#include "CORE/Components/Light.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <unordered_map>
#include <array>

namespace ShadowMapSystem {
	struct ShadowMap {
		LightComponent::Type lightType{};

		GLuint depthMapFBO{};
		GLuint depthTexture; // 2D or cubemap

		std::array<glm::mat4, 6> lightSpaces; // use lightSpaces[0] for dir and spot, everthing for point
	};

	constexpr uint32_t MAX_SHADOWMAPS = 8;

	std::unordered_map<ShadowMapHandle, ShadowMap> shadowMaps;
	ShadowMapHandle nextShadowMapHandle = 1;


	static void createShadowMap(GLuint& depthMapFBO, GLuint& depthMap, uint32_t resolution, LightComponent::Type type) {
		//FBO init
		glGenFramebuffers(1, &depthMapFBO);
		//create depth texture
		glGenTextures(1, &depthMap);
		if (type == LightComponent::Type::DIR || type == LightComponent::Type::SPOT) {

			glBindTexture(GL_TEXTURE_2D, depthMap);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			//attach depth texture as FBO's depth buffer
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		}
		else if (type == LightComponent::Type::POINT) {
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_DEPTH_COMPONENT, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
		}

		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("Shadow framebuffer incomplete");


		glBindTexture(GL_TEXTURE_2D, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//A framebuffer object however is not complete without a color buffer so we need to 
		//explicitly tell OpenGL we're not going to render any color data. We do this by setting 
		// both the read and draw buffer to GL_NONE with glDrawBuffer and glReadbuffer. 
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void update(Scene& scene) {
		auto LightsView = scene.getComponentView<ShadowCasterComponent>();
		for (auto [e, shadowCaster] : LightsView) {
			if (shadowCaster->shadowMapHandle == INVALID_SHADOWMAP_HANDLE) {
				if (shadowMaps.size() >= MAX_SHADOWMAPS) {
					LOG_WARN("Maximum number of shadow maps reached");
					continue;
				}
				auto lightComponent = scene.getComponent<LightComponent>(e);
				if (!lightComponent) {
					LOG_WARN("Shadow caster entity has no light component");
					continue;
				}


				ShadowMap shadowMap{};
				shadowMap.lightType = lightComponent->type;
				createShadowMap(shadowMap.depthMapFBO, shadowMap.depthTexture, shadowCaster->shadowMapResolution, lightComponent->type);
				shadowMaps[nextShadowMapHandle] = shadowMap;

				shadowCaster->shadowMapHandle = nextShadowMapHandle++;

			}
		}
	}


	static ShadowMap* getShadowMap(ShadowMapHandle h) {
		auto it = shadowMaps.find(h);
		if (it == shadowMaps.end()) {
			LOG_WARN("Invalid shadow map handle");
			return nullptr;
		}
		return &it->second;
	}

	const std::array<glm::mat4, 6>& getLightSpaceMatrix(ShadowMapHandle h) {
		auto it = shadowMaps.find(h);
		if (it == shadowMaps.end()) {
			LOG_WARN("Invalid shadow map handle");
			return { glm::mat4(1.0f) };
		}
		return it->second.lightSpaces;
	}

	GLuint getDepthMapTexture(ShadowMapHandle h) {
		auto it = shadowMaps.find(h);
		if (it == shadowMaps.end()) {
			LOG_WARN("Invalid shadow map handle");
			return 0;
		}
		return it->second.depthTexture;
	}


	void initializeShaderSamplers(ShaderProgram& program) {
		program.use();

		// Initialize ALL array elements for 2D shadow maps (units 3-10)
		for (int i = 0; i < MAX_SHADOWMAPS; i++) {
			program.setInt("shadowMaps2D[" + std::to_string(i) + "]", 3 + i);
		}

		// Initialize ALL array elements for cube shadow maps (units 11-18)
		for (int i = 0; i < MAX_SHADOWMAPS; i++) {
			program.setInt("shadowMapsCube[" + std::to_string(i) + "]", 11 + i);
		}
	}

	// Call this ONCE after shader compilation, e.g., in init():
	void init() {
		ShaderProgram* shader = &*ShaderManager::getShader("standard");
		if (shader) {
			initializeShaderSamplers(*shader);
		}
	}

	static void computeLightMatrix(const Transform& WorldTransform, ShadowMap* sm) {
		if (sm->lightType == LightComponent::Type::DIR) {
			glm::vec3 lightDir = glm::normalize(-WorldTransform.forward());
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			
			glm::vec3 center = glm::vec3(0.0f); // later: camera focus

			glm::vec3 lightPos =center  -lightDir * 100.0f; // Position the light far away in the direction opposite to its direction
			glm::mat4 lightView = glm::lookAt(lightPos, center, up);

			glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 1.0f, 200.0f);

			sm->lightSpaces[0] = lightProjection * lightView;
		}
		else if (sm->lightType == LightComponent::Type::SPOT) {
			glm::vec3 lightPos = WorldTransform.pos;
			glm::vec3 lightDir = glm::normalize(WorldTransform.forward());
			glm::vec3 up = WorldTransform.up();
			glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, up);
			glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 100.0f);
			sm->lightSpaces[0] = lightProjection * lightView;
		}
		else if (sm->lightType == LightComponent::Type::POINT) {
			glm::vec3 lightPos = WorldTransform.pos;
			float aspect = 1.0f;
			float near = 1.0f;
			float far = 100.0f;
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
			sm->lightSpaces[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			sm->lightSpaces[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			sm->lightSpaces[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			sm->lightSpaces[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
			sm->lightSpaces[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
			sm->lightSpaces[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		}
	}


	static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
	{
		const auto inv = glm::inverse(proj * view);

		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					const glm::vec4 pt =
						inv * glm::vec4(
							2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}

		return frustumCorners;
	}

	void computeShadowMapsPass(Scene& scene) {

		auto shadowCasterView = scene.getComponentView<ShadowCasterComponent>();

		ShaderProgram* depthShaderCube = &*ShaderManager::getShader("simpleDepthShader");
		ShaderProgram* depthShader2D = &*ShaderManager::getShader("simpleDepthShader2D");

		if (!depthShaderCube || !depthShader2D) {
			LOG_WARN("Shadow shaders not found");
			return;
		}

		GLint cullFaceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &cullFaceMode);

		glCullFace(GL_FRONT);


		for (auto [e, shadowCaster] : shadowCasterView) {
			ShadowMap* sm = getShadowMap(shadowCaster->shadowMapHandle);
			if (!sm) {
				LOG_WARN("ShadowMap is null");
				continue;
			}
			auto lightComponent = scene.getComponent<LightComponent>(e);
			if (!lightComponent) {
				LOG_WARN("Shadow caster entity has no light component");
				continue;
			}
			TransformComponent* transform = scene.getComponent<TransformComponent>(e);
			if (!transform) {
				LOG_WARN("Shadow caster entity has no transform component");
				continue;
			}
			
			glViewport(0, 0, shadowCaster->shadowMapResolution, shadowCaster->shadowMapResolution);
			glBindFramebuffer(GL_FRAMEBUFFER, sm->depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			Transform worldTransform = *transform * lightComponent->localTransform;

			computeLightMatrix(worldTransform, sm);

			ShaderProgram* activeShader = nullptr;

			//choose shader based on light type
			if (sm->lightType == LightComponent::Type::POINT) {
				activeShader = depthShaderCube;
				activeShader->use();

				// Pass 6 matrices for cubemap faces
				for (uint32_t i = 0; i < 6; i++) {
					activeShader->setMat4("lightSpaceMatrix[" + std::to_string(i) + "]", glm::value_ptr(sm->lightSpaces[i]));
				}
				activeShader->setVec3("lightPos", worldTransform.pos);
				activeShader->setFloat("far_plane", shadowCaster->far_plane);
			}
			else {
				// DIR or SPOT lights
				activeShader = depthShader2D;
				activeShader->use();

				// Pass single lightSpaceMatrix
				activeShader->setMat4("lightSpaceMatrix", glm::value_ptr(sm->lightSpaces[0]));
			}

			//now render scene from light's point of view
			auto renderableComponentsView = scene.getComponentView<RenderableComponent>();

			for (auto [e, renderableComponent] : renderableComponentsView) {
				TransformComponent* Etransform = scene.getComponent<TransformComponent>(e);
				if (!Etransform)
					continue;

				Transform worldTransform = *Etransform * renderableComponent->localTransform;
				const MeshRenderer::RenderableData& rdata = MeshRenderer::getRenderableData(renderableComponent->rhandle);

				for (uint32_t i = 0; i < rdata.meshdatas.size(); i++) {
					const MeshRenderer::MeshData& mesh = rdata.meshdatas[i];
					activeShader->setMat4("model", glm::value_ptr(worldTransform.getModelMatrix()));

					glBindVertexArray(mesh.vao);
					glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// RESTAURER l'état du culling original
		glCullFace(cullFaceMode);
	}

	//renderers call this to bind shadow maps before rendering
	void bindShadowMaps(ShaderProgram& program) {
		uint32_t texture2DOffset = 0;
		uint32_t textureCubeOffset = 0;

		// Only bind the active shadow maps to their pre-assigned units
		for (auto& [handle, sm] : shadowMaps) {
			if (sm.lightType == LightComponent::Type::DIR || sm.lightType == LightComponent::Type::SPOT) {
				if (texture2DOffset >= MAX_SHADOWMAPS / 2) {
					LOG_WARN("Too many 2D shadow maps");
					break;
				}

				glActiveTexture(GL_TEXTURE3 + texture2DOffset);
				glBindTexture(GL_TEXTURE_2D, sm.depthTexture);
				// Sampler uniform already set in initializeShaderSamplers
				program.setMat4("lightSpaceMatrices[" + std::to_string(texture2DOffset) + "]", glm::value_ptr(sm.lightSpaces[0]));
				texture2DOffset++;
			}
			else if (sm.lightType == LightComponent::Type::POINT) {
				if (textureCubeOffset >= MAX_SHADOWMAPS / 2) {
					LOG_WARN("Too many cube shadow maps");
					break;
				}

				glActiveTexture(GL_TEXTURE11 + textureCubeOffset);
				glBindTexture(GL_TEXTURE_CUBE_MAP, sm.depthTexture);
				// Sampler uniform already set in initializeShaderSamplers
				textureCubeOffset++;
			}
		}

		program.setInt("num2DShadowMaps", texture2DOffset);
		program.setInt("numCubeShadowMaps", textureCubeOffset);
	}

	void shutdown() {
		for (auto& [handle, sm] : shadowMaps) {
			glDeleteFramebuffers(1, &sm.depthMapFBO);
			glDeleteTextures(1, &sm.depthTexture);
		}
	}

	bool hasShadowMap() {
		return !shadowMaps.empty();
	}

}