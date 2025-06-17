#pragma once
#include "../CORE/Scene.hpp"
#include <iostream>
#include <PxPhysicsAPI.h>
#include "../CORE/Engine.hpp"
#include "../CORE/Prefabs/SomePrefabs.hpp"
#include "../CORE/UI/CameraController.hpp"
#include "../CORE/RenderComponents/Cursor.hpp"
#include "../CORE/UI/SceneObjectEditor.hpp"
#include "../CORE/RenderComponents/ModelRenderer.hpp"
#include "../CORE/Cameras/CameraMC.hpp"
#include <CORE/Debug.hpp>


class DevScene : public Scene {
public:

	void init() override;

	void onUpdate() override;
	void onImGuiRender() override;

	private:
	
		std::shared_ptr<Light> sunLight;

		PxRaycastHit hitInfo;
		GameObject* hitRaycastObject = nullptr;// = static_cast<GameObject*>(hitInfo.actor->userData);
		GameObject* selectedHitRaycastObject = nullptr;

		enum mode {
			EDIT, //right click will add a cube
			PLAY  //right click will select the nearest object under the cursor
		} currentMode = EDIT;

};