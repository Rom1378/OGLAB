
#include "devScene.hpp"


int main() {

	Engine::init();

	{
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

			Input::resetFrameKeys();

			glfwPollEvents();

			Input::update();


			// Handle input
			if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
				Input::setMouseLocked(false);
			}
			if (Input::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
				Input::setMouseLocked(true);
			}

			Engine::update(&scene); //scene update


			Engine::render(&scene);

			//IMGUI rendering
			Window::drawImGuiInterface(); // ImGUI NEWFRAME + FRAME BUFFER
			Engine::renderUI(&scene);// SCENE UI + IMGUI RENDER
			//END IMGUI rendering

		
			Window::update();
			Window::clear();

			//LightManager::compute_shadow_mapping(&scene);
		}

	}
	Engine::shutdown();
	return 0;
}

