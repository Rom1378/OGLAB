#pragma once
#include "CORE/Transform.hpp"



//TransformComponent used for Entity transform
//Transform used for general purpose transform storage, most often as local transform
struct TransformComponent : public Transform {
	TransformComponent(const glm::vec3& pos = glm::vec3(0.0f),
		const glm::quat& rot = glm::quat(1, 0, 0, 0),
		const glm::vec3& scale = glm::vec3(1.0f))
		: Transform(pos, rot, scale) {}
	TransformComponent(const TransformComponent&) = default;
};


