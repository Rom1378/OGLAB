#pragma once

// should be able to handle different kind of rendering,
// cubemaps
// models from files
// light...
// ...
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/matrix.hpp>




class Scene;

namespace Renderer {
	struct RendererSettings {
		bool enableBlinn = true;
		bool enableLighting = true;
	};


	void init();
	void update(float dt);
	void draw();//submit ()receive draw call from components

	void cleanup();

	void render(Scene* scene);


	void rescale_framebuffer(float width, float height);
	void clearFBO();

	GLuint getFBO();
	GLuint getRBO();
	GLuint getColorTextureBuffer();

	void setViewProjection(const glm::mat4& view, const glm::mat4& proj);
	const glm::mat4& getView();
	const glm::mat4& getProjection();
	const glm::vec3& getViewPosition();
	RendererSettings& getRendererSettings();
	
}
