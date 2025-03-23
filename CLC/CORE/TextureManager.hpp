#pragma once

#include <string>
#include <unordered_map>
#include <memory>

struct Texture
{
	unsigned int id;
	int width, height;
};


namespace TextureManager
{
	std::shared_ptr<Texture> loadTexture(const std::string& path, const std::string& name);
	std::shared_ptr<Texture> getTexture(const std::string& name);
	void deleteTexture(const std::string& name);
	void deleteTexture(std::shared_ptr<Texture> texture);
	void deleteTexture(unsigned int id);
	void clear();



}
