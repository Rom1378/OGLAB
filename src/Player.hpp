#pragma once
#include "CORE/GameObject.hpp"
#include "CORE/Window/Input.hpp"
#include "CORE/Physics.hpp"

class Player : public GameObject {

public:
	Player(const char* name) : GameObject(name) {

		auto& cm = Physics::getControllerManger();

		desc.height = 20.0f;
		desc.radius = 5.0f;
		desc.position = { 0.0f,0.0f,0.0f };

		PxMaterial* material = Physics::getPhysics()->createMaterial(
			0.5f,  // staticFriction
			0.5f,  // dynamicFriction
			0.1f   // restitution (bounciness)
		);
		desc.material = material; // PxMaterial*, e.g. your default
		desc.slopeLimit = 0.707f;                   // ~45 degrees
		desc.stepOffset = 0.5f;                     // Max step height
		desc.contactOffset = 0.1f;
		desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
		desc.upDirection = { 0.0f,0.0f,1.0f };
		m_playerController = cm.createController(desc);
		
		



		//m_playerBody = addComponent<CubePhysics>(CubePhysics::Type::DYNAMIC);
		//need to lock rotation

		//m_playerBody->LockRotationZ(1);
		//m_playerBody->LockRotationX(1);
		//m_playerBody->LockRotationY(1);
		//m_playerBody->setMass(m_mass);
	//	m_playerBody->getActor()->setMat

		m_camera = addComponent<CameraMC>(45, 1280.0f / 720.0f, 0.1f, 100000.0f);
		m_camera->setPosition(glm::vec3(5.0f, 110.0f, 0.0f));
		m_camera->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));


	}


	void onImGuiRender() override {
		ImGui::DragFloat("velocityx", &m_velocity.x, -10,10);
		ImGui::DragFloat("velocityy", &m_velocity.y, -10,10);
		ImGui::DragFloat("velocityz", &m_velocity.z, -10,10);
	}

	void onUpdate(float dt) override {
		// Process keyboard input for movement (only when mouse is locked)
		if (1 or Input::isMouseLocked()) {
			if (auto cam = getComponent<Camera>()) {

				// apply_forces(dt, *cam);

			
				glm::vec3 forward = cam->getForward();
				forward.y = 0;
				glm::vec3 right = cam->getRight();



				if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
					m_actualSpeed = m_sprintSpeed;
				else
					m_actualSpeed = m_walkSpeed;

				if (Input::isKeyPressed(GLFW_KEY_W)) {
					m_velocity += forward * m_actualSpeed;// *dt;

				}
				if (Input::isKeyPressed(GLFW_KEY_S)) {
					m_velocity -= forward * m_actualSpeed; // *dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_A)) {
					m_velocity -= right * m_actualSpeed; // *dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_D)) {
					m_velocity += right * m_actualSpeed;// *dt;
				}
				if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
					m_velocity.y += m_jump_force;// *dt;//m_actualSpeed * dt;
				}
				if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
					m_velocity.y -= m_jump_force;// *dt;//m_actualSpeed * dt;
				}

				//TransformableComponent::setPosition(pos);
				//this->getGameObject()->setPosition(pos);
			//	m_playerBody->applyForce(m_velocity);

				PxControllerCollisionFlags collisionFlags = m_playerController->move(PxVec3(m_velocity.x, m_velocity.y, m_velocity.z),
					0.0f, dt, 0, 0);

				setPosition(m_playerController->getPosition());

				m_velocity = { 0.0f,-0.081f,0.0f };



			}
		}
	}

private:
	//should use physx, try to just input forces 

	void apply_forces(float dt, const Camera& cam) {
		if (m_playerBody) {

			


		}
	}


	const float m_walkSpeed = 2.0f;
	const float m_sprintSpeed = 4.0f;
	const float m_jump_force{ 10.0f };

	float m_actualSpeed = m_walkSpeed;
	glm::vec3 m_velocity{ 0.0f,0.0f,0.0f };
	glm::vec3 speed{ 0.0f,0.0f,0.0f };

	float m_mass{ 85.0f };
	float m_health{ 100.0f };
	bool m_isSprinting = 0;
	bool m_isWalking{ 0 };

	std::shared_ptr<CubePhysics> m_playerBody;
	std::shared_ptr<Camera> m_camera;


	PxCapsuleControllerDesc desc{};
	PxController* m_playerController{ nullptr };

};