/*#pragma once
#include "PhysicsComponent.hpp"
#include "Camera.hpp"


//this component read the camera direction and cast a ray in that direction
//for that we need to get the camera component
//and the transform component
class RayCastComponent : public PhysicsComponent
{
public:
	RayCastComponent() : PhysicsComponent() {}

	void update(float dt) override {
		updateTransform();
	}

	void init() override {

		m_maxHits = 1;
		m_hitBufferPtr = &m_hitBuffer;
		m_callback = nullptr;
	
		this->getGameObject()->getComponent<Camera>()->getGameObject()->getComponent<Transform>()->getPosition();


	}

private:
	void updateTransform() {
	}	


	//var
	PxRaycastHit m_hitInfo;
	PxU32 m_maxHits;
	PxRaycastBuffer m_hitBuffer;
	PxRaycastBuffer* m_hitBufferPtr;
	PxRaycastCallback* m_callback;

};*/

