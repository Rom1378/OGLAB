#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Scene;

namespace Skybox {
	
	constexpr GLuint INVALID_HANDLE = UINT_MAX;
	struct skyboxHandle {
		skyboxHandle(GLuint handle) : id{ handle } {}
		bool isValid() const { return id != INVALID_HANDLE; }
		GLuint id;
	};

	void init();
	skyboxHandle createSkybox(const std::vector<std::string>& faces);

	void skyboxPass(Scene* scene);


	void cleanup();
}

