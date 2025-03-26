#pragma once
#include <glm/glm.hpp>

enum class LightType { POINT, DIRECTIONAL, SPOT };

class Light {
public:
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;

    Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity)
        : type(type), position(position), direction(direction), color(color), intensity(intensity) {}
};

