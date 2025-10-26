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
#include "../CORE/CameraComponents/CameraMC.hpp"
#include <CORE/Debug.hpp>



//F2 F1
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

	
};