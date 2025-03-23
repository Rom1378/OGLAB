#pragma once
#include "RenderComponent.hpp"
#include "GameObject.hpp"
#include "Mesh/CubeMesh.hpp"
#include <iostream>

class CubeRenderer : public RenderComponent {

protected:
	std::vector<float> vertices = MeshData::Cube::vertices;
	std::vector<unsigned int> indices = MeshData::Cube::indices;

public:


	void init() override {
	
		// Generate and bind VAO, VBO, and EBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		// Bind and fill VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

		// Bind and fill EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Set default shader
		setShader("standard");
		m_shader->use();

		this->m_color = glm::vec3(111.0f, 221.0f, 0.2f);

	}

	void draw(const glm::mat4& view, const glm::mat4& projection) override {
		if (!m_shader) return;

		m_shader->use();

		// Set uniforms for textures

		m_shader->setBool("useTexture", !m_textures.empty());
		if (!m_textures.empty())
			bindTextures();


		m_shader->setMat4("model", glm::value_ptr(this->getGameObject()->getModelMatrix()));
		m_shader->setMat4("view", glm::value_ptr(view));
		m_shader->setMat4("projection", glm::value_ptr(projection));

		// Set lighting uniforms
		m_shader->setVec3("lightPos", 3.2f, 1.0f, 2.0f);
		m_shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		m_shader->setVec3("objectColor", 1.0f, 1.0f, 0.2f);


		// Draw cube using indices
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (!m_textures.empty())
			unBindTextures();
	}

	~CubeRenderer() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

};


