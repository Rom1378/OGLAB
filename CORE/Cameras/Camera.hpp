#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Component.hpp"
#include "../Window/Input.hpp"
#include <iostream>
#include "../Transform.hpp"
#include "../Window/Window.hpp"


class Camera : public Transform, public Component {
public:
	Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);


	void update(float dt) override;
	// Camera control methods
	void setPosition(const glm::vec3& pos);
	void setRotation(const glm::vec3& rot);
	void lookAt(const glm::vec3& target);
	void setRotation(float pitch, float yaw, float roll);

	// Camera properties
	const glm::mat4& getViewMatrix() const { return view; }
	const glm::mat4& getProjectionMatrix() const { return projection; }

	//gets
	const glm::vec3& getForward() const { return m_forward; }
	const glm::vec3& getRight() const { return m_right; }
	const glm::vec3& getUp() const { return m_up; }
	float getAspectRatio() const { return aspectRatio; }
	void setAspectRatio(float ratio);
	void setFOV(float f);
	void setNearPlane(float near);
	void setFarPlane(float far);
	float getFOV() const { return fov; }
	float getNearPlane() const { return nearPlane; }
	float getFarPlane() const { return farPlane; }
	void updateView();
	float getMaxLightDistance() const { return maxLightDistance; }
	void setMaxLightDistance(float distance) { maxLightDistance = distance; }


protected:
	glm::vec3 m_forward{ 0.0f, 0.0f, -1.0f }; // Forward direction
	glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };   // Right direction
	glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };      // Up direction
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };

	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	float maxLightDistance;

	void updateProjection() {
		projection = glm::perspective(glm::radians(fov), aspectRatio,
			nearPlane, farPlane);
	}
};


