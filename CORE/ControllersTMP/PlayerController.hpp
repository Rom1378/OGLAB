/*#pragma once

#include "CORE/Controllers/Controller.hpp"
#include "CORE/CameraComponents/Camera.hpp"
#include "CORE/PhysicsComponents/CapsuleMotorComponent.hpp"



class PlayerController : public Controller {
public:
	PlayerController(std::shared_ptr<GameObject> go = nullptr) : Controller(), m_targetObj(go) {	}

	virtual ~PlayerController() override {};

	void update(float dt) override {
		if (!m_targetObj || !Input::isMouseLocked())
			return;

		CapsuleMotorComponent* capsMotorComp = m_targetObj->getComponent<CapsuleMotorComponent>().get();

		if (!capsMotorComp) {
			LOG_WARN("PlayerController: m_targetObj has no CapsuleMotorComponent");
			return;
		}

		glm::vec3 vel{};
		glm::vec3 dir{ 0,0,0 };

		// choosed to used the camera for direction as first choice
		glm::vec3 forward{};
		glm::vec3 right{};
		const auto& camComp = m_targetObj->getComponent<CameraComponent>();
		if (camComp) { // If has cam also update camera rotation.
			forward = camComp->getForward();
			right = camComp->getRight();
		}
		else {
			forward = glm::normalize(m_targetObj->getRotation() * glm::vec3(0, 0, -1));
			right = glm::normalize(m_targetObj->getRotation() * glm::vec3(1, 0, 0));
		}

		if (Input::isKeyPressed(GLFW_KEY_W))
			dir += forward;
		if (Input::isKeyPressed(GLFW_KEY_S))
			dir -= forward;
		if (Input::isKeyPressed(GLFW_KEY_A))
			dir -= right;
		if (Input::isKeyPressed(GLFW_KEY_D))
			dir += right;


		forward.y = 0;
		forward = glm::normalize(forward);

		float actual_speed{};
		float jump_force = capsMotorComp->get_jumpForce();


		capsMotorComp->set_isSprinting(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT));
		if (capsMotorComp->is_sprinting()) {
			actual_speed = capsMotorComp->get_sprintSpeed();
		}
		else
			actual_speed = capsMotorComp->get_walkSpeed();

		if (dir.x || dir.y || dir.z)
			dir = glm::normalize(dir);
		vel = dir * actual_speed * dt;

		if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
			vel.y = jump_force;
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
			vel.y = -jump_force;
		}

		//maybe not the right place 
		vel.y += -9.81f * dt;

		capsMotorComp->setVelocity(vel);

	}

	std::shared_ptr<GameObject> get_target_object() const { return m_targetObj; }
	void set_target_object(std::shared_ptr<GameObject> go) { m_targetObj = go; }

private:
	std::shared_ptr<GameObject> m_targetObj;
};*/
