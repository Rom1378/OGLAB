//#include "CORE/Lights/LightManager.hpp"
#include "Engine.hpp"
//#include "CORE/Physics.hpp"
#include "Shader.hpp"
//#include "Prefabs/SomePrefabs.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//#include "UI/SceneObjectEditor.hpp"
#include "Scene.hpp"
//#include "LineRenderer.hpp"
#include "CORE/TextureManager.hpp"

#include "CORE/Systems/Renderer/Renderer.hpp"
#include "CORE/Systems/Physics/Physics.hpp"

#include "CORE/UI/UI.hpp"



#include <chrono>

using namespace std::chrono;
high_resolution_clock::time_point lastTime = high_resolution_clock::now();

namespace Engine {
	bool m_isRunning = false;
	float m_dt = 0;


	float get_dt() { return m_dt; }

	float getDeltaTime() {
		high_resolution_clock::time_point currentTime = high_resolution_clock::now();
		duration<float> deltaTime = duration_cast<duration<float>>(currentTime - lastTime);
		lastTime = currentTime;
		return deltaTime.count(); // Returns time in seconds
	}
	void reset_dt() { getDeltaTime(); }

	void init() {
	//	registerPrefabs();
		Window::WindowProps props;
		props.title = "CLC";
		props.width = 1280;
		props.height = 720;
		Window::init(props);
		LOG("Is VSync: ", Window::isVSync());

		//systems inits
		Input::init();
		ShaderManager::loadConfigs("../../../Config/shaders.json");

		PhysicsSystem::init();

		Renderer::init();

		UI::init();

		Window::setVSync(1);

	}

	void shutdown() {
		//LightManager::shutdown();
		Renderer::cleanup();
		PhysicsSystem::shutdown();


		ShaderManager::cleanup();
		TextureManager::clear();
		Input::shutdown();
		Window::shutdown();
		m_isRunning = false;
		LOG_OK("Engine shutdown complete.");
	}

	void update(Scene* scene) {
		m_dt = getDeltaTime();
		scene->update(m_dt);

		//Window::update(); //swapbuffer
		//Input::update(); //shit + pollenvent
		//Window::clear();

	}

	void renderUI(Scene* scene) {
		UI::render(scene);

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}
	void render(Scene* scene) {
		//std::cout << "nothing rendering right now"<< std::endl;
		Renderer::prerender(scene);
		Renderer::render(scene);
		Renderer::postRender();

		}
	bool isRunning() { return m_isRunning; }
}