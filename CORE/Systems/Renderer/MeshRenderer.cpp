#include "MeshRenderer.hpp"

#include "CORE/Mesh/CubeMesh.hpp"

#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Renderable.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Lights/LightManager.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"

#include "AssimpLoader.hpp"
#include <memory>

#include "CORE/Debug.hpp"

#include "CORE/Scene.hpp"
#include <unordered_map>

#include "CORE/Systems/Renderer/Sphere.hpp"
#include "CORE/Shader.hpp"
#include "CORE/ShaderProgram.hpp"

namespace MeshRenderer {

	std::unordered_map<RenderableHandle, RenderableData> meshes{};
	RenderableHandle nextHandle = 1;

	RenderableHandle cubeHandle;
	RenderableHandle sphereHandle;


	static constexpr uint32_t MAX_DIR_LIGHTS = 4;
	static constexpr uint32_t MAX_POINT_LIGHTS = 8;
	static constexpr uint32_t MAX_SPOT_LIGHTS = 4;




	std::vector<Material>* getMaterial(RenderableHandle handle) {
		auto mit = meshes.find(handle);
		if (mit == meshes.end()) {
			LOG_ERROR("Handle does not link to any renderable");
			return nullptr;
		}
		else
			return &mit->second.material;
	}

	//init VAO VBO EBO... ogl buffers
	static void createMeshBuffers(RenderableData& data) {
		for (uint32_t i{ 0 }; i < data.meshdatas.size(); i++) {

			glGenVertexArrays(1, &data.meshdatas[i].vao);
			glGenBuffers(1, &data.meshdatas[i].vbo);
			glGenBuffers(1, &data.meshdatas[i].ebo);

			glBindVertexArray(data.meshdatas[i].vao);

			// Bind and fill VBO
			glBindBuffer(GL_ARRAY_BUFFER, data.meshdatas[i].vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.meshdatas[i].vertices.size(), data.meshdatas[i].vertices.data(), GL_STATIC_DRAW);
			// Bind and fill EBO
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.meshdatas[i].ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * data.meshdatas[i].indices.size(), data.meshdatas[i].indices.data(), GL_STATIC_DRAW);

			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
			glEnableVertexAttribArray(0);

			// Normal attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			glEnableVertexAttribArray(1);

			// Texture coordinates
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
			glEnableVertexAttribArray(2);
		}

	}

	RenderableHandle createCube() {
		if (cubeHandle)
			return cubeHandle;

		RenderableData rd{
			.meshdatas = { {
					.vertices = MeshRawData::Cube::vertices,
					.indices = MeshRawData::Cube::indices,
				} },
			.material = { Material() },
			.shader = &*ShaderManager::getShader("standard")
		};
		createMeshBuffers(rd);

		meshes[nextHandle] = rd;
		return nextHandle++;
	}

	RenderableHandle createSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
		if (sphereHandle)
			return sphereHandle;
		RenderableData rd{
			.meshdatas = {{}	},
			.material = { Material() },
			.shader = &*ShaderManager::getShader("standard")
		};

		generateSphere(rd.meshdatas[0].vertices, rd.meshdatas[0].indices, radius, sectorCount, stackCount);

		createMeshBuffers(rd);

		meshes[nextHandle] = rd;
		return nextHandle++;
	}


	RenderableHandle createRenderableFromFile(const std::string& path) {
		RenderableData rd{};
		if (assimpLoader::loadModel(rd, path)) {
			createMeshBuffers(rd);

			meshes[nextHandle] = rd;
			return nextHandle++;
		}
		else
			return INVALID_HANDLE;
	}

	static void initPrimitives() {
		//init primitives
		cubeHandle = createCube();
		sphereHandle = createSphere(1, 5, 5);
		LOG_OK("Primitives initialized");
	}
	void init() {
		initPrimitives();
	}

	void destroyRenderable(RenderableHandle handle) {
		meshes.erase(handle);
	}

	static void setLightUniforms(Scene* scene, ShaderProgram& shader) {

		auto lightView = scene->getComponentView<LightComponent>();

		uint32_t dirLightIndex = 0;
		uint32_t pointLightIndex = 0;
		uint32_t spotLightIndex = 0;


		for (auto [entity, light] : lightView) {
			TransformComponent* transform = scene->getComponent<TransformComponent>(entity);

			switch (light->type) {
			case LightComponent::Type::DIR: {
				if (dirLightIndex >= MAX_DIR_LIGHTS) break;

				std::string base = "dirLights[" + std::to_string(dirLightIndex) + "].";

				glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
				if (transform) {
					direction = transform->forward();
				}

				shader.setVec3(base + "direction", direction);
				shader.setVec3(base + "ambient", light->ambient);
				shader.setVec3(base + "diffuse", light->diffuse);
				shader.setVec3(base + "specular", light->specular);

				dirLightIndex++;
				break;
			}

			case LightComponent::Type::POINT: {
				if (pointLightIndex >= MAX_POINT_LIGHTS) break;
				if (!transform) {
					LOG_WARN("Point light entity must have a transform component");
					break;
				}

				std::string base = "pointLights[" + std::to_string(pointLightIndex) + "].";

				auto* pointData = light->getPointData();
				if (!pointData) {
					LOG_WARN("Point light missing PointLightData");
					break;
				}

				shader.setVec3(base + "position", transform->pos);
				shader.setVec3(base + "ambient", light->ambient);
				shader.setVec3(base + "diffuse", light->diffuse);
				shader.setVec3(base + "specular", light->specular);
				shader.setFloat(base + "constant", pointData->constant);
				shader.setFloat(base + "linear", pointData->linear);
				shader.setFloat(base + "quadratic", pointData->quadratic);

				pointLightIndex++;
				break;
			}

			case LightComponent::Type::SPOT: {
				if (spotLightIndex >= MAX_SPOT_LIGHTS) break;
				if (!transform) {
					LOG_WARN("Spot light entity must have a transform component");
					break;
				}

				std::string base = "spotLights[" + std::to_string(spotLightIndex) + "].";

				auto* spotData = light->getSpotData();
				if (!spotData) {
					LOG_WARN("Spot light missing SpotLightData");
					break;
				}

				shader.setVec3(base + "position", transform->pos);
				shader.setVec3(base + "direction", transform->forward());
				shader.setVec3(base + "ambient", light->ambient);
				shader.setVec3(base + "diffuse", light->diffuse);
				shader.setVec3(base + "specular", light->specular);
				
				shader.setFloat(base + "constant", spotData->constant);
				shader.setFloat(base + "linear", spotData->linear);
				shader.setFloat(base + "quadratic", spotData->quadratic);
				shader.setFloat(base + "cutOff", spotData->cutOff);
				shader.setFloat(base + "outerCutOff", spotData->outerCutOff);

				spotLightIndex++;
				break;
			}

			default:
				LOG_WARN("Light component has unknown type");
				break;
			}
		}
		shader.setInt("numDirLights", dirLightIndex);
		shader.setInt("numPointLights", pointLightIndex);
		shader.setInt("numSpotLights", spotLightIndex);
	}

	void renderPass(Scene* scene) {

		auto& rdSettings = Renderer::getRendererSettings();

		for (auto e : scene->getEntities()) {
			RenderableComponent* renderableComponent = scene->getComponent<RenderableComponent>(e);
			TransformComponent* transform = scene->getComponent<TransformComponent>(e);
			if (!renderableComponent)
				continue;
			if (!transform) {
				LOG_WARN("Entity having renderable component must have a transform component");
				continue;
			}

			RenderableData rdata = meshes[renderableComponent->rhandle];
			ShaderProgram* shader = rdata.shader;

			if (!shader) {
				LOG_ERROR("RenderComponent must have a shader bound");
				continue;
			}

			shader->use();
			for (uint32_t i = 0; i < rdata.meshdatas.size(); i++) {

			// Standard matrix uniforms
			shader->setMat4("model", glm::value_ptr(transform->getModelMatrix()));
			shader->setMat4("view", glm::value_ptr(Renderer::getView()));
			shader->setMat4("projection", glm::value_ptr(Renderer::getProjection()));

			shader->setVec3("viewPos", transform->pos);

			setLightUniforms(scene, *shader);

			shader->setBool("useLighting", rdSettings.enableLighting);
			shader->setBool("blinn", rdSettings.enableBlinn);
			/*
			shader->setMat4("lightSpaceMatrix", LightManager::getShadowMapper()->getLightSpaceMatrix());
			std::vector<std::shared_ptr<Light>> relevantLights = LightManager::getRelevantLights(cam, 128);
			shader->setBool("useLighting", !relevantLights.empty());
			shader->setInt("numLights", relevantLights.size());

			*/

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

			/*
			for (size_t i = 0; i < relevantLights.size(); i++) {
				std::string base = "lights[" + std::to_string(i) + "].";
				shader->setVec3(base + "position", relevantLights[i]->getWorldPosition());
				shader->setVec3(base + "color", relevantLights[i]->getColor());
				m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());
			}
			*/

				///////////////////////				//MESH DRAW
				const MeshData& mesh = rdata.meshdatas[i];
				const Material& material = rdata.material[i];

				// Material properties
				shader->setVec3("objectColor", material.color.x, material.color.y, material.color.z);
				/*
								if ( useShadows) {
									glActiveTexture(GL_TEXTURE10);
									shader->setInt("shadowMap", 10);
									glBindTexture(GL_TEXTURE_2D, LightManager::getShadowMapper()->getDepthMapTexture());
								}
				*/
				// Material properties
				shader->setBool("useTexture", !material.textures.empty());

				


				//shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
				//shader->setMat4("lightSpaceMatrix", glm::mat4(1.0f));



				shader->setFloat("material.shininess", material.shininess);

				// Bind first diffuse texture if it exists
				for (const Texture& texture : material.textures) {
					switch (texture.type)
					{
					case Texture::Type::DIFFUSE:
						glActiveTexture(GL_TEXTURE0);
						shader->setInt("material.diffuse", 0);
						glBindTexture(GL_TEXTURE_2D, texture.id);
						break;
					case Texture::Type::NORMAL:
						glActiveTexture(GL_TEXTURE2);
						shader->setInt("material.normal", 2);
						glBindTexture(GL_TEXTURE_2D, texture.id);
						break;
					case Texture::Type::SPECULAR:
						glActiveTexture(GL_TEXTURE1);
						shader->setInt("material.specular", 1);
						glBindTexture(GL_TEXTURE_2D, texture.id);
						break;
					default:
						break;
					}
				}

				// Draw mesh
				glBindVertexArray(mesh.vao);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				// Reset to default texture unit
				glActiveTexture(GL_TEXTURE0);




			}

			/////////////////// END MESH DRAW ///////////////



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

	void setCubeTextures(RenderableHandle handle, const std::vector<Texture>& textures) {
		if (isValidHandle(handle))
			meshes[handle].material[0].textures =  textures ;

	}


}


