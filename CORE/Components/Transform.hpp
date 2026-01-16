#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4

struct TransformComponent {
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;

	TransformComponent(const glm::vec3& pos = glm::vec3(0.0f),
		const glm::quat& rot = glm::quat(0, 0, 0, 0),
		const glm::vec3& scale = glm::vec3(1.0f))
		: pos(pos), rot(rot), scale(scale) {}
	TransformComponent(const TransformComponent&) = default;

	glm::mat4 getViewMatrix() const {
		glm::vec3 forward = rot * glm::vec3(0, 0, -1);  // Forward is -Z in view space
		glm::vec3 up = rot * glm::vec3(0, 1, 0);
		glm::vec3 right = rot * glm::vec3(1, 0, 0);

		glm::vec3 target = pos + forward;

		return glm::lookAt(pos, target, up);
	}

	glm::mat4 toMat4() const { 
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, pos);
		transform = transform * glm::mat4_cast(rot);
	}

};

