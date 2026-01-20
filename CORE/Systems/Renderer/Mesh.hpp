// Mesh.h

/*
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <glad/glad.h>
#include "MeshRenderer.hpp"

class Texture;
class ShaderProgram;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    //glm::vec3 Tangent;
    //glm::vec3 Bitangent;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices);

private:
	GLuint VAO, VBO, EBO;
	void setupMesh();
};

class Material {
    std::vector<Texture*> textures{};
    glm::vec4 color{ 1.0f };
    bool useTexture{ false };


};


struct MeshComponent {
    MeshComponent(MeshRenderer::Primitive p = MeshRenderer::Primitive::Cube) : 
        mesh{ MeshRenderer::cubeMesh } {}

    MeshComponent(const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices) :
           mesh{ vertices,indices } {}

    Mesh* mesh;
    Material material;
    ShaderProgram* shader;


};*/
