#include "MeshRenderer.hpp"

#include "Mesh.hpp"

#include "CORE/Mesh/CubeMesh.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Lights/LightManager.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"

#include "AssimpLoader.hpp"
#include <memory>

#include "CORE/Debug.hpp"

#include "CORE/Scene.hpp"
#include <unordered_map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>


namespace MeshRenderer {

	std::unordered_map<RenderableHandle, RenderableData> meshes;
	RenderableHandle nextHandle = 1;

	RenderableHandle cubeHandle;
	RenderableHandle sphereHandle;

	struct RenderState {
		bool renderGeometryOnly	{false};
		bool shadowcaster{ true };
		bool hide{ false };
	};

	struct Material {
		ShaderProgram* shader{ nullptr };
		std::vector<Texture*> textures{};

		glm::vec4 albedoColor{ 1.0f };
		float metallic{ 0.0f };
		float roughness{ 0.5f };
		float emissive{ 0.0f };

		// Textures are shared resources - use handles
		//TODO: Texture* should become some kind of TextureHandle
		Texture* albedoTexture{ INVALID_TEXTURE };
		Texture* normalMap{ INVALID_TEXTURE };
		Texture* metallicRoughnessMap{ INVALID_TEXTURE };
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		GLuint vbo, ebo, vao;
		uint32_t indexCount;
	};

	struct RenderableData{
		std::vector<MeshData> meshdatas;
		Material material;
		RenderState rdState;
	};


	RenderableHandle createCube() {
		GLuint VAO, VBO, EBO;
		// Generate and bind VAO, VBO, and EBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		// Bind and fill VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshRawData::Cube::vertices.size(), MeshRawData::Cube::vertices.data(), GL_STATIC_DRAW);

		// Bind and fill EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * MeshRawData::Cube::indices.size(), MeshRawData::Cube::indices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Set default shader

		Material m;
		m.shader = &*ShaderManager::getShader("cube");
		RenderState s;
		RenderableData rd{
			.meshdatas = {
				MeshData{
					MeshRawData::Cube::vertices ,
					MeshRawData::Cube::indices,
					VBO,
					EBO,
					VAO,
					static_cast<uint32_t>(MeshRawData::Cube::indices.size())
				}
			},
			.material = m,
			.rdState = s
		};
	}

	RenderableHandle createSphere() {
		LOG("createSphere not implemented");
	}

	RenderableHandle createRenderableFromFile(const std::string& path) {

	}

	void destroy(RenderableHandle handle);


	void initPrimitives() {
		//init primitives
		cubeHandle = createCube();
		sphereHandle = createSphere();
	}
	void init() {
		glGenVertexArrays(1, &VAO);
		initPrimitives();
	}

	RenderableComponent(RenderableHandle handle);
	RenderableComponent(Primitive t);

	RenderComponent::RenderComponent(const std::string& model_path) : meshType{ MeshType::MODEL } {
		meshHandle = createMesh(model_path);
		shader = &*ShaderManager::getShader("assimpModel");
	}

	RenderComponent::RenderComponent(MeshType t) : meshType{ t } {
		switch (t)
		{
		case MeshRenderer::MeshType::CUBE:
			shader = &*ShaderManager::getShader("cube");
			break;
		case MeshRenderer::MeshType::SPHERE:
			shader = &*ShaderManager::getShader("sphere");
			break;
		case MeshRenderer::MeshType::MODEL:
			shader = &*ShaderManager::getShader("assimpModel");
			break;
		default:
			LOG_ERROR("Mesh type does not exist");
			return;
			break;
		}
		meshHandle = createMesh(t);
	}

	MeshHandle createMesh(const std::string& model_path) {
		assimpLoader::loadModel(model_path);
	}

	MeshHandle createMesh(MeshType t) {

	}

	void destroyMesh(MeshHandle handle) {

	}


	void renderPass(Scene* scene) {
		for (auto e : scene->getEntities()) {
			RenderComponent* renderComponent = e.getComponent<RenderComponent>();
			TransformComponent* transform = e.getComponent<TransformComponent>();
			if (!renderComponent)
				continue;

			if (!transform) {
				LOG_WARN("Entity with mesh component must have a transform component in order to be rendered");
				continue;
			}

			ShaderProgram* shader= renderComponent->shader;
			if (!shader) {
				LOG_ERROR("RenderComponent must have a shader bound");
				continue;
			}

			shader->use();

			// Standard matrix uniforms
			shader->setMat4("model", glm::value_ptr(transform->getModelMatrix()));
			shader->setMat4("view", glm::value_ptr(Renderer::getView()));
			shader->setMat4("projection", glm::value_ptr(Renderer::getProjection()));

			shader->setMat4("lightSpaceMatrix", LightManager::getShadowMapper()->getLightSpaceMatrix());

			renderComponent->material.

			// Material properties
			shader->setVec3("objectColor", m_color.x, m_color.y, m_color.z);
			shader->setVec3("viewPos", cam->getWorldPosition());


			// Lighting (same as your existing draw() code)
			std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);
			shader->setBool("useLighting", !relevantLights.empty());
			shader->setInt("numLights", relevantLights.size());

			/*
			for (size_t i = 0; i < relevantLights.size(); i++) {
				std::string base = "lights[" + std::to_string(i) + "].";

				// Set light type
				int lightType = 0; // Default to Point Light
				switch (relevantLights[i]->getType()) {
				case LightType::POINT: lightType = 0; break;
				case LightType::DIRECTIONAL: lightType = 1; break;
				case LightType::SPOT: lightType = 2; break;
				}
				//m_shader->setInt(base + "type", lightType);
				m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
				//m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
				//m_shader->setVec3(base + "color", relevantLights[i]->getColor());
				//m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());
			}
			*/

			for (size_t i = 0; i < relevantLights.size(); i++) {
				std::string base = "lights[" + std::to_string(i) + "].";
				shader->setVec3(base + "position", relevantLights[i]->getWorldPosition());
				shader->setVec3(base + "color", relevantLights[i]->getColor());
				shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());
			}
			for (unsigned int i = 0; i < meshes.size(); i++) {
				meshes[i].Draw(shader, LightManager::getShadowMapper()->getLightSpaceMatrix(),
					!relevantLights.empty(), true);
			}

		}
	}

	void cleanup() {
		for (auto mesh : meshes) {
			delete mesh;
		}
		cubeMesh = nullptr;
		LOG_WARN("Nothing to cleanup inMeshRenderer");
	}
}