#pragma once


#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <array>

class Scene;
class ShaderProgram;


namespace ShadowMapSystem {

	using ShadowMapHandle = uint32_t;
	constexpr ShadowMapHandle INVALID_SHADOWMAP_HANDLE = 0;
	void init();
	void update(Scene& scene);

	void computeShadowMapsPass(Scene& scene);
	//void renderPass(Scene* scene);

	const std::array<glm::mat4, 6>& getLightSpaceMatrix(ShadowMapHandle h);
	GLuint getDepthMapTexture(ShadowMapHandle h);
	void destroyShadowMap(ShadowMapHandle h);

	void bindShadowMaps(ShaderProgram& shader);
	bool hasShadowMap();

	void shutdown();
}