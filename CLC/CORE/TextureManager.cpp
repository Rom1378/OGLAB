#include "TextureManager.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.hpp"
#include "Mesh/CubeMap.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace TextureManager
{
	namespace Internal {
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	}

	std::shared_ptr<Texture> loadTexture(const std::string& path, const std::string& name)
	{
		//first check if the texture hasnt already been loaded :
		auto it = Internal::textures.find(name);
		if (it != Internal::textures.end())
		{
			return it->second;
		}

		auto texture = std::make_shared<Texture>();
		texture->id = 0;
		texture->width = 0;
		texture->height = 0;

		// Load texture
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glGenTextures(1, &texture->id);
			glBindTexture(GL_TEXTURE_2D, texture->id);
			// Set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Load and generate the texture
			if (nrChannels == 3)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cerr << "TextureManager: Unsupported number of channels: " << nrChannels << std::endl;
			}
			glGenerateMipmap(GL_TEXTURE_2D);
			texture->width = width;
			texture->height = height;
		}
		else
		{
			std::cerr << "TextureManager: Failed to load texture: " << path << std::endl;
		}
		stbi_image_free(data);

		Internal::textures[name] = texture;
		return texture;
	}


	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	std::shared_ptr<Texture> loadCubemap(const std::vector<std::string>& faces)
	{

		auto texture = std::make_shared<Texture>();
		texture->id = 0;
		texture->width = 0;
		texture->height = 0;

		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cerr << "TextureManager: Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return texture;
	}

	std::shared_ptr<Texture> loadCubemap(const std::vector<std::string>& faces, const std::string& name)
	{
		auto it = Internal::textures.find(name);
		if (it != Internal::textures.end())
		{
			return it->second;
		}

		auto texture = loadCubemap(faces);
		Internal::textures[name] = texture;
		return texture;
	}

	std::shared_ptr<Texture> getTexture(const std::string& name)
	{
		auto it = Internal::textures.find(name);
		if (it != Internal::textures.end())
		{
			return it->second;
		}
		return nullptr;
	}


	void deleteTexture(const std::string& name)
	{
		auto it = Internal::textures.find(name);
		if (it != Internal::textures.end())
		{
			glDeleteTextures(1, &it->second->id);
			Internal::textures.erase(it);
		}
	}

	void deleteTexture(std::shared_ptr<Texture> texture)
	{
		deleteTexture(texture->id);
	}

	void deleteTexture(unsigned int id)
	{
		for (auto it = Internal::textures.begin(); it != Internal::textures.end();)
		{
			if (it->second->id == id)
			{
				glDeleteTextures(1, &it->second->id);
				it = Internal::textures.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void clear()
	{
		for (auto& [name, texture] : Internal::textures)
		{
			glDeleteTextures(1, &texture->id);
		}
		Internal::textures.clear();
	}

	int calculateOptimalCubemapSize(int hdrWidth, int hdrHeight) {
		// Calculate based on HDR texture area
		const float sourcePixels = hdrWidth * hdrHeight;

		// These thresholds can be adjusted based on quality needs
		if (sourcePixels > 16000 * 8000) return 8192;  // 16K source
		if (sourcePixels > 8000 * 4000) return 4096;  // 8K source
		if (sourcePixels > 4000 * 2000) return 2048;  // 4K source
		if (sourcePixels > 2000 * 1000) return 1024;  // 2K source
		return 512;  // Default for smaller textures
	}


	HDRTextureInfo  TextureManager::loadHDRTextureWithInfo(const std::string& path) {
		HDRTextureInfo info;
		info.texture = std::make_shared<Texture>();

		stbi_set_flip_vertically_on_load(true);
		float* data = stbi_loadf(path.c_str(), &info.width, &info.height, nullptr, 3);

		if (data) {
			glGenTextures(1, &info.texture->id);
			glBindTexture(GL_TEXTURE_2D, info.texture->id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, info.width, info.height,
				0, GL_RGB, GL_FLOAT, data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else {
			info.width = info.height = 0;
			std::cerr << "Failed to load HDR image: " << path << std::endl;
		}


		return info;
	}

	std::shared_ptr<Texture> TextureManager::convertHDRToCubemap(const HDRTextureInfo& hdrInfo) {
		if (hdrInfo.width == 0 || hdrInfo.height == 0) return nullptr;

		// Calculate resolution based on source
		const int cubemapSize = calculateOptimalCubemapSize(hdrInfo.width, hdrInfo.height);
		std::cout << "Converting HDR (" << hdrInfo.width << "x" << hdrInfo.height
			<< ") to cubemap " << cubemapSize << "x" << cubemapSize << std::endl;

		// Create cubemap texture with calculated size
		auto cubemap = std::make_shared<Texture>();
		glGenTextures(1, &cubemap->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
				cubemapSize, cubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Changed!
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create framebuffer for rendering
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cubemapSize, cubemapSize); // Fixed!
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		// Check framebuffer completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer not complete!" << std::endl;
			return nullptr;
		}

		// Set up projection and view matrices
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};

		// Get shader - FIXED TYPO IN SHADER NAME
		auto shader = ShaderManager::getShader("equirectanular2cubemap");
		if (!shader) {
			std::cerr << "Failed to get equirectanular2cubemap" << std::endl;
			return nullptr;
		}
		shader->use();
		shader->setInt("equirectangularMap", 0);
		shader->setMat4("projection", glm::value_ptr(captureProjection));

		// Bind HDR texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrInfo.texture->id);

		// Set up cube VAO if not already set up
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO = 0;
		if (cubeVAO == 0) {
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			glBindVertexArray(cubeVAO);
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, MeshData::CubeMap::vertices.size() * sizeof(float),
				MeshData::CubeMap::vertices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glBindVertexArray(0);
		}

		// Render to each cubemap face - USING CORRECT VIEWPORT SIZE
		glViewport(0, 0, cubemapSize, cubemapSize);
		glBindVertexArray(cubeVAO);
		for (unsigned int i = 0; i < 6; ++i) {
			shader->setMat4("view", glm::value_ptr(captureViews[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap->id, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Generate mipmaps - CRUCIAL FOR GOOD QUALITY
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// Clean up
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteRenderbuffers(1, &captureRBO);
		glDeleteFramebuffers(1, &captureFBO);

		return cubemap;
	}

}