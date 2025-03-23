#pragma once
#include "../GameObject.hpp"
#include "../RenderComponent.hpp"
#include "../Mesh/CubeMesh.hpp"
#include "../Component.hpp"
#include <glad/glad.h>

// Cube specific renderer
class CubeRenderer : public RenderComponent {
public:
    void init() override {
        // Setup cube mesh data
        const float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            // Back face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            // Other faces...
        };

        const unsigned int indices[] = {
            0, 1, 2,  2, 3, 0,  // Front
            4, 5, 6,  6, 7, 4,  // Back
            // Other faces...
        };

        // Create and bind VAO
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindVertexArray(m_VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        setShader("basic");
    }

    void draw() override {
        if (m_shader) {
            m_shader->use();
            
            // Update shader uniforms
            auto transform = getGameObject()->getTransform();
            m_shader->setMat4("model", transform->getModelMatrix());
            
            // Draw cube
            glBindVertexArray(m_VAO);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

    ~CubeRenderer() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
};

class CubePhysics : public Component {
public:
    void init() override {
        // Create collision shape
        createCollisionBox({1.0f, 1.0f, 1.0f});
        
        // Set physics properties
        setMass(1.0f);
        setFriction(0.5f);
        setRestitution(0.5f);
    }

    void update(float dt) override {
        // Update physics simulation
        updatePhysics(dt);
        
        // Update transform from physics
        syncTransform();
    }

private:
    void createCollisionBox(const glm::vec3& size);
    void updatePhysics(float dt);
    void syncTransform();
};

class Cube : public GameObject {
public:
    Cube() {
        // Add and configure renderer
        auto renderer = addComponent<CubeRenderer>();
    }

    static std::shared_ptr<Cube> create() {
        return std::make_shared<Cube>();
    }
};