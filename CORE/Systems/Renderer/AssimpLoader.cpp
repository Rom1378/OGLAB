#pragma once

#include "AssimpLoader.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <stdexcept>
#include "CORE/Debug.hpp"

#include "MeshRenderer.hpp"
#include "CORE/Mesh/Vertex.hpp"
#include "CORE/TextureManager.hpp"
#include "CORE/Shader.hpp"
#include "CORE/Systems/Renderer/MeshRenderer.hpp"

namespace assimpLoader {
	bool loadModel(MeshRenderer::RenderableData& rd, const std::string& path) {
		Assimp::Importer importer;

		try
		{
			const aiScene* scene = importer.ReadFile(path,
				aiProcess_Triangulate |
				aiProcess_GenSmoothNormals |
				aiProcess_FlipUVs |
				aiProcess_CalcTangentSpace);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
				throw std::runtime_error("Assimp error: " + std::string(importer.GetErrorString()));
			}

			std::string directory = path.substr(0, path.find_last_of('/'));
			processNode(rd, scene->mRootNode, scene, directory);


			// Print model information
			printModelInfo(rd);

			return true;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR(e.what());
			return false;
		}

	}

	void processNode(MeshRenderer::RenderableData& rd, aiNode* node, const aiScene* scene, const std::string& directory) {
		// Process all meshes in node
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			std::pair<MeshRenderer::MeshData, std::vector<Texture>> meshAndTexture = processMesh(mesh, scene, directory);

			MeshRenderer::Material m{
				.textures = meshAndTexture.second,
				.shader = &*ShaderManager::getShader("assimpModel")
			};
			rd.meshdatas.push_back(meshAndTexture.first);
			rd.material.push_back(m);


		}

		// Process children recursively
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(rd, node->mChildren[i], scene, directory);
		}
	}

	std::pair<MeshRenderer::MeshData, std::vector<Texture>> processMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory) {
		// Process vertices
		MeshRenderer::MeshData meshdata;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex{};

			// Position
			vertex.Position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);

			// Normals
			if (mesh->HasNormals()) {
				vertex.Normal = glm::vec3(
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				);
			}

			// Texture coordinates
			if (mesh->mTextureCoords[0]) {
				vertex.TexCoords = glm::vec2(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				);
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f);
			}
			meshdata.vertices.push_back(vertex);
		}

		// Process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				meshdata.indices.push_back(face.mIndices[j]);
			}
		}

		std::vector<Texture> textures;
		// Process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// Diffuse maps
			std::vector<Texture> diffuseMaps = loadMaterialTextures(
				material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// Specular maps
			std::vector<Texture> specularMaps = loadMaterialTextures(
				material, aiTextureType_SPECULAR, "texture_specular", directory);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return { meshdata, textures };
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
		aiTextureType type,
		const std::string& typeName, const std::string& directory) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			auto t = mat->GetTexture(type, i, &str);

			textures.push_back({
				.id = TextureFromFile(str.C_Str(), directory),
				.type = typeName,
				.path =  str.C_Str(),
				});
		}
		return textures;
	}

	unsigned int TextureFromFile(const char* path, const std::string& directory) {
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else {
			LOG_WARN("Texture failed to load at path : ", path);
			//std::cerr << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	void printModelInfo(const MeshRenderer::RenderableData& data) {
		LOG("Model Information:");

		std::cout << "  Meshes: " << data.meshdatas.size() << "\n";

		for (size_t i = 0; i < data.meshdatas.size(); i++) {
			std::cout << "  Mesh " << i << ":\n";
			std::cout << "    Vertices: " << data.meshdatas[i].vertices.size() << "\n";
			std::cout << "    Indices: " << data.meshdatas[i].indices.size() << "\n";
			std::cout << "    Textures: " << data.material[i].textures.size() << "\n";

			for (auto tex : data.material[i].textures) {
				std::cout << "      " << tex.type << ": " << tex.path << "\n";
			}
		}
	}
}