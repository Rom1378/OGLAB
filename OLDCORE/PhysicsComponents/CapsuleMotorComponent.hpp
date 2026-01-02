#pragma once

#include "CORE/Component.hpp"
#include "CORE/Physics.hpp"
#include <glm/gtx/matrix_decompose.hpp>

class CapsuleMotorComponent: public PhysicsComponent {

public:

	CapsuleMotorComponent(const PxCapsuleControllerDesc& desc) : m_desc{ desc }, m_playerController{ nullptr } {
		m_playerController = Physics::getControllerManger().createController(desc);
	}
	
	CapsuleMotorComponent() : m_playerController{ nullptr }, m_velocity{ 0 } {
		std::cout << " CONTRUCTOR CALLED " << std::endl;
	}
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

	~CapsuleMotorComponent() {
		m_playerController->release();
	}

	void setVelocity(glm::vec3 vel) { 
		this->m_velocity = vel;

	}

	glm::vec3 getVelocity() const { return m_velocity; }
	
	void update(float dt) override {

		//return;

		if (!m_gameObject)
			return;

			PxControllerCollisionFlags collisionFlags =
				m_playerController->move(PxVec3(this->m_velocity.x, this->m_velocity.y, this->m_velocity.z), 0.001f, dt, PxControllerFilters());

			if (collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
				m_velocity.y = 0.0f;

	}

	void setPosition(const glm::vec3& worldPosition) override {

		m_playerController->setPosition(PxExtendedVec3(worldPosition.x, worldPosition.y, worldPosition.z));
		//updateTransform();
	}

	void setRotation(const glm::vec3& rotation) override {	} //no need for rotation
	void setRotationQuaternion(const glm::quat& worldRotation) override {} //no need for rotation

	// update GameObject position based on the physx fetched results
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

			getGameObject().setPosition(position, false);
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

	float get_walkSpeed() const { return m_walkSpeed; }
	float get_sprintSpeed() const { return m_sprintSpeed; }
	float get_jumpForce() const { return m_jump_force; }

	bool is_sprinting() const { return m_isSprinting; }
	void set_isSprinting(bool v) { m_isSprinting = v; }

private:
	const float m_walkSpeed = 23.0f;
	const float m_sprintSpeed = 46.0f;
	const float m_jump_force{ 3.0f };
	glm::vec3 m_velocity{ 0.0f,0.0f,0.0f };
	glm::vec3 speed{ 0.0f,0.0f,0.0f };
	bool m_isSprinting = 0;

	PxCapsuleControllerDesc m_desc;
	PxController* m_playerController;

};