#pragma once

#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Physics.hpp"
#include "CORE/Scene.hpp"
#include "CORE/Window/Input.hpp"

#include "CORE/Systems/Input/Camera.hpp"

namespace InputSystem {

	float jumpForce{ 3.0f };
	float movementSpeed{ 4.f };
	float sprintMultiplier{ 2.5f };

	void updateKeyMovements(Scene* scene, float dt);

	void update(Scene* scene, float dt) {
		updateActiveCameraRotation(scene);
		updateKeyMovements(scene, dt);
	}


	void updateKeyMovements(Scene* scene, float dt) {

		//CapsuleMotorComponent* capsMotorComp = m_targetObj->getComponent<CapsuleMotorComponent>().get();
		Entity activeEntity = scene->getActiveEntity();
		CharacterControllerStateComponent* state = scene->getComponent<CharacterControllerStateComponent>(activeEntity);

		TransformComponent* transform = scene->getComponent<TransformComponent>(activeEntity);


		if (!state) {
			LOG_WARN("ActiveEntity must have a CharacterControllerStateComponent");
			return;
		}
		if (!transform) {
			LOG_WARN("ActiveEntity must have a transformComponent");
			return;
		}

		glm::vec3 vel{ state->velocity };
		glm::vec3 dir{ 0,0,0 };

		// choosed to used the camera for direction as first choice
		glm::vec3 forward{ transform->forward() };
		glm::vec3 right{ transform->right() };

		forward.y = 0;
		forward = glm::normalize(forward);

		if (Input::isKeyPressed(GLFW_KEY_W))
			dir += forward;
		if (Input::isKeyPressed(GLFW_KEY_S))
			dir -= forward;
		if (Input::isKeyPressed(GLFW_KEY_A))
			dir -= right;
		if (Input::isKeyPressed(GLFW_KEY_D))
			dir += right;

		state->isSprinting = Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT);

		float actual_speed{ movementSpeed };


		if (state->isSprinting)
			actual_speed *= sprintMultiplier;

		if (dir.x || dir.y || dir.z)
			dir = glm::normalize(dir);

		vel += dir * actual_speed * dt;

		if (Input::isKeyJustPressed(GLFW_KEY_SPACE)) {
			vel.y = jumpForce;
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
			vel.y = -jumpForce;
		}

		state->velocity = vel;
	}
}
