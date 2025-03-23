#include "HeightMap.hpp"
#include "Shader.hpp"

HeightMap::HeightMap(const char* path) 
{
	auto renderComponent = this->addComponent<HeightMapRenderer>();
	//auto physicsComponent = addComponent<PhysicsComponent>();

	renderComponent->loadHeightMap(path);

}

void HeightMapRenderer::init()
{
	// Create the shader program
	m_shaderProgram = std::make_shared<ShaderProgram>();
	m_shaderProgram->loadShaders("res/shaders/heightmap.vert", "res/shaders/heightmap.frag");

	// Create the vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Create the vertex buffer object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	// Create the element buffer object
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	// Set the vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Unbind the VAO
	glBindVertexArray(0);
}

void HeightMapRenderer::draw()
{
	// Use the shader program
	m_shaderProgram->use();

	// Bind the VAO
	glBindVertexArray(m_vao);

	// Draw the heightmap
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	// Unbind the VAO
	glBindVertexArray(0);
}

void HeightMapRenderer::loadHeightMap(const char* path)
{
	// Load the heightmap
	m_heightMap = std::make_shared<HeightMap>(path);

	// Get the heightmap data
	auto data = m_heightMap->getData();

	// Set the index count
	m_indexCount = data._indices.size();

	// Bind the VAO
	glBindVertexArray(m_vao);

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, data._vertices.size() * sizeof(renderer::vertex), data._vertices.data(), GL_STATIC_DRAW);

	// Bind the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data._indices.size() * sizeof(uint32_t), data._indices.data(), GL_STATIC_DRAW);

	// Unbind the VAO
	glBindVertexArray(0);
}