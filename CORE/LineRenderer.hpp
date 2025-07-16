#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include <memory>

class Camera;

namespace LineRenderer {

	void init();

	void draw(glm::vec3 const& start, glm::vec3 const& end, glm::vec4 const& color);

	void render(const std::shared_ptr<Camera> cam);

};