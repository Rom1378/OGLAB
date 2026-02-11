#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>

#include "CORE/TextureManager.hpp"

namespace MeshRenderer{
struct RenderableData;
struct MeshData;
}


namespace assimpLoader {

	bool loadModel(MeshRenderer::RenderableData& rd, const std::string& path);
	void processNode(MeshRenderer::RenderableData& rd, aiNode* node, const aiScene* scene, const std::string& directory);
	std::pair<MeshRenderer::MeshData, std::vector<Texture>> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		aiTextureType type,
		Texture::Type typeName, const std::string& directory);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool useSRGB);
	void printModelInfo(const MeshRenderer::RenderableData& data);

}