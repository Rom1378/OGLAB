#include "Component.hpp"
#include "GameObject.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


void Component::onImGuiRender() {
	ImGui::Text("onImGuiRender not written for this component");

}


glm::mat4 TransformableComponent::getLocalMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_localPosition);
    glm::quat q = glm::quat(glm::radians(m_localRotation));
    model *= glm::toMat4(q);
    model = glm::scale(model, m_localScale);
    return model;
}

glm::mat4 TransformableComponent::getWorldMatrix() const {
    if (m_gameObject)
        return this->m_gameObject->getModelMatrix() * getLocalMatrix();
    LOG_WARN("No parent gameobject for this component");
    return getLocalMatrix();
}

glm::vec3 TransformableComponent::getWorldPosition() const {
    glm::mat4 world = getWorldMatrix();
    return glm::vec3(world[3]); // Extract translation
}

glm::vec3 TransformableComponent::getWorldDirection() const {
    glm::vec3 forward = { 0, 0, -1 }; // OpenGL forward
    return glm::normalize(glm::mat3(getWorldMatrix()) * forward);
}

