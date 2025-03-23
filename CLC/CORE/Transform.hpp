#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform
{

public:
	Transform() : m_position(0.0f), m_rotation(0.0f), m_scale(1.0f) {}
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : m_position(position), m_rotation(rotation), m_scale(scale) {}

	const glm::vec3& getPosition() const { return m_position; }
	const glm::vec3& getRotation() const { return m_rotation; }
	const glm::vec3& getScale() const { return m_scale; }

	void setPosition(const glm::vec3& position) { m_position = position; }
	void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
	void setScale(const glm::vec3& scale) { m_scale = scale; }

	void move(const glm::vec3& offset) { m_position += offset; }
	void rotate(const glm::vec3& offset) { m_rotation += offset; }
	void scale(const glm::vec3& offset) { m_scale += offset; }

	glm::mat4 getModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, m_scale);
		return model;
	}

protected:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

}; // class Transform