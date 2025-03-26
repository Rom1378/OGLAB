#include "SphereRenderer.hpp"
#include "LightManager.hpp"


void SphereRenderer::draw(const std::shared_ptr<Camera> cam) {
    if (!m_shader) return;

    m_shader->use();

    // Set uniforms
    m_shader->setMat4("model", glm::value_ptr(this->getGameObject()->getModelMatrix()));
    m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
    m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));


    std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);

    bool useLighting = !relevantLights.empty();
    m_shader->setBool("useLighting", useLighting);
    m_shader->setInt("numLights", relevantLights.size());

    for (size_t i = 0; i < relevantLights.size(); i++) {
        std::string base = "lights[" + std::to_string(i) + "].";

        // Set light type
        int lightType = 0; // Default to Point Light
        switch (relevantLights[i]->type) {
        case LightType::POINT: lightType = 0; break;
        case LightType::DIRECTIONAL: lightType = 1; break;
        case LightType::SPOT: lightType = 2; break;
        }
        m_shader->setInt(base + "type", lightType);

        m_shader->setVec3(base + "position", relevantLights[i]->position.x, relevantLights[i]->position.y, relevantLights[i]->position.z);
        m_shader->setVec3(base + "direction", relevantLights[i]->direction.x, relevantLights[i]->direction.y, relevantLights[i]->direction.z);
        m_shader->setVec3(base + "color", relevantLights[i]->color.x, relevantLights[i]->color.y, relevantLights[i]->color.z);
        m_shader->setFloat(base + "intensity", relevantLights[i]->intensity);
        //print lights data
        std::cout << "Light " << i << " position: " << relevantLights[i]->position.x << ", " << relevantLights[i]->position.y << ", " << relevantLights[i]->position.z << std::endl;
        std::cout << "Light " << i << " color: " << relevantLights[i]->color.x << ", " << relevantLights[i]->color.y << ", " << relevantLights[i]->color.z << std::endl;
        std::cout << "Light " << i << " intensity: " << relevantLights[i]->intensity << std::endl;

    }


    m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);

    // Draw sphere using indices
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}