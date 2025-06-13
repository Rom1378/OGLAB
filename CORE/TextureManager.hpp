#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

struct Texture
{
	unsigned int id;
	int width, height;

	std::string type;
	std::string path; // we store the path of the texture to compare with other textures (e.g. when loading a model)


};

struct HDRTextureInfo {
	std::shared_ptr<Texture> texture;
	int width;
	int height;
};

namespace TextureManager
{
	std::shared_ptr<Texture> loadTexture(const std::string& path, const std::string& name);
	std::shared_ptr<Texture> loadCubemap(const std::vector<std::string>& faces, const std::string& name);
	std::shared_ptr<Texture> loadCubemap(const std::vector<std::string>& faces);

	HDRTextureInfo loadHDRTextureWithInfo(const std::string& path);

	std::shared_ptr<Texture> convertHDRToCubemap(const HDRTextureInfo& hdrInfo);

	std::shared_ptr<Texture> getTexture(const std::string& name);
	void deleteTexture(const std::string& name);
	void deleteTexture(std::shared_ptr<Texture> texture);
	void deleteTexture(unsigned int id);
	void clear();
}
