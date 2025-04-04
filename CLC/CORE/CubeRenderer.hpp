#pragma once
#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include <iostream>

class CubeRenderer : public RenderComponent {

protected:
	
public:


    void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override {
        // Use simple depth shader (no materials/textures/lighting)
        auto depthShader = ShaderManager::getShader("simpleDepthShader");
        depthShader->use();

        // Only need model and light space matrix
        depthShader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
        depthShader->setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));

        // Draw bare geometry
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    void renderWithMaterials(const std::shared_ptr<Camera>& cam) override{
        if (!m_shader) return;

        m_shader->use();

        // Standard matrix uniforms
        m_shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
        m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
        m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));

        // Material properties
        m_shader->setBool("useTexture", !m_textures.empty());
        m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);

        // Bind textures if available
        if (!m_textures.empty()) {
            bindTextures();
        }

        // Lighting (same as your existing draw() code)
        std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);
        m_shader->setBool("useLighting", !relevantLights.empty());
        m_shader->setInt("numLights", relevantLights.size());

        for (size_t i = 0; i < relevantLights.size(); i++) {
            std::string base = "lights[" + std::to_string(i) + "].";

            // Set light type
            int lightType = 0; // Default to Point Light
            switch (relevantLights[i]->getType()) {
            case LightType::POINT: lightType = 0; break;
            case LightType::DIRECTIONAL: lightType = 1; break;
            case LightType::SPOT: lightType = 2; break;
            }
            m_shader->setInt(base + "type", lightType);

            m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
            m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
            m_shader->setVec3(base + "color", relevantLights[i]->getColor());
            m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());

        }

        // Bind shadow map (now handled by LightManager)
        LightManager::bindShadowMap(m_shader);

        // Draw
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (!m_textures.empty()) {
            unBindTextures();
        }
    }


    void init() override;
	void draw(const std::shared_ptr<Camera> cam) override;
	~CubeRenderer() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

};


