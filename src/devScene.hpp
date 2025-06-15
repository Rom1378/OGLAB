#include "../CORE/Scene.hpp"
#include "../CORE/Shader.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../CORE/Engine.hpp"
#include "../CORE/Prefabs/SomePrefabs.hpp"
#include "../CORE/UI/CameraController.hpp"
#include "../CORE/RenderComponents/Cursor.hpp"
#include "../CORE/UI/SceneObjectEditor.hpp"
#include "../CORE/RenderComponents/ModelRenderer.hpp"
#include "../CORE/Cameras/CameraMC.hpp"


class DevScene : public Scene {
public:

	void init() override;

	void onUpdate() override;
	void onImGuiRender() override;

	private:
	
		std::shared_ptr<Light> sunLight;

};