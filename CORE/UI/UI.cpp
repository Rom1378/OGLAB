#include "UI.hpp"

#include "Entity.hpp"

namespace UI {
	void init() {
		initEntityTreeBrowser();
	}

	void render(Scene* scene) {
		DrawEntityTree(scene);
	}
}