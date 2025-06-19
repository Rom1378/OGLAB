#pragma once

#include "../Cameras/Camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace UI {
	inline void UICameraController(std::shared_ptr<Camera> cam) {
		if (!cam) {
			LOG_WARN("camera is null");
			return;
		}
		
	}
}