#include "CubeMap.hpp"
#include "Mesh/CubeMap.hpp"

void CubeMap::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshData::CubeMap::vertices.size(), MeshData::CubeMap::vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// Set default shader
	setShader("cubemap");

}

void CubeMap::draw(const glm::mat4& view, const glm::mat4& projection) {
	if (!m_shader) return;

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);   // Disable depth writinkg

	m_shader->use();

	// Set uniforms for textures

	//m_shader->setBool("useTexture", !m_textures.empty());
	//if (!m_textures.empty())
	//	bindTextures();

	// Set uniforms for color
	//m_shader->setVec4("color", m_color.x, m_color.y, m_color.z, m_color.w);


	// Set uniforms for model, view, projection
	m_shader->setMat4("view", glm::value_ptr(glm::mat4(glm::mat3(view))));
	m_shader->setMat4("projection", glm::value_ptr(projection));

	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textures[0]->id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);    // Re-enable depth writing
	glDepthFunc(GL_LESS);

	//unbind texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


}