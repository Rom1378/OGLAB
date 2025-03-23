#pragma once
#include <glm/glm.hpp>

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

private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

}; // class Transform