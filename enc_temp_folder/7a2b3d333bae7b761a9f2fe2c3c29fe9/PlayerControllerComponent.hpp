#pragma once

#include "CORE/Component.hpp"
#include "CORE/Physics.hpp"
#include <glm/gtx/matrix_decompose.hpp>

class PlayerControllerComponent : public PhysicsComponent {

public:

	PlayerControllerComponent(const PxCapsuleControllerDesc& desc) : m_desc{ desc }, m_playerController{ nullptr } {
		m_playerController = Physics::getControllerManger().createController(desc);
	}
	PlayerControllerComponent() : m_playerController{ nullptr } {}
	void init() override {

		m_desc.height = 2.0f;
		m_desc.radius = 1.f;
		m_desc.position = { 0.0f, 0.0f,0.0f };

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

		body = m_playerController->getActor();
		this->material = material;


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
				m_velocity -= forward * m_actualSpeed * dt;
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

			updateTransform();

			//m_gameObject->move(m_playerController->get);

			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;

		}

	}

	void setPosition(const glm::vec3& worldPosition) override {

		m_playerController->setPosition(PxExtendedVec3(worldPosition.x, worldPosition.y, worldPosition.z));
		//updateTransform();
	}

	void setRotation(const glm::vec3& rotation) override {	} //no need for rotation
	void setRotationQuaternion(const glm::quat& worldRotation) override {} //no need for rotation
	void updateTransform() override {
		if (body and this->m_attachedToGameObject) {
			PxTransform pxTransform = body->getGlobalPose();
			// Update position
			glm::vec3 physxPosition(pxTransform.p.x, pxTransform.p.y, pxTransform.p.z);

			// Convert PhysX quaternion to glm quaternion and normalize
			PxQuat pxQuat = pxTransform.q;
			pxQuat.normalize();
			glm::quat physxRotation(pxQuat.w, pxQuat.x, pxQuat.y, pxQuat.z);

			//transform matrix from physx data
			glm::mat4 physxMatrix = glm::translate(glm::mat4(1.0f), physxPosition) *
				glm::mat4_cast(physxRotation);

			glm::mat4 worldMatrix = physxMatrix * getLocalMatrix();

			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 position;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(worldMatrix, scale, rotation, position, skew, perspective);

			// Update GameObject with quaternion directly

			getGameObject()->setPosition(position, false);
			//getGameObject()->setRotationQuaternion(rotation, false);
		}
	}

	void updateRotation() override {	}//override so 

	void onImGuiRender() override {

		//ImGui::Begin("PlayerControllerComponent");
		ImGui::BeginChild("PlayerControllerComponent");
		TransformableComponent::onImGuiRender();
		ImGui::DragFloat3("velocity", glm::value_ptr(m_velocity));
		//ImGui::DragFloat3("upDirection",  (float&) &m_playerController->getUpDirection());
		ImGui::EndChild();


		//ImGui::End();
	}




	const char* getName() { return "PlayerControllerComponent"; }

	void updatePhysX() override {
		setPosition(getWorldPosition());
		//setRotation(getLocalRotation());
		// m_playerController->setPosition(m_gameObject->getModelMatrix() * getLocalMatrix());
	}

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