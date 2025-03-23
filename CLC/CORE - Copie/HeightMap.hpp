#include "GameObject.hpp"
#include "RenderComponent.hpp"

class HeightMap : public GameObject
{

	HeightMap(const char* path);
}; // class HeightMap


class HeightMapRenderer : public RenderComponent
{
	void init() override;
	void draw() override;
	void loadHeightMap(const char* path);

}; // class HeightMapRenderer