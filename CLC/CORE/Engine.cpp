#include "Engine.hpp"
#include "Window.hpp"
#include "Input.hpp"
#include "Physics.hpp"
#include "Shader.hpp"
#include "Prefabs/SomePrefabs.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "UI/SceneObjectEditor.hpp"
#include "Scene.hpp"
#include "LightManager.hpp"

#include <chrono>

using namespace std::chrono;
high_resolution_clock::time_point lastTime = high_resolution_clock::now();


void checkopenglerror() {
	int err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error in Engine::update: " << err << std::endl;
	}
}

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

	void init() {
		registerPrefabs();

		Window::WindowProps props;
		props.title = "CLC";
		props.width = 1280;
		props.height = 720;
		Window::init(props);
		std::cout << Window::isVSync() << std::endl;
		Input::init();
		ShaderManager::loadConfigs("shaders.json");
		Physics::init();
		LightManager::init();

	}

	void shutdown() {
	}

	void update(Scene* scene) {
		Window::update();
		Input::update();
		Window::clear();
		m_dt = getDeltaTime();
		scene->update(m_dt);


		// First compute shadow maps
		//scene->render();
		LightManager::compute_shadow_mapping(scene);

	}

	void renderUI(Scene* scene) {

		// Render UI
		UI::renderImGuiSceneHierarchy(scene);
		UI::renderImGuiObjectEditor();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	}
	void render(Scene* scene) {
		Window::bind_framebuffer();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene with shadows
		//scene->render();

		// Optional: Render debug quad in a separate window
		if (0) {
			auto debugShader = ShaderManager::getShader("debugDepthQuad");
			debugShader->use();
			debugShader->setFloat("near_plane", 1.0f);
			debugShader->setFloat("far_plane", 100.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, LightManager::getDepthMap());
			LightManager::renderQuad();
		}
	}
	bool isRunning() { return m_isRunning; }
}