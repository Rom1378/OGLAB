#include "CORE/Scene.hpp"
#include "CORE/Components/Physics.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Window/Input.hpp"


namespace InputSystem {

	struct _MouseParams {
		float sensibilityX{ 1.0f };
		float sensibilityY{ 1.0f };
	}mouseParams;


	void updateActiveCameraRotation(Scene* scene) {
		Entity camEtt = scene->getActiveCamera();
		TransformComponent* transform = scene->getComponent<TransformComponent>(camEtt);
		CharacterControllerStateComponent* charController = scene->getComponent<CharacterControllerStateComponent>(camEtt);

		if (!charController || !transform) {
			LOG_WARN("Camera must have KinematicBody and a Transform component");
			return;
		}

		glm::vec2 mouseDelta = Input::getMouseDelta();

		if (mouseDelta.x or mouseDelta.y) {
			float xoffset = mouseDelta.x * mouseParams.sensibilityX;
			float yoffset = mouseDelta.y * mouseParams.sensibilityY;

			charController->yaw += xoffset;
			charController->pitch += yoffset;

			if (charController->pitch > 89.0f) charController->pitch = 89.0f;
			if (charController->pitch < -89.0f) charController->pitch = -89.0f;

		}

	}


	void setCameraSensibilityX(float x) { mouseParams.sensibilityX = x; }
	void setCameraSensibilityY(float y) { mouseParams.sensibilityY = y; }
	float getCameraSensibilityY() { return mouseParams.sensibilityX; }
	float getCameraSensibilityX() { return mouseParams.sensibilityY; }


}