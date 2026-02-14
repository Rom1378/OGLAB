#include "MeshRenderer.hpp"

#include "CORE/Mesh/CubeMesh.hpp"

#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Renderable.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Lights/LightManager.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"
#include "CORE/Systems/Renderer/ShadowMap.hpp"

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

			// Tangent attribute
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			glEnableVertexAttribArray(3);

			// Bitangent attribute
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			glEnableVertexAttribArray(4);

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	RenderableHandle createCube() {
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
		auto shadowView = scene->getComponentView<ShadowCasterComponent>();

		for (auto e : scene->getEntities()) {
			RenderableComponent* renderableComponent = scene->getComponent<RenderableComponent>(e);
			TransformComponent* Etransform = scene->getComponent<TransformComponent>(e);
			if (!renderableComponent)
				continue;
			if (!Etransform) {
				LOG_WARN("Entity having renderable component must have a transform component");
				continue;
			}

			Transform worldTransform = *Etransform * renderableComponent->localTransform;

			RenderableData rdata = meshes[renderableComponent->rhandle];
			ShaderProgram* shader = rdata.shader;

			if (!shader) {
				LOG_ERROR("RenderComponent must have a shader bound");
				continue;
			}

			shader->use();
			
			// Standard matrix uniforms (commun à tous les meshes)
			shader->setMat4("model", glm::value_ptr(worldTransform.getModelMatrix()));
			shader->setMat4("view", glm::value_ptr(Renderer::getView()));
			shader->setMat4("projection", glm::value_ptr(Renderer::getProjection()));
			shader->setVec3("viewPos", Renderer::getViewPosition());

			if (!rdSettings.geometryOnlyPass) {
				
				if (rdSettings.enableLighting)
					setLightUniforms(scene, *shader);
					
				if (rdSettings.enableShadows && ShadowMapSystem::hasShadowMap()) {
					ShadowMapSystem::bindShadowMaps(*shader);
					
					// Initialize all shadow indices to -1 (no shadow)
					for (int i = 0; i < MAX_DIR_LIGHTS; i++) {
						shader->setInt("dirLightShadowIndices[" + std::to_string(i) + "]", -1);
					}
					for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
						shader->setInt("pointLightShadowIndices[" + std::to_string(i) + "]", -1);
					}
					for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
						shader->setInt("spotLightShadowIndices[" + std::to_string(i) + "]", -1);
					}

					// Map lights to their shadow maps
					uint32_t shadowInfoIndex = 0;
					uint32_t dir2DIndex = 0, point2DIndex = 0, pointCubeIndex = 0, spot2DIndex = 0;

					for (auto [entity, shadowCaster] : shadowView) {
						auto lightComponent = scene->getComponent<LightComponent>(entity);
						auto transform = scene->getComponent<TransformComponent>(entity);

						if (lightComponent && transform) {
							shader->setVec3("shadowLightPositions[" + std::to_string(shadowInfoIndex) + "]", transform->pos);

							// Map light index to shadow map index based on type
							if (lightComponent->type == LightComponent::Type::DIR) {
								shader->setInt("dirLightShadowIndices[" + std::to_string(dir2DIndex) + "]", dir2DIndex);
								dir2DIndex++;
							}
							else if (lightComponent->type == LightComponent::Type::POINT) {
								shader->setFloat("shadowFarPlanes[" + std::to_string(pointCubeIndex) + "]", shadowCaster->far_plane);
								shader->setInt("pointLightShadowIndices[" + std::to_string(pointCubeIndex) + "]", pointCubeIndex);
								pointCubeIndex++;
							}
							else if (lightComponent->type == LightComponent::Type::SPOT) {
								shader->setInt("spotLightShadowIndices[" + std::to_string(spot2DIndex) + "]", dir2DIndex + spot2DIndex);
								spot2DIndex++;
							}

							shadowInfoIndex++;
						}
					}
				}
				
				shader->setBool("useLighting", rdSettings.enableLighting);
				shader->setBool("blinn", rdSettings.enableBlinn);
				shader->setBool("useShadow", rdSettings.enableShadows && ShadowMapSystem::hasShadowMap());
			}

			// Render chaque submesh avec son propre matériau
			for (uint32_t i = 0; i < rdata.meshdatas.size(); i++) {

				const MeshData& mesh = rdata.meshdatas[i];
				const Material& material = rdata.material[i];

				if (!rdSettings.geometryOnlyPass) {
					// Material properties
					shader->setVec3("objectColor", material.color.x, material.color.y, material.color.z);
					shader->setBool("useTexture", !material.textures.empty());
					shader->setFloat("material.shininess", material.shininess);

					for (int unit = 0; unit < 3; unit++) {
						glActiveTexture(GL_TEXTURE0 + unit);
						glBindTexture(GL_TEXTURE_2D, 0);
					}

					bool hasDiffuse = false;
					bool hasSpecular = false;
					bool hasNormal = false;

					for (const Texture& texture : material.textures) {
						switch (texture.type)
						{
						case Texture::Type::DIFFUSE:
							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, texture.id);
							shader->setInt("material.diffuse", 0);
							hasDiffuse = true;
							break;
						case Texture::Type::SPECULAR:
							glActiveTexture(GL_TEXTURE1);
							glBindTexture(GL_TEXTURE_2D, texture.id);
							shader->setInt("material.specular", 1);
							hasSpecular = true;
							break;
						case Texture::Type::NORMAL:
							glActiveTexture(GL_TEXTURE2);
							glBindTexture(GL_TEXTURE_2D, texture.id);
							shader->setInt("material.normal", 2);
							hasNormal = true;
							break;
						default:
							break;
						}
					}

					shader->setBool("hasDiffuseMap", hasDiffuse);
					shader->setBool("hasSpecularMap", hasSpecular);
					shader->setBool("hasNormalMap", hasNormal);
				}

				// Draw mesh
				glBindVertexArray(mesh.vao);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				Renderer::increaseDrawCallCount();
			}
			
			for (int unit = 0; unit < 3; unit++) {
				glActiveTexture(GL_TEXTURE0 + unit);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			
			glBindVertexArray(0);
		}
		

		glActiveTexture(GL_TEXTURE0);
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
			meshes[handle].material[0].textures = textures;

	}

	const RenderableData& getRenderableData(RenderableHandle handle) {
		return meshes[handle];
	}


}


