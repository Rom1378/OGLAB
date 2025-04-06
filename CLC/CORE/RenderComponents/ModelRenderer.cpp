#include "ModelRenderer.hpp"
#include "stb_image.h"

void ModelRenderer::renderRawGeometry(const glm::mat4& lightSpaceMatrix) {
	auto shader = ShaderManager::getShader("simpleDepthShader");
	shader->use();
	shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));

	for (auto& mesh : meshes) {
		mesh.drawRawGeometry();
	}
}

void ModelRenderer::renderWithMaterials(const std::shared_ptr<Camera>& cam) {

	if (!m_shader) return;

	m_shader->use();

	// Standard matrix uniforms
	m_shader->setMat4("model", glm::value_ptr(getGameObject()->getModelMatrix()));
	m_shader->setMat4("view", glm::value_ptr(cam->getViewMatrix()));
	m_shader->setMat4("projection", glm::value_ptr(cam->getProjectionMatrix()));
	m_shader->setMat4("lightSpaceMatrix", LightManager::getShadowMapper()->getLightSpaceMatrix());

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
		//m_shader->setInt(base + "type", lightType);
		m_shader->setVec3(base + "position", relevantLights[i]->getPosition());
		//m_shader->setVec3(base + "direction", relevantLights[i]->getDirection());
		//m_shader->setVec3(base + "color", relevantLights[i]->getColor());
		//m_shader->setFloat(base + "intensity", relevantLights[i]->getIntensity());

	}
	// Draw all meshes
	for (unsigned int i = 0; i < meshes.size(); i++) {

		meshes[i].Draw(m_shader, LightManager::getShadowMapper()->getLightSpaceMatrix(), true, true);
	}

}

void ModelRenderer::init() {
	// Load the model
	loadModel();
	m_shader = ShaderManager::getShader("assimpModel");
	m_isShadowCaster = true;
	m_isShadowReceiver = true;

}

ModelRenderer::~ModelRenderer() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void ModelRenderer::loadModel() {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(m_path,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw std::runtime_error("Assimp error: " + std::string(importer.GetErrorString()));
	}

	directory = m_path.substr(0, m_path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

	// Print model information
	printModelInfo();
}

void ModelRenderer::processNode(aiNode* node, const aiScene* scene) {
	// Process all meshes in node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// Process children recursively
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh ModelRenderer::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

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

		vertices.push_back(vertex);
	}

	// Process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(
			material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// Specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(
			material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> ModelRenderer::loadMaterialTextures(aiMaterial* mat,
	aiTextureType type,
	const std::string& typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);


		Texture texture;
		texture.id = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(texture);
	}
	return textures;
}

unsigned int ModelRenderer::TextureFromFile(const char* path, const std::string& directory) {
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
		std::cerr << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void ModelRenderer::printModelInfo() const {
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