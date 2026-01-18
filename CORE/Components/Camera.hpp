#pragma once

/*struct CameraComponent {
    float fov;
    float nearPlane;
    float farPlane;

};
*/

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CORE/Camera.hpp"

struct CameraComponent {
	Camera cam;
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const glm::mat4& projection) : cam{ projection } {}
	CameraComponent(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f) :
		cam(glm::perspective(fov, aspectRatio, nearPlane, farPlane)),
		fov{fov}, aspectRatio{aspectRatio}, nearPlane{nearPlane}, farPlane{farPlane}
	{ }
};