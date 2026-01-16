#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//runtime camera
class Camera {

public:

	Camera(glm::mat4 projection) : projection{ projection } {}
	const glm::mat4& getProjection() const { return projection; }
private:
	glm::mat4 projection{};

};

