#include "Light.hpp"
#include "LightManager.hpp"
#include "../GameObject.hpp"


Light::Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity)
    : Transform(position, direction), type(type), color(color), intensity(intensity) {
}

Light::~Light() {
    LightManager::removeLight(shared_from_this());
}

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
        m_position = gameObject->getPosition();

    }
}

