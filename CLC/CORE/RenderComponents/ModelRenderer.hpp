
#include "../RenderComponent.hpp"
#include "../LightManager.hpp"
#include <glm/glm.hpp>
#include "../ModelLoader/Mesh.hpp" 
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class ModelRenderer : public RenderComponent {
public:
	ModelRenderer(const std::string& path) : RenderComponent(), m_path(path) {	}
	void setPath(const std::string& path) { m_path = path; }
	void renderRawGeometry(const glm::mat4& lightSpaceMatrix) override;
	void renderWithMaterials(const std::shared_ptr<Camera>& cam) override;
	void init() override;
	~ModelRenderer();
	void draw(const std::shared_ptr<Camera> cam) override { renderWithMaterials(cam); }

private:

	void loadModel();
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	unsigned int TextureFromFile(const char* path, const std::string& directory);
	void printModelInfo() const;
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	std::string m_path;

};