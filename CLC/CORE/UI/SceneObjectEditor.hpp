#pragma once
#include "../GameObject.hpp"
#include "../Scene.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <algorithm>

namespace UI {

	void renderImGuiSceneHierarchy(Scene* scene);
	void renderImGuiObjectEditor();
	void setRaycastSelectedObject(GameObject* obj);
	void handleRaycastSelection(GameObject* hitObject);

}