#include "Scene.hpp"
#include "Engine.hpp"
#include <algorithm>

#include "CORE/Entity.hpp"
#include "CORE/EntityManager.hpp"

#include "CORE/Systems/Renderer/MeshRenderer.hpp"
#include "CORE/Systems/Renderer/Renderer.hpp"

#include "CORE/Systems/Input/Mouvement.hpp"

#include "CORE/Systems/Physics/Physics.hpp"
#include "CORE/Systems/TransformSync/Sync.hpp"

#include "CORE/Camera.hpp"

#include "CORE/Components/Camera.hpp"
#include "CORE/Components/Transform.hpp"
#include "CORE/Components/Physics.hpp"
#include "CORE/Components/Renderable.hpp"
#include "CORE/Components/Light.hpp"

void Scene::init() {
	cameraEntity= createEntity("Dev camera");

	addComponent<CameraComponent>(cameraEntity);
	addComponent<TransformComponent>(cameraEntity);
	
	// stores dx dt dz yaw pitch
	addComponent<CharacterControllerStateComponent>(cameraEntity);
	
	// simulate dx dt dz yaw pitch
	addComponent<PhysicsBodyComponent>(cameraEntity, PhysicsBodyComponent::PhysicsRole::CharacterController);




    LightComponent spotLight;
    spotLight.type = LightComponent::Type::SPOT;
    spotLight.data = SpotLightData();

    addComponent<LightComponent>(cameraEntity, spotLight);

	
	activeCamera = cameraEntity;
	activeEntity = cameraEntity; //will 

	skybox = Skybox::createSkybox({
		"res/textures/CubeMaps/skybox/right.jpg",
		"res/textures/CubeMaps/skybox/left.jpg",
		"res/textures/CubeMaps/skybox/top.jpg",
		"res/textures/CubeMaps/skybox/bottom.jpg",
		"res/textures/CubeMaps/skybox/front.jpg",
		"res/textures/CubeMaps/skybox/back.jpg"
		});

	Entity cube = createEntity("Cube");
	auto rdcomp = addComponent<RenderableComponent>(cube, MeshRenderer::createCube());

	Texture& cat = *TextureManager::loadTexture("res/textures/CAT.png", "cat");
	Texture& container = *TextureManager::loadTexture("res/textures/container/container2.png", "container");
	container.type = Texture::Type::DIFFUSE;
	cat.type = Texture::Type::DIFFUSE;
	Texture& specular = *TextureManager::loadTexture("res/textures/container/container2_specular.png", "container_specular");
	specular.type = Texture::Type::SPECULAR;

	MeshRenderer::setCubeTextures(rdcomp->rhandle, { container, specular});


	TransformComponent comp{  };
	comp.scale = { 1.0,3.0f,1.0f };
	comp.pos = { 0,-4,0 };
	addComponent<TransformComponent>(cube, comp);


    // Create cubes in a grid
    for (int x = -2; x <= 3; x++) {
        for (int z = -2; z <= 3; z++) {
            Entity cube = createEntity("Cube_" + std::to_string(x) + "_" + std::to_string(z));
            auto rdcomp = addComponent<RenderableComponent>(cube, MeshRenderer::createCube());
            PhysicsBodyComponent body{ PhysicsBodyComponent::PhysicsRole::Dynamic };
            ColliderComponent collider{ ColliderComponent::Type::Box };
            collider.localTransform.pos = { -0.2f,0,0};
            addComponent<PhysicsBodyComponent>(cube, body);
            addComponent<ColliderComponent>(cube, collider);

            collider.halfExtents = { 0.5,0.5,0.5 };
            MeshRenderer::setCubeTextures(rdcomp->rhandle, { container, specular });

            TransformComponent transform;
            transform.pos = glm::vec3(x * 3.0f, 100.0f, z * 3.0f);
            transform.scale = glm::vec3(1.0f);
            addComponent<TransformComponent>(cube, transform);
        }
    }

    // Floor
    Entity floor = createEntity("Floor");
    auto floorRd = addComponent<RenderableComponent>(floor, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent body{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent collider{ColliderComponent::Type::Box};
    collider.halfExtents = glm::vec3{ 100.,0.5,100 };
    addComponent<ColliderComponent>(floor, collider);
    auto physxCube = addComponent<PhysicsBodyComponent>(floor, body);
    MeshRenderer::setCubeTextures(floorRd->rhandle, { container, specular });
    TransformComponent floorTransform;
    floorTransform.pos = glm::vec3(0.0f, -3.0f, 0.0f);
    floorTransform.scale = glm::vec3(200.0f, 0.5f, 200.0f);
    addComponent<TransformComponent>(floor, floorTransform);

	Entity Sphere = createEntity("Sphere");
	auto sphereRd = addComponent<RenderableComponent>(Sphere, MeshRenderer::createSphere(1.0f, 32, 32));
	PhysicsBodyComponent sphereBody{ PhysicsBodyComponent::PhysicsRole::Dynamic };
	ColliderComponent sphereCollider{ ColliderComponent::Type::Sphere };
	sphereCollider.radius = 1.0f;

	addComponent<ColliderComponent>(Sphere, sphereCollider);
	auto physxSphere = addComponent<PhysicsBodyComponent>(Sphere, sphereBody);
	MeshRenderer::setCubeTextures(sphereRd->rhandle, { container, specular });
	TransformComponent sphereTransform;
	sphereTransform.pos = glm::vec3(5.0f, 250.0f, 0.0f);
	addComponent<TransformComponent>(Sphere, sphereTransform);





    //load a 3D Model
    Entity paleVisitor = createEntity("PaleVisitor");


    addComponent<RenderableComponent>(paleVisitor, "res/3DModels/paleVisitor/source/PaleVisitor.fbx");
    auto t = addComponent<TransformComponent>(paleVisitor);
    t->pos = glm::vec3(-5.0f, 1.0f, 3.0f);


    //load a 3D Model
    Entity knife = createEntity("Knife");


    addComponent<RenderableComponent>(knife, "res/3DModels/knife/source/Knife_low.fbx");
    auto transfrom = addComponent<TransformComponent>(knife);
    transfrom->pos = glm::vec3(-5.0f, 1.0f, 0.0f);


    



    // ================= LIGHTS (BRIGHTER) =================

    // Point Light 1 - White (center, moving in circle)
    Entity pointLight1 = createEntity("Point Light 1 (White)");
    LightComponent light1;
    light1.type = LightComponent::Type::POINT;
    light1.ambient = glm::vec3(0.3f);
    light1.diffuse = glm::vec3(2.0f);  // Brighter!
    light1.specular = glm::vec3(2.0f);
    light1.data = PointLightData{ 1.0f, 0.07f, 0.017f };  // Less attenuation
    addComponent<LightComponent>(pointLight1, light1);
    TransformComponent light1Transform;
    light1Transform.pos = glm::vec3(0.0f, 5.0f, 0.0f);
    addComponent<TransformComponent>(pointLight1, light1Transform);

    // Point Light 2 - Red (moving)
    Entity pointLight2 = createEntity("Point Light 2 (Red)");
    LightComponent light2;
    light2.type = LightComponent::Type::POINT;
    light2.ambient = glm::vec3(0.1f, 0.0f, 0.0f);
    light2.diffuse = glm::vec3(3.0f, 0.0f, 0.0f);
    light2.specular = glm::vec3(1.5f, 0.0f, 0.0f);
    light2.data = PointLightData{ 1.0f, 0.07f, 0.017f };
    addComponent<LightComponent>(pointLight2, light2);
    TransformComponent light2Transform;
    light2Transform.pos = glm::vec3(-8.0f, 3.0f, 0.0f);
    addComponent<TransformComponent>(pointLight2, light2Transform);

    // Point Light 3 - Blue (moving)
    Entity pointLight3 = createEntity("Point Light 3 (Blue)");
    LightComponent light3;
    light3.type = LightComponent::Type::POINT;
    light3.ambient = glm::vec3(0.0f, 0.0f, 0.1f);
    light3.diffuse = glm::vec3(0.0f, 0.0f, 3.0f);
    light3.specular = glm::vec3(0.0f, 0.0f, 1.5f);
    light3.data = PointLightData{ 1.0f, 0.07f, 0.017f };
    addComponent<LightComponent>(pointLight3, light3);
    TransformComponent light3Transform;
    light3Transform.pos = glm::vec3(8.0f, 3.0f, 0.0f);
    addComponent<TransformComponent>(pointLight3, light3Transform);

    // Point Light 4 - Green (moving)
    Entity pointLight4 = createEntity("Point Light 4 (Green)");
    LightComponent light4;
    light4.type = LightComponent::Type::POINT;
    light4.ambient = glm::vec3(0.0f, 0.1f, 0.0f);
    light4.diffuse = glm::vec3(0.0f, 3.0f, 0.0f);
    light4.specular = glm::vec3(0.0f, 1.5f, 0.0f);
    light4.data = PointLightData{ 1.0f, 0.07f, 0.017f };
    addComponent<LightComponent>(pointLight4, light4);
    TransformComponent light4Transform;
    light4Transform.pos = glm::vec3(0.0f, 3.0f, 8.0f);
    addComponent<TransformComponent>(pointLight4, light4Transform);

    // Directional Light - Subtle ambient
    Entity dirLight = createEntity("Directional Light");
    LightComponent dirLightComp;
    dirLightComp.type = LightComponent::Type::DIR;
    dirLightComp.ambient = glm::vec3(0.1f);
    dirLightComp.diffuse = glm::vec3(0.4f);
    dirLightComp.specular = glm::vec3(0.5f);
    //dirLightComp.data = DirLightData{ glm::vec3(-0.2f, -1.0f, -0.3f) };
    addComponent<LightComponent>(dirLight, dirLightComp);


}

void Scene::update(float dt) {

    //lights move
    {
        float time = glfwGetTime();

        // Get light entities
        auto lightView = getComponentView<LightComponent>();

        int lightIndex = 0;
        for (auto [entity, light] : lightView) {
            if (light->type != LightComponent::Type::POINT) continue;

            TransformComponent* transform = getComponent<TransformComponent>(entity);
            if (!transform) continue;

            // Each light moves in a different pattern
            float radius = 8.0f;
            float speed = 0.5f;
            float offset = lightIndex * glm::pi<float>() / 2.0f;  // 90 degree offset each

            transform->pos.x = radius * glm::cos(time * speed + offset);
            transform->pos.z = radius * glm::sin(time * speed + offset);
            transform->pos.y = 3.0f + 2.0f * glm::sin(time * speed * 2.0f + offset);

            lightIndex++;
        }

    }











// 1 input system
// 2 simulate physx
// 3 sync
// 4 update 3d

	if (Input::isMouseLocked())
		InputSystem::update(this, dt);
	PhysicsSystem::update(this, dt);
	TransformSyncSystem::update(this);
//	PhysicsStep(world, dt);
	//PlayerMovementSystem(world, dt);
	//RenderSystem(world);

	Renderer::update(dt);

    CameraComponent* camComp = getComponent<CameraComponent>(activeCamera);
	TransformComponent* transform = getComponent<TransformComponent>(activeCamera);

    const Camera* cam;
    if (camComp && transform) {
        cam = &camComp->cam;
        if (cam) 
        {
            Transform world = *transform * camComp->localTransform;
            Renderer::setViewProjection(world.getViewMatrix(), cam->getProjection());

        }
    }
    else {
        LOG_WARN("No Active CameraComponent or Transform component!!!");
    }


	onUpdate();
}

Entity Scene::createEntity() {
	entities.push_back(EntityManager::Create());
	return entities.back();
}

Entity Scene::createEntity(std::string name) {
    Entity e = createEntity();
	addComponent<NameComponent>(e, name);
    return e;
}

Entity Scene::createEntity(const char* name) {
    Entity e = createEntity();
	addComponent<NameComponent>(e, name);
    return e;
}

bool Scene::DestroyEntity(Entity e) {
	auto it = std::find(entities.begin(), entities.end(), e);
	if (it == entities.end()) {
		return false;
	}
	return EntityManager::Destroy(e);
}
