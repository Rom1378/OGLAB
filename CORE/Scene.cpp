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
    cameraEntity = createEntity("Dev camera");

    addComponent<CameraComponent>(cameraEntity);
    addComponent<TransformComponent>(cameraEntity);

    // stores dx dt dz yaw pitch
    addComponent<CharacterControllerStateComponent>(cameraEntity);

    // simulate dx dt dz yaw pitch
    addComponent<PhysicsBodyComponent>(cameraEntity, PhysicsBodyComponent::PhysicsRole::CharacterController);

    LightComponent spotLight =
    {
        .type = LightComponent::Type::SPOT,
        .ambient = { 0.1f,0.1f,0.1f },
		.diffuse = { 1.0f,1.0f,1.0f },
        .specular = { 1.0f,1.0f,1.0f },
        .localTransform = Transform{ {0.0f,0.0f,1.0f}, {1.0f,0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f} },
        .data = SpotLightData{
            .cutOff = glm::cos(glm::radians(12.5f)),
            .outerCutOff = glm::cos(glm::radians(15.0f)),
            .constant = 1.0f,
            .linear = 0.09f,
            .quadratic = 0.032f
		}
    };
   
    addComponent<LightComponent>(cameraEntity, spotLight);

	activeCamera = cameraEntity;
	activeEntity = cameraEntity;

	skybox = Skybox::createSkybox({
		"res/textures/CubeMaps/skybox/right.jpg",
		"res/textures/CubeMaps/skybox/left.jpg",
		"res/textures/CubeMaps/skybox/top.jpg",
		"res/textures/CubeMaps/skybox/bottom.jpg",
		"res/textures/CubeMaps/skybox/front.jpg",
		"res/textures/CubeMaps/skybox/back.jpg"
	});

	// Load textures
	// ========== TEXTURES DIFFUSE/ALBEDO - sRGB = TRUE ==========
	Texture& cat = *TextureManager::loadTexture("res/textures/CAT.png", "cat", true);
	Texture& wood = *TextureManager::loadTexture("res/textures/NICEWOOD.png", "wood", true);
	Texture& container = *TextureManager::loadTexture("res/textures/container/container2.png", "container", true);
	Texture& brickDiffuse = *TextureManager::loadTexture("res/textures/brick_wall.jpg", "brick_diffuse", true);

	brickDiffuse.type = Texture::Type::DIFFUSE;
	wood.type = Texture::Type::DIFFUSE;
	container.type = Texture::Type::DIFFUSE;
	cat.type = Texture::Type::DIFFUSE;

	// ========== TEXTURES NON-COLOR - sRGB = FALSE ==========
	Texture& brickNormalMap = *TextureManager::loadTexture("res/textures/normal_map_brick_wall.jpg", "brick_normal", false);
	Texture& specular = *TextureManager::loadTexture("res/textures/container/container2_specular.png", "container_specular", false);

	brickNormalMap.type = Texture::Type::NORMAL;
	specular.type = Texture::Type::SPECULAR;

    // ========== MAIN FLOOR ==========
    Entity mainFloor = createEntity("Main Floor");
    auto mainFloorRd = addComponent<RenderableComponent>(mainFloor, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent floorBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent floorCollider{ ColliderComponent::Type::Box };
    floorCollider.halfExtents = glm::vec3{ 50.0f, 0.5f, 50.0f };
    addComponent<ColliderComponent>(mainFloor, floorCollider);
    addComponent<PhysicsBodyComponent>(mainFloor, floorBody);
    MeshRenderer::setCubeTextures(mainFloorRd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent mainFloorTransform;
    mainFloorTransform.pos = glm::vec3(0.0f, 0.0f, 0.0f);
    mainFloorTransform.scale = glm::vec3(100.0f, 1.0f, 100.0f);
    addComponent<TransformComponent>(mainFloor, mainFloorTransform);

    // ========== CENTRAL ROOM (15x15) ==========
    // North Wall
    Entity centralNorthWall = createEntity("Central Room - North Wall");
    auto cnwRd = addComponent<RenderableComponent>(centralNorthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cnwBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cnwCollider{ ColliderComponent::Type::Box };
    cnwCollider.halfExtents = glm::vec3{ 7.5f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(centralNorthWall, cnwCollider);
    addComponent<PhysicsBodyComponent>(centralNorthWall, cnwBody);
    MeshRenderer::setCubeTextures(cnwRd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cnwTransform;
    cnwTransform.pos = glm::vec3(0.0f, 3.0f, -7.5f);
    cnwTransform.scale = glm::vec3(15.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(centralNorthWall, cnwTransform);

    // South Wall
    Entity centralSouthWall = createEntity("Central Room - South Wall");
    auto cswRd = addComponent<RenderableComponent>(centralSouthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cswBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cswCollider{ ColliderComponent::Type::Box };
    cswCollider.halfExtents = glm::vec3{ 7.5f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(centralSouthWall, cswCollider);
    addComponent<PhysicsBodyComponent>(centralSouthWall, cswBody);
    MeshRenderer::setCubeTextures(cswRd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cswTransform;
    cswTransform.pos = glm::vec3(0.0f, 3.0f, 7.5f);
    cswTransform.scale = glm::vec3(15.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(centralSouthWall, cswTransform);

    // East Wall (with doorway)
    Entity centralEastWall1 = createEntity("Central Room - East Wall 1");
    auto cew1Rd = addComponent<RenderableComponent>(centralEastWall1, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cew1Body{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cew1Collider{ ColliderComponent::Type::Box };
    cew1Collider.halfExtents = glm::vec3{ 0.5f, 3.0f, 3.0f };
    addComponent<ColliderComponent>(centralEastWall1, cew1Collider);
    addComponent<PhysicsBodyComponent>(centralEastWall1, cew1Body);
    MeshRenderer::setCubeTextures(cew1Rd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cew1Transform;
    cew1Transform.pos = glm::vec3(7.5f, 3.0f, -4.5f);
    cew1Transform.scale = glm::vec3(1.0f, 6.0f, 6.0f);
    addComponent<TransformComponent>(centralEastWall1, cew1Transform);

    Entity centralEastWall2 = createEntity("Central Room - East Wall 2");
    auto cew2Rd = addComponent<RenderableComponent>(centralEastWall2, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cew2Body{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cew2Collider{ ColliderComponent::Type::Box };
    cew2Collider.halfExtents = glm::vec3{ 0.5f, 3.0f, 3.0f };
    addComponent<ColliderComponent>(centralEastWall2, cew2Collider);
    addComponent<PhysicsBodyComponent>(centralEastWall2, cew2Body);
    MeshRenderer::setCubeTextures(cew2Rd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cew2Transform;
    cew2Transform.pos = glm::vec3(7.5f, 3.0f, 4.5f);
    cew2Transform.scale = glm::vec3(1.0f, 6.0f, 6.0f);
    addComponent<TransformComponent>(centralEastWall2, cew2Transform);

    // West Wall (with doorway)
    Entity centralWestWall1 = createEntity("Central Room - West Wall 1");
    auto cww1Rd = addComponent<RenderableComponent>(centralWestWall1, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cww1Body{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cww1Collider{ ColliderComponent::Type::Box };
    cww1Collider.halfExtents = glm::vec3{ 0.5f, 3.0f, 3.0f };
    addComponent<ColliderComponent>(centralWestWall1, cww1Collider);
    addComponent<PhysicsBodyComponent>(centralWestWall1, cww1Body);
    MeshRenderer::setCubeTextures(cww1Rd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cww1Transform;
    cww1Transform.pos = glm::vec3(-7.5f, 3.0f, -4.5f);
    cww1Transform.scale = glm::vec3(1.0f, 6.0f, 6.0f);
    addComponent<TransformComponent>(centralWestWall1, cww1Transform);

    Entity centralWestWall2 = createEntity("Central Room - West Wall 2");
    auto cww2Rd = addComponent<RenderableComponent>(centralWestWall2, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent cww2Body{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent cww2Collider{ ColliderComponent::Type::Box };
    cww2Collider.halfExtents = glm::vec3{ 0.5f, 3.0f, 3.0f };
    addComponent<ColliderComponent>(centralWestWall2, cww2Collider);
    addComponent<PhysicsBodyComponent>(centralWestWall2, cww2Body);
    MeshRenderer::setCubeTextures(cww2Rd->rhandle, { brickDiffuse, brickNormalMap });
    TransformComponent cww2Transform;
    cww2Transform.pos = glm::vec3(-7.5f, 3.0f, 4.5f);
    cww2Transform.scale = glm::vec3(1.0f, 6.0f, 6.0f);
    addComponent<TransformComponent>(centralWestWall2, cww2Transform);

    // ========== EAST ROOM (12x10) ==========
    Entity eastNorthWall = createEntity("East Room - North Wall");
    auto enwRd = addComponent<RenderableComponent>(eastNorthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent enwBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent enwCollider{ ColliderComponent::Type::Box };
    enwCollider.halfExtents = glm::vec3{ 6.0f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(eastNorthWall, enwCollider);
    addComponent<PhysicsBodyComponent>(eastNorthWall, enwBody);
    MeshRenderer::setCubeTextures(enwRd->rhandle, { wood });
    TransformComponent enwTransform;
    enwTransform.pos = glm::vec3(14.0f, 3.0f, -5.0f);
    enwTransform.scale = glm::vec3(12.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(eastNorthWall, enwTransform);

    Entity eastSouthWall = createEntity("East Room - South Wall");
    auto eswRd = addComponent<RenderableComponent>(eastSouthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent eswBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent eswCollider{ ColliderComponent::Type::Box };
    eswCollider.halfExtents = glm::vec3{ 6.0f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(eastSouthWall, eswCollider);
    addComponent<PhysicsBodyComponent>(eastSouthWall, eswBody);
    MeshRenderer::setCubeTextures(eswRd->rhandle, { wood });
    TransformComponent eswTransform;
    eswTransform.pos = glm::vec3(14.0f, 3.0f, 5.0f);
    eswTransform.scale = glm::vec3(12.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(eastSouthWall, eswTransform);

    Entity eastEastWall = createEntity("East Room - East Wall");
    auto eewRd = addComponent<RenderableComponent>(eastEastWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent eewBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent eewCollider{ ColliderComponent::Type::Box };
    eewCollider.halfExtents = glm::vec3{ 0.5f, 3.0f, 5.0f };
    addComponent<ColliderComponent>(eastEastWall, eewCollider);
    addComponent<PhysicsBodyComponent>(eastEastWall, eewBody);
    MeshRenderer::setCubeTextures(eewRd->rhandle, { wood });
    TransformComponent eewTransform;
    eewTransform.pos = glm::vec3(20.0f, 3.0f, 0.0f);
    eewTransform.scale = glm::vec3(1.0f, 6.0f, 10.0f);
    addComponent<TransformComponent>(eastEastWall, eewTransform);

    // ========== WEST ROOM (10x10) ==========
    Entity westNorthWall = createEntity("West Room - North Wall");
    auto wnwRd = addComponent<RenderableComponent>(westNorthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent wnwBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent wnwCollider{ ColliderComponent::Type::Box };
    wnwCollider.halfExtents = glm::vec3{ 5.0f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(westNorthWall, wnwCollider);
    addComponent<PhysicsBodyComponent>(westNorthWall, wnwBody);
    MeshRenderer::setCubeTextures(wnwRd->rhandle, { container, specular });
    TransformComponent wnwTransform;
    wnwTransform.pos = glm::vec3(-12.5f, 3.0f, -5.0f);
    wnwTransform.scale = glm::vec3(10.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(westNorthWall, wnwTransform);

    Entity westSouthWall = createEntity("West Room - South Wall");
    auto wswRd = addComponent<RenderableComponent>(westSouthWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent wswBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent wswCollider{ ColliderComponent::Type::Box };
    wswCollider.halfExtents = glm::vec3{ 5.0f, 3.0f, 0.5f };
    addComponent<ColliderComponent>(westSouthWall, wswCollider);
    addComponent<PhysicsBodyComponent>(westSouthWall, wswBody);
    MeshRenderer::setCubeTextures(wswRd->rhandle, { container, specular });
    TransformComponent wswTransform;
    wswTransform.pos = glm::vec3(-12.5f, 3.0f, 5.0f);
    wswTransform.scale = glm::vec3(10.0f, 6.0f, 1.0f);
    addComponent<TransformComponent>(westSouthWall, wswTransform);

    Entity westWestWall = createEntity("West Room - West Wall");
    auto wwwRd = addComponent<RenderableComponent>(westWestWall, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent wwwBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent wwwCollider{ ColliderComponent::Type::Box };
    wwwCollider.halfExtents = glm::vec3{ 0.5f, 3.0f, 5.0f };
    addComponent<ColliderComponent>(westWestWall, wwwCollider);
    addComponent<PhysicsBodyComponent>(westWestWall, wwwBody);
    MeshRenderer::setCubeTextures(wwwRd->rhandle, { container, specular });
    TransformComponent wwwTransform;
    wwwTransform.pos = glm::vec3(-17.5f, 3.0f, 0.0f);
    wwwTransform.scale = glm::vec3(1.0f, 6.0f, 10.0f);
    addComponent<TransformComponent>(westWestWall, wwwTransform);

    // ========== DECORATIVE OBJECTS ==========
    // Central pedestal
    Entity pedestal = createEntity("Pedestal");
    auto pedestalRd = addComponent<RenderableComponent>(pedestal, MeshRenderer::Primitive::CUBE);
    PhysicsBodyComponent pedestalBody{ PhysicsBodyComponent::PhysicsRole::Static };
    ColliderComponent pedestalCollider{ ColliderComponent::Type::Box };
    pedestalCollider.halfExtents = glm::vec3{ 0.75f, 1.5f, 0.75f };
    addComponent<ColliderComponent>(pedestal, pedestalCollider);
    addComponent<PhysicsBodyComponent>(pedestal, pedestalBody);
    MeshRenderer::setCubeTextures(pedestalRd->rhandle, { wood });
    TransformComponent pedestalTransform;
    pedestalTransform.pos = glm::vec3(0.0f, 1.5f, 0.0f);
    pedestalTransform.scale = glm::vec3(1.5f, 3.0f, 1.5f);
    addComponent<TransformComponent>(pedestal, pedestalTransform);

    // Display sphere on pedestal
    Entity displaySphere = createEntity("Display Sphere");
    auto displaySphereRd = addComponent<RenderableComponent>(displaySphere, MeshRenderer::createSphere(0.8f, 32, 32));
    MeshRenderer::setCubeTextures(displaySphereRd->rhandle, { cat });
    TransformComponent displaySphereTransform;
    displaySphereTransform.pos = glm::vec3(0.0f, 4.0f, 0.0f);
    addComponent<TransformComponent>(displaySphere, displaySphereTransform);

    // Physics objects in East Room
    for (int i = 0; i < 5; i++) {
        Entity cube = createEntity("Physics Cube " + std::to_string(i));
        auto rdcomp = addComponent<RenderableComponent>(cube, MeshRenderer::createCube());
        PhysicsBodyComponent body{ PhysicsBodyComponent::PhysicsRole::Dynamic };
        ColliderComponent collider{ ColliderComponent::Type::Box };
        collider.halfExtents = { 0.5f, 0.5f, 0.5f };
        addComponent<PhysicsBodyComponent>(cube, body);
        addComponent<ColliderComponent>(cube, collider);
        MeshRenderer::setCubeTextures(rdcomp->rhandle, { container, specular });
        
        TransformComponent transform;
        transform.pos = glm::vec3(14.0f, 10.0f + i * 2.0f, -2.0f + i * 0.5f);
        transform.scale = glm::vec3(1.0f);
        addComponent<TransformComponent>(cube, transform);
    }

    // Physics spheres in West Room
    for (int i = 0; i < 3; i++) {
        Entity sphere = createEntity("Physics Sphere " + std::to_string(i));
        auto sphereRd = addComponent<RenderableComponent>(sphere, MeshRenderer::createSphere(0.6f, 32, 32));
        PhysicsBodyComponent sphereBody{ PhysicsBodyComponent::PhysicsRole::Dynamic };
        ColliderComponent sphereCollider{ ColliderComponent::Type::Sphere };
        sphereCollider.radius = 0.6f;
        addComponent<ColliderComponent>(sphere, sphereCollider);
        addComponent<PhysicsBodyComponent>(sphere, sphereBody);
        MeshRenderer::setCubeTextures(sphereRd->rhandle, { wood });
        
        TransformComponent sphereTransform;
        sphereTransform.pos = glm::vec3(-12.5f, 8.0f + i * 3.0f, i * 1.5f);
        addComponent<TransformComponent>(sphere, sphereTransform);
    }

    // ========== 3D MODELS ==========
    Entity paleVisitor = createEntity("PaleVisitor");
    addComponent<RenderableComponent>(paleVisitor, "res/3DModels/paleVisitor/source/PaleVisitor.fbx");
    auto pvTransform = addComponent<TransformComponent>(paleVisitor);
    pvTransform->pos = glm::vec3(14.0f, 1.0f, 0.0f);

    Entity knife = createEntity("Knife");
    addComponent<RenderableComponent>(knife, "res/3DModels/knife/source/Knife_low.fbx");
    auto knifeTransform = addComponent<TransformComponent>(knife);
    knifeTransform->pos = glm::vec3(-12.5f, 1.5f, 0.0f);

    // ========== LIGHTING ==========
    // Central room - warm white overhead
    Entity centralLight = createEntity("Central Room Light");
    LightComponent centralLightComp;
    centralLightComp.type = LightComponent::Type::POINT;
    centralLightComp.ambient = glm::vec3(0.2f, 0.2f, 0.15f);
    centralLightComp.diffuse = glm::vec3(1.5f, 1.4f, 1.2f);
    centralLightComp.specular = glm::vec3(1.0f);
    centralLightComp.data = PointLightData{ 1.0f, 0.045f, 0.0075f };
    addComponent<LightComponent>(centralLight, centralLightComp);
    auto centralLightShadow = addComponent<ShadowCasterComponent>(centralLight);
    TransformComponent centralLightTransform;
    centralLightTransform.pos = glm::vec3(0.0f, 5.5f, 0.0f);
    addComponent<TransformComponent>(centralLight, centralLightTransform);
    auto centralLightSphere = addComponent<RenderableComponent>(centralLight, MeshRenderer::createSphere(0.15f, 8, 8));

    // East room - cool blue light
    Entity eastLight = createEntity("East Room Light");
    LightComponent eastLightComp;
    eastLightComp.type = LightComponent::Type::POINT;
    eastLightComp.ambient = glm::vec3(0.05f, 0.05f, 0.1f);
    eastLightComp.diffuse = glm::vec3(0.3f, 0.5f, 1.5f);
    eastLightComp.specular = glm::vec3(0.5f, 0.7f, 1.0f);
    eastLightComp.data = PointLightData{ 1.0f, 0.07f, 0.017f };
    addComponent<LightComponent>(eastLight, eastLightComp);
    TransformComponent eastLightTransform;
    eastLightTransform.pos = glm::vec3(14.0f, 5.0f, 0.0f);
    addComponent<TransformComponent>(eastLight, eastLightTransform);
    auto eastLightSphere = addComponent<RenderableComponent>(eastLight, MeshRenderer::createSphere(0.15f, 8, 8));

    // West room - warm orange light
    Entity westLight = createEntity("West Room Light");
    LightComponent westLightComp;
    westLightComp.type = LightComponent::Type::POINT;
    westLightComp.ambient = glm::vec3(0.1f, 0.05f, 0.0f);
    westLightComp.diffuse = glm::vec3(1.5f, 0.5f, 0.1f);
    westLightComp.specular = glm::vec3(1.0f, 0.5f, 0.0f);
    westLightComp.data = PointLightData{ 1.0f, 0.07f, 0.017f };
    addComponent<LightComponent>(westLight, westLightComp);
    TransformComponent westLightTransform;
    westLightTransform.pos = glm::vec3(-12.5f, 5.0f, 0.0f);
    addComponent<TransformComponent>(westLight, westLightTransform);
    auto westLightSphere = addComponent<RenderableComponent>(westLight, MeshRenderer::createSphere(0.15f, 8, 8));

    // Accent light - green (moving)
    Entity accentLight = createEntity("Accent Light (Green)");
    LightComponent accentLightComp;
    accentLightComp.type = LightComponent::Type::POINT;
    accentLightComp.ambient = glm::vec3(0.0f, 0.05f, 0.0f);
    accentLightComp.diffuse = glm::vec3(0.0f, 2.0f, 0.0f);
    accentLightComp.specular = glm::vec3(0.0f, 1.0f, 0.0f);
    accentLightComp.data = PointLightData{ 1.0f, 0.09f, 0.032f };
    addComponent<LightComponent>(accentLight, accentLightComp);
    TransformComponent accentLightTransform;
    accentLightTransform.pos = glm::vec3(0.0f, 3.0f, 0.0f);
    addComponent<TransformComponent>(accentLight, accentLightTransform);
    auto accentLightSphere = addComponent<RenderableComponent>(accentLight, MeshRenderer::createSphere(0.12f, 8, 8));

    // Directional light - ambient scene lighting
    Entity dirLight = createEntity("Directional Light");
    LightComponent dirLightComp;
    dirLightComp.type = LightComponent::Type::DIR;
    dirLightComp.ambient = glm::vec3(0.15f, 0.15f, 0.2f);
    dirLightComp.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
    dirLightComp.specular = glm::vec3(0.2f);
    addComponent<LightComponent>(dirLight, dirLightComp);
}

void Scene::update(float dt) {
    // Animate accent light in a circle around central pedestal
    float time = glfwGetTime();
    auto lightView = getComponentView<LightComponent>();

    for (auto [entity, light] : lightView) {
        if (light->type != LightComponent::Type::POINT) continue;

        NameComponent* name = getComponent<NameComponent>(entity);
        if (!name || name->name.find("Accent") == std::string::npos) continue;

        TransformComponent* transform = getComponent<TransformComponent>(entity);
        if (!transform) continue;

        float radius = 5.0f;
        float speed = 1.2f;

        transform->pos.x = radius * glm::cos(time * speed);
        transform->pos.z = radius * glm::sin(time * speed);
        transform->pos.y = 3.0f + 1.5f * glm::sin(time * speed * 2.0f);
    }

	if (Input::isMouseLocked())
		InputSystem::update(this, dt);
	PhysicsSystem::update(this, dt);
	TransformSyncSystem::update(this);

	Renderer::update(*this, dt);

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
