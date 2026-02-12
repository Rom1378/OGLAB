#pragma once

// should be able to handle different kind of rendering,
// cubemaps
// models from files
// light...
// ...
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/matrix.hpp>

#include <cstdint>



class Scene;
class ShaderProgram;

namespace Renderer {
	struct RendererSettings {
		bool enableBlinn = true;
		bool enableLighting = true;
		bool enableShadows = true;
		bool geometryOnlyPass = false;
	};


	void init();
	void update(Scene& scene, float dt);

	void draw();//submit ()receive draw call from components

	void cleanup();

	void prerender(Scene* scene);
	void render(Scene* scene);
	void postRender(Scene& scene);


	void rescale_framebuffer(float width, float height);
	void clearFBO();


	GLuint getMainFBO();
	GLuint getMainRBO();
	GLuint getColorTextureBuffer();

	void setViewProjection(const glm::mat4& view, const glm::mat4& proj);
	const glm::mat4& getView();
	const glm::mat4& getProjection();
	glm::vec3 getViewPosition();
	RendererSettings& getRendererSettings();

	uint32_t getDrawCallCount();
	void increaseDrawCallCount();
	
}
