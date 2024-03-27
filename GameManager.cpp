﻿//
// Created by steve on 18/02/2024.
//

#include "GameManager.hpp"

#include "Serializer.hpp"

#include <utility>
#include <unordered_map>


namespace sage
{
    
    void GameManager::init()
    {
        // Initialization
        //--------------------------------------------------------------------------------------
        const int screenWidth = SCREEN_WIDTH;
        const int screenHeight = SCREEN_HEIGHT;

        InitWindow(screenWidth, screenHeight, "raylib [models] example - mesh picking");

        auto playerId = GameObjectFactory::createPlayer({20.0f, 0, 20.0f}, "Player");
        actorMovementSystem->SetControlledActor(playerId);

//        GameObjectFactory::createTower({0.0f, 0.0f, 0.0f}, "Tower");
//        GameObjectFactory::createTower({10.0f, 0.0f, 20.0f}, "Tower 2");
//
        // Ground quad
        EntityID floor = Registry::GetInstance().CreateEntity();
        Vector3 g0 = (Vector3){ -50.0f, 0.1f, -50.0f };
        Vector3 g2 = (Vector3){  50.0f, 0.1f,  50.0f };
        BoundingBox bb = {
            .min = g0,
            .max = g2
        };
        auto floorCollidable = std::make_unique<Collideable>(floor, bb);
        floorCollidable->collisionLayer = FLOOR;
        collisionSystem->AddComponent(std::move(floorCollidable));

//        auto floorWorldObject = std::make_unique<WorldObject>(floor);
//        worldSystem->AddComponent(std::move(floorWorldObject));
        
        DeserializeMap();
        
        navigationGridSystem->Init(100, 1.0f);
        navigationGridSystem->PopulateGrid();
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
                auto newId = std::to_string(Registry::GetInstance().CreateEntity());
                const auto& componentMap = entityIdEntry.second;
    
                if (componentMap.find(transformSystem->getComponentName()) != componentMap.end())
                {
                    const auto& transformComponent = componentMap.at(transformSystem->getComponentName());
                    transformSystem->DeserializeComponents(newId, transformComponent);
                }
    
                if (componentMap.find(renderSystem->getComponentName()) != componentMap.end())
                {
                    const auto& renderableComponent = componentMap.at(renderSystem->getComponentName());
                    renderSystem->DeserializeComponents(newId, renderableComponent);
                }
    
                if (componentMap.find(collisionSystem->getComponentName()) != componentMap.end())
                {
                    const auto& collideableComponent = componentMap.at(collisionSystem->getComponentName());
                    collisionSystem->DeserializeComponents(newId, collideableComponent);
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
            transformSystem->Update();

            //----------------------------------------------------------------------------------
            draw();
            Registry::GetInstance().RunMaintainance();
            
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

        renderSystem->Draw();

#ifdef EDITOR_MODE
        gameEditor->Draw();
#endif

        DrawGrid(100, 1.0f);
        
//        for (const auto& gridSquare : navigationGridSystem->GetGridSquares())
//        {
//            auto bb = collisionSystem->GetComponent(gridSquare->entityId)->worldBoundingBox;
//            bb.max.y = 0.1f;
//            Color color = gridSquare->occupied ? RED : GREEN;
//            DrawBoundingBox(bb, color);
//        }
        for (const auto& gridSquareRow : navigationGridSystem->GetGridSquares())
        {
            for (const auto& gridSquare : gridSquareRow) 
            {
                BoundingBox bb;
                bb.min = gridSquare->worldPosMin;
                bb.max = gridSquare->worldPosMax;
                bb.max.y = 0.1f;
                Color color = gridSquare->occupied ? RED : GREEN;
                DrawBoundingBox(bb, color);
            }
        }

        EndMode3D();

        userInput->DrawDebugText();
        
#ifdef EDITOR_MODE
        gameEditor->DrawDebugText();
#endif

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