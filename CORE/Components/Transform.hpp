#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4

struct Transform {
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale;

	Transform() = default;
	Transform(const glm::vec3& pos = glm::vec3(0.0f),
		const glm::quat& rot = glm::quat(0, 0, 0, 0),
		const glm::vec3& scl = glm::vec3(1.0f))
		: pos(pos), rot(rot), scale(scl) {}
};

