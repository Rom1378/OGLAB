#include "UI.hpp"

#include "CORE/UI/Entity.hpp"
#include "CORE/UI/Physics.hpp"
#include "CORE/UI/RendererSettings.hpp"

namespace UI {
	void init() {
		initEntityTreeBrowser();
	}

	void render(Scene* scene) {
		DrawEntityTree(scene);
		imgui_physics_menu();
		postProcessingMenu();
	}
}