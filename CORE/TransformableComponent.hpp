#pragma once

#include <CORE/Component.hpp>

//component but has a local transform
class TransformableComponent : public Component {
public:

	TransformableComponent() {

	}
	//void update() override;

	void onImGuiRender() override;


	glm::mat4 getLocalMatrix() const;
	glm::mat4 getWorldMatrix() const;
	glm::vec3 getWorldPosition() const;
	glm::vec3 getWorldDirection() const;

	glm::vec3 getLocalRotation() const;
	glm::quat getLocalRotationQuat() const;
	glm::vec3 getLocalPosition() const;

	inline void setPosition(glm::vec3 const& pos) { m_localPosition = pos; }
	inline void setPosition(float x, float y, float z) { m_localPosition = glm::vec3(x, y, z); }
	inline void setRotation(glm::vec3 const& rot) { m_localRotation = rot; }
	inline void setRotation(float x, float y, float z) { m_localRotation = glm::vec3(x, y, z); }
	void setScale(glm::vec3 const& scale) { m_localScale = scale; }
	void setScale(float x, float y, float z) { m_localScale = glm::vec3(x, y, z); }

	const char* getName() override { return "TransformableComponent"; }

	inline bool isAttachedToGameObject() const { return m_attachedToGameObject; }
	inline void setAttachedToGameObject(bool attach) { m_attachedToGameObject = attach; }

protected:

	glm::vec3 m_localPosition = { 0, 0, 0 };
	glm::vec3 m_localRotation = { 0, 0, 0 }; // Euler
	glm::vec3 m_localScale = { 1, 1, 1 };

	bool m_attachedToGameObject{ true };

};
