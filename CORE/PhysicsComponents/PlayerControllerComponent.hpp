#pragma once

#include "CORE/Component.hpp"
#include "CORE/Physics.hpp"

class PlayerControllerComponent : public Component {

public:

	PlayerControllerComponent(const PxCapsuleControllerDesc& desc) : m_desc{ desc }, m_playerController{ nullptr } {
		m_playerController = Physics::getControllerManger().createController(desc);
	}
	PlayerControllerComponent() : m_playerController{ nullptr } 	{}
	void init() override{

		m_desc.height = 2.0f;
		m_desc.radius = 1.f;
		m_desc.position = { 0.0f,310.0f,0.0f };

		PxMaterial* material = Physics::getPhysics()->createMaterial(
			0.5f,  // staticFriction
			0.5f,  // dynamicFriction
			0.1f   // restitution (bounciness)
		);
		m_desc.material = material; // PxMaterial*, e.g. your default
		m_desc.slopeLimit = 0.707f;                   // ~45 degrees
		m_desc.stepOffset = 0.5f;                     // Max step height
		m_desc.contactOffset = 0.1f;
		m_desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
		m_desc.upDirection = { 0.0f,1.0f,0.0f };
		m_playerController = Physics::getControllerManger().createController(m_desc);
	
	}

	~PlayerControllerComponent() {
		m_playerController->release();
	}
	void update(float dt) override {
		if (!m_gameObject)
			return;
		const auto& camera = m_gameObject->getComponent<CameraComponent>();
		//const auto& transform = 
		if (camera and camera->isAttachedToGameObject()) {

			glm::vec3 forward = camera->getForward();
			forward.y = 0;
			forward = glm::normalize(forward);
			glm::vec3 right = camera->getRight();

			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
				m_actualSpeed = m_sprintSpeed;
			else
				m_actualSpeed = m_walkSpeed;

			if (Input::isKeyPressed(GLFW_KEY_W)) {
				m_velocity += forward * m_actualSpeed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_S)) {
				m_velocity -= forward* m_actualSpeed* dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_A)) {
				m_velocity -= right * m_actualSpeed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_D)) {
				m_velocity += right * m_actualSpeed * dt;
			}

			if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
				m_velocity.y = m_jump_force;// *dt;//m_actualSpeed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
				m_velocity.y = -m_jump_force;// *dt;//m_actualSpeed * dt;
			}

			m_velocity.y += -9.81f * dt;

			PxControllerCollisionFlags collisionFlags =
				m_playerController->move(PxVec3(m_velocity.x, m_velocity.y, m_velocity.z), 0.001f, dt, PxControllerFilters());

			if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
				m_velocity.y = 0.0f;

			m_gameObject->setPosition(m_playerController->getPosition());

			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;

		}

	}


	void onImGuiRender() override {

		ImGui::Begin("PlayerControllerComponent");
		ImGui::DragFloat3("velocity", glm::value_ptr(m_velocity));
		//ImGui::DragFloat3("upDirection",  (float&) &m_playerController->getUpDirection());


		ImGui::End();
	}




	const char* getName() { return "PlayerControllerComponent"; }

private:
	const float m_walkSpeed = 3.0f;
	const float m_sprintSpeed = 6.0f;
	const float m_jump_force{ 3.0f };
	float m_actualSpeed = m_walkSpeed;
	glm::vec3 m_velocity{ 0.0f,0.0f,0.0f };
	glm::vec3 speed{ 0.0f,0.0f,0.0f };
	bool m_isSprinting = 0;
	bool m_isWalking{ 0 };

	PxCapsuleControllerDesc m_desc;
	PxController* m_playerController;

};