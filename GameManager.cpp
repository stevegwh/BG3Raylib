//
// Created by steve on 18/02/2024.
//

#include "GameManager.hpp"

#include "Registry.hpp"
#include "WorldObject.hpp"
#include "GameObjectFactory.hpp"
#include "Serializer.hpp"


namespace sage
{
GameManager::GameManager() :
sCamera(std::make_unique<sage::Camera>()),
userInput(std::make_unique<sage::UserInput>()),
renderSystem(std::make_unique<RenderSystem>()),
collisionSystem(std::make_unique<sage::CollisionSystem>()),
transformSystem(std::make_unique<sage::TransformSystem>()),
navigationGridSystem(std::make_unique<NavigationGridSystem>()),
sceneManager(std::make_unique<SceneManager>())
{
    EntityID rootNodeId = Registry::GetInstance().CreateEntity();
    auto rootNodeObject = std::make_unique<WorldObject>(rootNodeId);
    worldSystem = std::make_unique<sage::WorldSystem>(rootNodeId);
    worldSystem->AddComponent(std::move(rootNodeObject));
    actorMovementSystem = std::make_unique<sage::ActorMovementSystem>(userInput.get());
}

GameManager::~GameManager()
{
    cleanup();
}

void GameManager::init()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - mesh picking");
    
    // Should each state have its own set of systems?
    // Or, should the destructor just make sure to delete all its entities from the systems?
#ifdef EDITOR_MODE
    sceneManager->BufferSceneChange(1);
    sceneManager->TriggerSceneChange();
#else
    sceneManager->BufferSceneChange(0);
    sceneManager->TriggerSceneChange();
#endif

}

void GameManager::removeTower(EntityID entityId)
{
    Registry::GetInstance().DeleteEntity(entityId);
}

void GameManager::DeserializeMap()
{
    auto data = Serializer::DeserializeFile();
    if (data.has_value())
    {
        for (const auto& entityIdEntry : data.value())
        {
            //const std::string& entityId = entityIdEntry.first;
            auto id = Registry::GetInstance().CreateEntity();
            auto idString = std::to_string(id);
            const auto& componentMap = entityIdEntry.second;

            if (componentMap.find(transformSystem->getComponentName()) != componentMap.end())
            {
                const auto& transformComponent = componentMap.at(transformSystem->getComponentName());
                transformSystem->DeserializeComponents(idString, transformComponent);
            }

            if (componentMap.find(renderSystem->getComponentName()) != componentMap.end())
            {
                const auto& renderableComponent = componentMap.at(renderSystem->getComponentName());
                renderSystem->DeserializeComponents(idString, renderableComponent);
            }

            if (componentMap.find(collisionSystem->getComponentName()) != componentMap.end())
            {
                const auto& collideableComponent = componentMap.at(collisionSystem->getComponentName());
                collisionSystem->DeserializeComponents(idString, collideableComponent);
            }
        }
    }
}

void GameManager::SerializeMap() const
{
    SerializationData serializeData;
    transformSystem->SerializeComponents(serializeData);
    renderSystem->SerializeComponents(serializeData);
    collisionSystem->SerializeComponents(serializeData);

    Serializer::SerializeToFile(serializeData);
}

void GameManager::Update()
{
    init();
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        sCamera->HandleInput(); // TODO: Should merge this with userInput
        sCamera->Update();
        userInput->ListenForInput();

        sceneManager->head->Update();

        //----------------------------------------------------------------------------------
        draw();
        Registry::GetInstance().RunMaintainance();
        sceneManager->TriggerSceneChange();
    }
}

void GameManager::draw()
{
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(*sCamera->getCamera());

    // If we hit something, draw the cursor at the hit point
    userInput->Draw();

    sceneManager->head->Draw3D();

    EndMode3D();

    userInput->DrawDebugText();

    sceneManager->head->Draw2D();

    DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
    
};

void GameManager::cleanup()
{
    CloseWindow();
}
}


/*
// Check ray collision against model meshes
RayCollision meshHitInfo = { 0 };
for (int m = 0; m < tower->model.meshCount; m++)
{
    // NOTE: We consider the model.transform for the collision check but
    // it can be checked against any transform Matrix, used when checking against same
    // model drawn multiple times with multiple transforms
    meshHitInfo = GetRayCollisionMesh(ray, tower->model.meshes[m], tower->model.transform);
    if (meshHitInfo.hit)
    {
        // Save the closest hit mesh
        if ((!collision.hit) || (collision.distance > meshHitInfo.distance)) collision = meshHitInfo;

        break;  // Stop once one mesh collision is detected, the colliding mesh is m
    }
}

if (meshHitInfo.hit)
{
    collision = meshHitInfo;
    cursorColor = ORANGE;
    hitObjectName = "Renderable";
}
 */