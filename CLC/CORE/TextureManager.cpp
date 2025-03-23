#include "TextureManager.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace TextureManager
{
	namespace Internal {
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	}

	std::shared_ptr<Texture> loadTexture(const std::string& path, const std::string& name)
	{
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





}