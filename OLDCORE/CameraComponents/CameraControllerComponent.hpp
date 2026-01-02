
/*
#pragma once

#include "CORE/CameraComponents/Camera.hpp"
#include "CORE/GameObject.hpp"


class CameraRotationControllerComponent : public Component {

public:
	CameraRotationControllerComponent(float speed = 14.0) : m_speed{ speed } {}
	~CameraRotationControllerComponent() = default;

	void init() {}
	void update(float dt) {
		try {
			const std::shared_ptr<CameraComponent>& cam = m_gameObject->getComponent<CameraComponent>();
			
			if (cam == nullptr)
				throw std::exception("Host gameObject doesnt have Camera component");

			glm::vec3 dm{ 0.0,0.0,0.0 };
			auto& right = cam->getRight();
			auto& forward = cam->getForward();

			float speed = m_speed;
			if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
				speed *= 2.0;
			}
			else {
				speed /= 2.0;
			}
		
			if (Input::isKeyPressed(GLFW_KEY_W)) {
				dm += forward * speed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_S)) {
				dm -= forward * speed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_A)) {
				dm -= right * speed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_D)) {
				dm += right * speed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
				dm.y += speed * dt;
			}
			if (Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
				dm.y -= speed * dt;
			}

			m_gameObject->move(dm);

		}
		catch (const std::exception& e) {
			LOG_ERROR(e.what());
		}

	}
	void onImGuiRender() {
		ImGui::Begin("CameraRotationControllerComponent");
		ImGui::DragFloat("m_speed", &m_speed, 0.2, 0.01, 30.0);
		ImGui::End();
	}
	const char* getName() { return "CameraRotationControllerComponent"; }

private:

	float m_speed;

};

*/