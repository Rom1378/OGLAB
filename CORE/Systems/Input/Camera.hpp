#pragma once

#include "CORE/Components/Camera.hpp"

class Scene;
namespace InputSystem {

	void updateActiveCameraRotation(Scene* scene);

	void setCameraSensibilityX(float x);
	void setCameraSensibilityY(float y);
	float getCameraSensibilityX();
	float getCameraSensibilityY();


}