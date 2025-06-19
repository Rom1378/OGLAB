#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Component.hpp"
#include "../Transform.hpp"
#include <memory>

enum class LightType { POINT, DIRECTIONAL, SPOT };

class Light : public Component, Transform, public std::enable_shared_from_this<Light> {
public:

    Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity);
    ~Light();

    
    void init() override;

    void update(float dt) override;

    LightType getType() const { return type; }
    void setType(LightType type) { this->type = type; }

    glm::vec3 getColor() const { return color; }
    void setColor(glm::vec3 color) { this->color = color; }

    float getIntensity() const { return intensity; }
    void setIntensity(float intensity) { this->intensity = intensity; }

    glm::vec3 getDirection() const { return getRotation(); }
    void setDirection(glm::vec3 direction) { this->m_rotation = direction; }

    glm::vec3 getPosition() const { return m_position; }
    void setPosition(glm::vec3 position) { m_position = position; }

private:
    LightType type;
    glm::vec3 color;
    float intensity;

};

