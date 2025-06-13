#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // This header contains toMat4

class Transform {

public:
	Transform(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
		: m_position(position), m_rotation(rotation), m_scale(scale) {
		updateQuaternionFromEuler();
	}

	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getRotation() const { return m_rotation; }
	const glm::quat& getRotationQuaternion() const { return m_rotationQuat; }
	const glm::vec3& getScale() const { return m_scale; }

	void setPosition(const glm::vec3& position) { m_position = position; }
	void setRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		updateQuaternionFromEuler();
	}
	void setRotationQuaternion(const glm::quat& quaternion) {
		m_rotationQuat = quaternion;
		// Update Euler angles for compatibility
		m_rotation = glm::degrees(glm::eulerAngles(m_rotationQuat));
	}

	void setScale(const glm::vec3& scale) { m_scale = scale; }

	void move(const glm::vec3& offset) { m_position += offset; }
	//void rotate(const glm::vec3& offset) { m_rotation += offset; }

	void rotate(const glm::vec3& offset) {
		m_rotation += offset;
		updateQuaternionFromEuler();
	}
	void scale(const glm::vec3& offset) { m_scale += offset; }

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);

		// Use quaternion for rotation instead of Euler angles
		model *= glm::toMat4(m_rotationQuat);

		model = glm::scale(model, m_scale);
		return model;
	}

protected:

	void updateQuaternionFromEuler() {
		m_rotationQuat = glm::quat(glm::radians(m_rotation));
	}

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::quat m_rotationQuat; // Store rotation as quaternion
	glm::vec3 m_scale;

}; // class Transform