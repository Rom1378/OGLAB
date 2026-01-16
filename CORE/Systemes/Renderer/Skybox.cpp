
#include "Skybox.hpp"
#include "CORE/Mesh/CubeMap.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Debug.hpp"
#include "CORE/Systemes/Renderer/Renderer.hpp"

#include "CORE/TextureManager.hpp"
#include "CORE/Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace Skybox {
	GLuint gVAO{ };
	GLuint gVBO{ };

	ShaderProgram* cubeMapShader{ nullptr };
	
	void init() {

		glGenVertexArrays(1, &gVAO);
		glGenBuffers(1, &gVBO);
		glBindVertexArray(gVAO);
		glBindBuffer(GL_ARRAY_BUFFER, gVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshData::CubeMap::vertices.size(), MeshData::CubeMap::vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		cubeMapShader = ShaderManager::getShader("cubemap").get();
	}

	skyboxHandle createSkybox(const std::vector<std::string>& faces) {
		
		GLuint skyTexture = static_cast<GLuint>(TextureManager::loadCubemap(faces, "skybox")->id);
		
		if (!cubeMapShader)
			LOG_ERROR("cubeMapShader is null");
		cubeMapShader->use();
		cubeMapShader->setInt("skybox", 0);
		return skyboxHandle{ skyTexture};
	}

	void Skybox::skyboxPass(Scene* scene) {

		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_FALSE);   // Disable depth writinkg

		if (!cubeMapShader)
			LOG_ERROR("cubeMapShader is null");
		
		cubeMapShader->use();
		cubeMapShader->setMat4("view", glm::value_ptr(glm::mat4(glm::mat3(Renderer::getView()))));
		cubeMapShader->setMat4("projection", glm::value_ptr(Renderer::getProjection()));

		glBindVertexArray(gVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, scene->getSkyboxHandle().id);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		}

	void cleanup() {
		cubeMapShader = nullptr;
	}

}
