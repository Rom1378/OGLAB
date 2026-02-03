#pragma once
#include <glad/glad.h>
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "CORE/Mesh/Vertex.hpp"
#include <cstdint>

#include "CORE/TextureManager.hpp"

class ShaderProgram;
class Scene;



namespace MeshRenderer {
	enum class Primitive { CUBE, SPHERE };
	using RenderableHandle = uint32_t;
	constexpr RenderableHandle INVALID_HANDLE = 0;

	//constexpr Texture* INVALID_TEXTURE = nullptr;//TODO: Texture* should become some kind of TextureHandle 
	constexpr ShaderProgram* INVALID_SHADER = nullptr;



	struct RenderState {
		bool renderGeometryOnly{ false };
		bool shadowcaster{ true };
		bool hide{ false };
	};

	struct Material {
		std::vector<Texture> textures = {};
		// Textures are shared resources - use handles
		//TODO: Texture* should become some kind of TextureHandle
		//Texture* albedoTexture{ INVALID_TEXTURE };
		//Texture* normalMap{ INVALID_TEXTURE };
		//Texture* metallicRoughnessMap{ INVALID_TEXTURE };
		glm::vec4 color{ 1.0f };
		glm::vec4 albedoColor{ 1.0f };
		float shininess{ 64.0f };

		glm::vec3 ambient{ 1.0f, 0.5f, 0.31f };


		float metallic{ 0.0f };
		float roughness{ 0.5f };
		float emissive{ 0.0f };

		
	};

	struct MeshData {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		GLuint vbo{}, ebo{}, vao{};
		uint32_t indexCount;
	};


	struct RenderableData {
		std::vector<MeshData> meshdatas{};
		std::vector<Material> material{};
		ShaderProgram* shader{ nullptr };
		RenderState rdState{};
	};



	void init();

	RenderableHandle createCube();
	RenderableHandle createSphere(float radius=1, unsigned int sectorCount=5, unsigned int stackCount=5);
	RenderableHandle createRenderableFromFile(const std::string& path);

	bool isValidHandle(RenderableHandle handle);


	void destroyRenderable(RenderableHandle handle);

	void setCubeTextures(RenderableHandle handle, const std::vector<Texture>& textures);

	void renderPass(Scene* scene);

	const RenderableData& getRenderableData(RenderableHandle handle);


	//void DrawCube(Tran, );

	void cleanup();
}


