// Mesh.h
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../ShaderProgram.hpp"
#include "../TextureManager.hpp"
#include <stdexcept>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures);
    void Draw(std::shared_ptr<ShaderProgram> shader);

    void Draw(std::shared_ptr<ShaderProgram> shader,
        const glm::mat4& lightSpaceMatrix,
        bool useLighting = true,
        bool useShadows = true);

    void drawRawGeometry() const
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
    // Debug info
    void printInfo() const;

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};
