#include "MeshRenderer.hpp"

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

#include "CORE/Systems/Renderer/Sphere.hpp"





namespace MeshRenderer {

	std::unordered_map<RenderableHandle, RenderableData> meshes{};
	RenderableHandle nextHandle = 1;

	RenderableHandle cubeHandle;
	RenderableHandle sphereHandle;



	std::vector<Material>* getMaterial(RenderableHandle handle) {
		auto mit = meshes.find(handle);
		if (mit == meshes.end()) {
			LOG_ERROR("Handle does not link to any renderable");
			return nullptr;
		}
		else
			return &mit->second.material;
	}

	RenderableHandle createCube() {
		if (cubeHandle)
			return cubeHandle;
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
		m.shader = &*ShaderManager::getShader("standard");
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
			.material = {m},
		};

		meshes[nextHandle] = std::move(rd);
		return nextHandle++;
	}

	RenderableHandle createSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
		if (sphereHandle)
			return sphereHandle;
		MeshData meshData{};
		generateSphere(meshData.vertices, meshData.indices, radius, sectorCount, stackCount);
		glGenVertexArrays(1, &meshData.vao);
		glGenBuffers(1, &meshData.vbo);
		glGenBuffers(1, &meshData.ebo);

		glBindVertexArray(meshData.vao);

		// Bind and fill VBO
		glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
		glBufferData(GL_ARRAY_BUFFER, meshData.vertices.size() * sizeof(float), meshData.vertices.data(), GL_STATIC_DRAW);

		// Bind and fill EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.size() * sizeof(uint32_t), meshData.indices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		Material m{
				.shader = &*ShaderManager::getShader("sphere")
		};
		RenderableData rd{
			.meshdatas = { meshData },
			.material = {m}
		};
		meshes[nextHandle] = std::move(rd);
		return nextHandle++;
	}


	RenderableHandle createRenderableFromFile(const std::string& path) {
		RenderableData rd{};
		if (assimpLoader::loadModel(rd, path)) {
			meshes[nextHandle] = rd;
			return nextHandle++;
		}
		else
			return INVALID_HANDLE;
	}

	static void initPrimitives() {
		//init primitives
		cubeHandle = createCube();
		sphereHandle = createSphere(1,5,5);
		LOG_OK("Primitives initialized");
	}
	void init() {
		initPrimitives();
	}

	void destroyRenderable(RenderableHandle handle) {
		meshes.erase(handle);
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

			ShaderProgram* shader = renderComponent->shader;
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
		meshes.clear();
		cubeHandle = INVALID_HANDLE;
		sphereHandle = INVALID_HANDLE;
	}
	bool isValidHandle(RenderableHandle handle) {
		return meshes.find(handle) != meshes.end();
	}

}


