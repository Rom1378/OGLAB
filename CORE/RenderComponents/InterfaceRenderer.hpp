#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class RenderComponent;

class InterfaceRenderer
{
public:
	virtual ~InterfaceRenderer() = default;
	virtual void initialize(RenderComponent *component) = 0;
	virtual void render(RenderComponent* component) = 0;
}; // class InterfaceRenderer


// Basic single-object renderer
class StandardRenderer : public InterfaceRenderer {
public:
    void initialize(RenderComponent* component) override {
        // Setup standard VAO, VBO, etc.
    }

    void render(RenderComponent* component) override {
        // Standard OpenGL draw calls
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

private:
    GLuint VAO, VBO, EBO;
    int indexCount;
};

/*
// Instanced renderer for multiple objects
class InstancedRenderer : public InterfaceRenderer {
public:
    void initialize(RenderComponent* component) override {
        // Setup instanced arrays
        setupInstancedBuffers();
    }

    void render(RenderComponent* component) override {
        // Instanced rendering
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
    }

    void updateInstanceData(const std::vector<InstanceData>& data) {
        // Update instance buffer
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(InstanceData), data.data(), GL_DYNAMIC_DRAW);
    }

private:
    GLuint VAO, VBO, EBO, instanceVBO;
    int indexCount, instanceCount;
};

*/