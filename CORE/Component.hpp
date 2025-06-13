#pragma once

class GameObject;

class Component
{
public:
	virtual ~Component() = default;
	virtual void init() {};
	virtual void update(float dt) {};

	GameObject* getGameObject() { return m_gameObject; }
	void setGameObject(GameObject* gameObject) { m_gameObject = gameObject; }
protected:

	GameObject* m_gameObject;

}; // class Component
