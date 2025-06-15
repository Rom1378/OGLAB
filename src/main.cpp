
#include "devScene.hpp"


int main() {

	Engine::init();

	// Create scene
	DevScene scene;
	scene.init();


	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LESS);

	//polygon mode


	// Create the framebuffer
	Window::CreateFramebuffer(1280, 720); // Initialize with proper size
	while (Window::isOpen()) {
		// Handle input
		if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
			Input::setMouseLocked(false);
		}
		if (Input::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
			Input::setMouseLocked(true);
		}


		Window::drawImGuiInterface();

		

		Engine::renderUI(&scene);
		Engine::update(&scene);

		//LightManager::compute_shadow_mapping(&scene);
		Engine::render(&scene);
	}

	Engine::shutdown();
	return 0;
}

