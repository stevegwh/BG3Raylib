﻿//
// Created by steve on 18/02/2024.
//

#pragma once
#include "raylib.h"
#include "raymath.h"

#include <memory>
#include <string>

#include "Registry.hpp"
#include "Renderable.hpp"
#include "CollisionSystem.hpp"
#include "UserInput.hpp"
#include "Camera.hpp"
#include "RenderSystem.hpp"
#include "Entity.hpp"
#include "TransformSystem.hpp"
#include "WorldObject.hpp"
#include "WorldSystem.hpp"

#include "Editor.hpp"

namespace sage
{
    static constexpr int SCREEN_WIDTH = 1280;
    static constexpr int SCREEN_HEIGHT = 720;
    class Game
    {

        std::unique_ptr<sage::UserInput> userInput;
        std::unique_ptr<sage::Editor> gameEditor;

        static void init();
        static void cleanup();
        void draw();
        void createTower(Vector3 position, const char* name) const;
        void removeTower(EntityID entityId);

        Game() :
        sCamera(std::make_unique<sage::Camera>()),
        renderSystem(std::make_unique<RenderSystem>()),
        collisionSystem(std::make_unique<sage::CollisionSystem>()),
        transformSystem(std::make_unique<sage::TransformSystem>())
        {
            userInput = std::make_unique<sage::UserInput>();
            gameEditor = std::make_unique<sage::Editor>(userInput.get());

            init();
            
            EntityID rootNodeId = Registry::GetInstance().CreateEntity();
            auto rootNodeObject = std::make_unique<WorldObject>(rootNodeId);
            worldSystem = std::make_unique<sage::WorldSystem>(rootNodeId);
            worldSystem->AddComponent(std::move(rootNodeObject));

            createTower({0.0f, 0.0f, 0.0f}, "Tower");
            createTower({10.0f, 0.0f, 20.0f}, "Tower 2");

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
            
            auto floorWorldObject = std::make_unique<WorldObject>(floor);
            worldSystem->AddComponent(std::move(floorWorldObject));
            
        }

        ~Game()
        {
            cleanup();
        }
        
    public:

        std::unique_ptr<sage::CollisionSystem> collisionSystem;
        std::unique_ptr<sage::RenderSystem> renderSystem;
        std::unique_ptr<sage::TransformSystem> transformSystem;
        std::unique_ptr<sage::WorldSystem> worldSystem;
        std::unique_ptr<sage::Camera> sCamera;

        static Game& GetInstance()
        {
            static Game instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }
        Game(Game const&) = delete;
        void operator=(Game const&)  = delete;
        
        void Update();

        friend class Editor;
    };
}

