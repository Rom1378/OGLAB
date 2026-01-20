#pragma once
#include <cstdint>

using RenderableHandle = uint32_t;
class Scene;
class Texture;
class ShaderProgram;
struct Vertex;

namespace MeshRenderer {
	enum class Primitive { CUBE, SPHERE };

	constexpr RenderableHandle INVALID_HANDLE = 0;
	constexpr Texture* INVALID_TEXTURE = nullptr;//TODO: Texture* should become some kind of TextureHandle 
	constexpr ShaderProgram* INVALID_SHADER = nullptr;



	struct RenderableComponent {

		RenderableComponent(RenderableHandle handle);
		RenderableComponent(Primitive t);

		RenderableHandle rhandle{ INVALID_HANDLE };
		ShaderProgram* shader{ INVALID_SHADER };   // TODO: should become a handle
		Material material;                     // Value - unique per component // should become a kind of handle too

	};


	void init();

	RenderableHandle createCube();
	RenderableHandle createSphere();
	RenderableHandle createRenderableFromFile(const std::string& path);

	void destroy(RenderableHandle handle);

	void setTextures(const std::vector<Texture*>& textures);


	void renderPass(Scene* scene);

	//void DrawCube(Tran, );

	void cleanup();
}