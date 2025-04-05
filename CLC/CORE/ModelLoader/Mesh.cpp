#include "Mesh.hpp"
#include <iostream>
#include "../LightManager.hpp"

Mesh::Mesh(std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::vector<Texture> textures)
	: vertices(vertices), indices(indices), textures(textures) {
	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(std::shared_ptr<ShaderProgram> shader) {
	// Start texture units after shadow map (unit 15)
	unsigned int textureUnit = 1; // Start from 1 to leave 0 free
	shader->setBool("useTexture", !textures.empty());

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + textureUnit);

		std::string uniformName;
		if (textures[i].type == "texture_diffuse") {
			uniformName = "material.diffuse" + std::to_string(textureUnit);
		}
		else if (textures[i].type == "texture_specular") {
			uniformName = "material.specular" + std::to_string(textureUnit);
		}
		else if (textures[i].type == "texture_normal") {
			uniformName = "material.normal" + std::to_string(textureUnit);
		}

		shader->setInt(uniformName.c_str(), textureUnit	);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		textureUnit++;
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw(std::shared_ptr<ShaderProgram> shader,
	const glm::mat4& lightSpaceMatrix,
	bool useLighting,
	bool useShadows) {
	// Bind shadow map first to unit 15 if needed
	if (useShadows) {
		glActiveTexture(GL_TEXTURE15);
		shader->setInt("shadowMap", 15);
		glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
	}

	// Bind model textures starting from unit 0
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	
	shader->setBool("useTexture", !textures.empty());

	shader->setBool("useLighting", useLighting);
	shader->setBool("useShadows", useShadows);
	shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);



	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;
		std::string uniformName;

		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
			uniformName = "material.diffuse" + number;
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
			uniformName = "material.specular" + number;
		}

		shader->setInt(uniformName.c_str(), i);
		//shader->setInt("texture_diffuse1", i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::printInfo() const {
	std::cout << "Mesh Info:\n";
	std::cout << "  Vertices: " << vertices.size() << "\n";
	std::cout << "  Indices: " << indices.size() << "\n";
	std::cout << "  Textures: " << textures.size() << "\n";

	if (!vertices.empty()) {
		std::cout << "  First vertex:\n";
		std::cout << "    Position: " << vertices[0].Position.x << ", "
			<< vertices[0].Position.y << ", "
			<< vertices[0].Position.z << "\n";
		std::cout << "    Normal: " << vertices[0].Normal.x << ", "
			<< vertices[0].Normal.y << ", "
			<< vertices[0].Normal.z << "\n";
		std::cout << "    TexCoords: " << vertices[0].TexCoords.x << ", "
			<< vertices[0].TexCoords.y << "\n";
	}
}