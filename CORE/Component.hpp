#pragma once

#include "Debug.hpp"
#include <glm/glm.hpp>

class GameObject;

class Component
{
public:
	virtual ~Component() = default;
	virtual void init() {}
	virtual void update(float dt) {}
	virtual void onImGuiRender();



	GameObject* getGameObject() { return m_gameObject; }
	void setGameObject(GameObject* gameObject) { m_gameObject = gameObject; }


protected:

	GameObject* m_gameObject;

}; // class Component


//component but has a local transform
class TransformableComponent : public Component {
public:

	//void update() override;

	glm::mat4 getLocalMatrix() const;
	glm::mat4 getWorldMatrix() const;
	glm::vec3 getWorldPosition() const;
	glm::vec3 getWorldDirection() const;

	void setPosition(glm::vec3 const& pos) { m_localPosition = pos; }
	void setRotation(glm::vec3 const& rot) { m_localRotation = rot; }
	void setScale(glm::vec3 const& scale) {  m_localScale = scale; }
private:

	glm::vec3 m_localPosition = { 0, 0, 0 };
	glm::vec3 m_localRotation = { 0, 0, 0 }; // Euler
	glm::vec3 m_localScale = { 1, 1, 1 };
};

