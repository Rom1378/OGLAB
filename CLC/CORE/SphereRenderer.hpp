#pragma once
#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include <vector>
#include <glm/glm.hpp>
#include <array>

constexpr float M_PI = 3.14159265359f;  // Fixed M_PI definition

class SphereRenderer : public RenderComponent {
public:
    SphereRenderer(float radius = 1.0f, unsigned int sectorCount = 36, unsigned int stackCount = 18)
        : radius(radius), sectorCount(sectorCount), stackCount(stackCount) {
        
        generateSphere(radius, sectorCount, stackCount);
        initBuffers();
    }

    void init() override {
        // Set default shader
        setShader("sphere");
    }

    void draw(const std::shared_ptr<Camera> cam) override;

    ~SphereRenderer() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    
    float radius;
    unsigned int sectorCount, stackCount;

    void initBuffers() {
        // Generate and bind VAO, VBO, and EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Bind and fill VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Bind and fill EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    void generateSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
        vertices.clear();
        indices.clear();

        // Precompute sizes
        unsigned int vertexCount = (stackCount + 1) * (sectorCount + 1);
        unsigned int indexCount = stackCount * sectorCount * 6;

        // Reserve space to prevent multiple reallocations
        vertices.resize(vertexCount * 6); // 3 for position, 3 for normal
        indices.resize(indexCount);

        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f / radius;
        float sectorStep = 2.0f * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;

        unsigned int vertexIndex = 0;
        for (unsigned int i = 0; i <= stackCount; ++i) {
            stackAngle = M_PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (unsigned int j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                // Store vertex position
                vertices[vertexIndex++] = x;
                vertices[vertexIndex++] = y;
                vertices[vertexIndex++] = z;

                // Store normalized vertex normal
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices[vertexIndex++] = nx;
                vertices[vertexIndex++] = ny;
                vertices[vertexIndex++] = nz;
            }
        }

        unsigned int indexIndex = 0;
        for (unsigned int i = 0; i < stackCount; ++i) {
            unsigned int k1 = i * (sectorCount + 1);
            unsigned int k2 = k1 + sectorCount + 1;

            for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices[indexIndex++] = k1;
                    indices[indexIndex++] = k2;
                    indices[indexIndex++] = k1 + 1;
                }

                if (i != (stackCount - 1)) {
                    indices[indexIndex++] = k1 + 1;
                    indices[indexIndex++] = k2;
                    indices[indexIndex++] = k2 + 1;
                }
            }
        }
    }

};
