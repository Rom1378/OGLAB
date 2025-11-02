#pragma once
#include "CORE/GameObject.hpp"
#include "CORE/Window/Input.hpp"
#include "CORE/PhysicsComponents/PlayerControllerComponent.hpp"

class Player : public GameObject {

public:
	Player(const char* name) : GameObject(name) {
	
		m_camera = addComponent<CameraMCComponent>(65, 1280.0f / 720.0f, 0.1f, 100000.0f);
		m_camera->setPosition(glm::vec3(0.0f, 1.5f, 0.0f));
		m_camera->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));

		m_playerController = addComponent<PlayerControllerComponent>();


		const auto& paleVisitoModel = addComponent<ModelRenderer>("res/3DModels/paleVisitor/source/PaleVisitor.fbx");
		paleVisitoModel->setRotation(-90, 0, 0);
		paleVisitoModel->setPosition(0, -2, 0);

		const auto& DAMICROWAVE= addComponent<ModelRenderer>("res/3DModels/DAMICROWAVE.fbx");
		DAMICROWAVE->setRotation(0, 0, 0);
		DAMICROWAVE->setPosition(0, 0.6, 0);

	}


	void onImGuiRender() override {
		
	}

	void onUpdate(float dt) override {
		// Process keyboard input for movement (only when mouse is locked)
		if (Input::isMouseLocked()) {
			
				
		}
	}

private:
	float m_mass{ 80.0f };
	float m_health{ 100.0f };

	std::shared_ptr<CubePhysics> m_playerBody;
	std::shared_ptr<CameraComponent> m_camera;
	std::shared_ptr<PlayerControllerComponent> m_playerController;



};