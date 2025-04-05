
#include "../RenderComponent.hpp"
#include "../LightManager.hpp"
#include <glm/glm.hpp>
#include "../ModelLoader/Mesh.hpp" 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class ModelRenderer : public RenderComponent {
public:
	ModelRenderer(const std::string& path) : RenderComponent(), m_path(path) {	}

	void setPath(const std::string& path) {
		m_path = path;
	}


	void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override {
		auto shader = ShaderManager::getShader("simpleDepthShader");
		shader->use();
		shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));

		for (auto& mesh : meshes) {
			mesh.drawRawGeometry();
		}
	}


	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override {


		if (!m_shader) return;

		m_shader->use();

		// Standard matrix uniforms
		m_shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
		m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
		m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));
		m_shader->setMat4("lightSpaceMatrix", LightManager::getShadowMapper()->getLightSpaceMatrix());

		// Set light space matrix for shadows
		//m_shader->setMat4("lightSpaceMatrix", LightManager::getLightSpaceMatrix());



		// Material properties
		m_shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);
		m_shader->setVec3("viewPos", cam->getPosition());


		// Lighting (same as your existing draw() code)
		std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);
		m_shader->setBool("useLighting", !relevantLights.empty());
		m_shader->setInt("numLights", relevantLights.size());

		for (size_t i = 0; i < relevantLights.size(); i++) {
			std::string base = "lights[" + std::to_string(i) + "].";

			// Set light type
			int lightType = 0; // Default to Point Light
			switch (relevantLights[i]->getType()) {
			case LightType::POINT: lightType = 0; break;
			case LightType::DIRECTIONAL: lightType = 1; break;
			case LightType::SPOT: lightType = 2; break;
			}
			m_shader->setInt(base + "type", lightType);
			m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
			m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
			m_shader->setVec3(base + "color", relevantLights[i]->getColor());
			m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());

		}
		//glActiveTexture(GL_TEXTURE15);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
		m_shader->setInt("shadowMap", 0);
		// 
		// Draw all meshes
		for (unsigned int i = 0; i < meshes.size(); i++) {

			meshes[i].Draw(m_shader, LightManager::getShadowMapper()->getLightSpaceMatrix(),true,true);
		}

	}


	void init() override {
		// Load the model
		loadModel();
		m_shader = ShaderManager::getShader("assimpModel");
		m_isShadowCaster= true;
		m_isShadowReceiver = true;

	}

	~ModelRenderer() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void draw(const std::shared_ptr<Camera> cam) override {
		renderWithMaterials(cam);
	}
private:

	void loadModel();
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	unsigned int TextureFromFile(const char* path, const std::string& directory);
	void printModelInfo() const {
		std::cout << "Model Information:\n";
		std::cout << "  Meshes: " << meshes.size() << "\n";

		for (size_t i = 0; i < meshes.size(); i++) {
			std::cout << "  Mesh " << i << ":\n";
			std::cout << "    Vertices: " << meshes[i].vertices.size() << "\n";
			std::cout << "    Indices: " << meshes[i].indices.size() << "\n";
			std::cout << "    Textures: " << meshes[i].textures.size() << "\n";

			for (const auto& tex : meshes[i].textures) {
				std::cout << "      " << tex.type << ": " << tex.path << "\n";
			}
		}
	}


	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	std::string m_path;

};