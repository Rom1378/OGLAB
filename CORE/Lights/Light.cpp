#include "Light.hpp"
#include "LightManager.hpp"
#include "../GameObject.hpp"


Light::Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity)
    :// Transform(position, direction), TODO ADD LOCAL TRANSFORMATION FOR NOW ONLY USING GAMEOBJECT TRANSFORM
	type(type), color(color), intensity(intensity) {
	setPosition(position);
}

//Light::~Light() {

    //LightManager::removeLight(shared_from_this());
//}

void Light::init() {
    try {
        auto self = shared_from_this();
		LightManager::addLight(self);
	}
	catch (const std::bad_weak_ptr& e) {
		LOG_ERROR("shared_from_this failed in Light::init()\n");
	}
}

void Light::update(float dt) {
	if (auto gameObject = this->getGameObject()) {
		//setPosition()
		//:w
		// m_position = gameObject->getPosition();

	}
}

void Light::onImGuiRender() {
	//glm::vec3 lightDir = getGameObject()->getRotation();
	//if (ImGui::DragFloat3("Direction", glm::value_ptr(lightDir), 0.1f)) {
	//	getGameObject()->setRotation(lightDir);
	//}

	glm::vec3 lightColor = getColor();
	if (ImGui::ColorEdit3("Color", glm::value_ptr(lightColor))) {
		setColor(lightColor);
	}

	float lightIntensity = getIntensity();
	if (ImGui::DragFloat("Intensity", &lightIntensity, 0.01f, 0.0f, 10.0f)) {
		setIntensity(lightIntensity);
	}
}

