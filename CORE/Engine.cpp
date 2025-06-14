#include "Engine.hpp"
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
//#include "LightManager.hpp"

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

	void init() {
		registerPrefabs();

		Window::WindowProps props;
		props.title = "CLC";
		props.width = 1280;
		props.height = 720;
		Window::init(props);
		std::cout << Window::isVSync() << std::endl;
		Input::init();
		ShaderManager::loadConfigs("../../../Config/shaders.json");
		Physics::init();
		LightManager::init();

	}

	void shutdown() {
		LightManager::shutdown();
		Physics::shutdown();
		ShaderManager::cleanup();
		TextureManager::clear();
		Input::shutdown();
		Window::shutdown();
		m_isRunning = false;
		std::cout << "Engine shutdown complete." << std::endl;
	}

	void update(Scene* scene) {
		Window::update();
		Input::update();
		Window::clear();
		m_dt = getDeltaTime();
		scene->update(m_dt);

	}

	void renderFrame(Scene* scene, LightManager::ShadowMapper* shadowMapper) {
		// 1. Shadow Pass
		shadowMapper->renderShadowPass(scene, LightManager::getLights()[0]);

		// 2. Main Pass
		Window::bind_framebuffer();
		Window::update_viewport();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind shadow map to shader
		//auto mainShader = ShaderManager::getShader("standard");
		//mainShader->use();
		//mainShader->setInt("shadowMap", 1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, shadowMapper->getDepthMapTexture());
		//mainShader->setMat4("lightSpaceMatrix", shadowMapper->getLightSpaceMatrix());

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
		renderFrame(scene, LightManager::getShadowMapper());
		// Render scene normally
		scene->renderMainPass();

		// Optional: Render debug quad in a separate window
		if (0) {
			auto debugShader = ShaderManager::getShader("debugDepthQuad");
			debugShader->use();
			debugShader->setFloat("near_plane", 1.0f);
			debugShader->setFloat("far_plane", 100.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
			//LightManager::renderQuad();
		}
	}
	bool isRunning() { return m_isRunning; }
}