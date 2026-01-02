#pragma once

#include "../CameraComponents/Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace UI {
	inline void UICameraRotationController(std::shared_ptr<CameraComponent> cam) {
		if (!cam) {
		///	LOG_WARN("camera is null");
			return;
		}
		
	}
}