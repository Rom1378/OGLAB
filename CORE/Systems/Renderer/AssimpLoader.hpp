#pragma once

#include <vector>
#include <string>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Texture;

namespace assimpLoader {
	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		aiTextureType type,
		const std::string& typeName);

	unsigned int TextureFromFile(const char* path, const std::string& directory);
	void printModelInfo();
}