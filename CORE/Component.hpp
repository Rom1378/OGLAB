#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


class GameObject;

class Component
{
public:
	virtual ~Component() = default;
	virtual void init() {}
	virtual void update(float dt) {}
	virtual void onImGuiRender();

	GameObject* getGameObject() const { return m_gameObject; }
	void setGameObject(GameObject* gameObject) { m_gameObject = gameObject; }

	virtual const char* getName() { return "Unamed"; }


protected:

	GameObject* m_gameObject;

}; // class Component

