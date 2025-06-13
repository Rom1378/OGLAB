#include "Light.hpp"
#include "../GameObject.hpp"


void Light::update(float dt) {
    if (auto gameObject = this->getGameObject()) {
        m_position = gameObject->getPosition();

    }
}