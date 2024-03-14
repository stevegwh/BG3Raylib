//
// Created by Steve Wheeler on 25/02/2024.
//

#include "NavigationGridSystem.hpp"
#include "Collideable.hpp"
#include "CollisionSystem.hpp"
#include "Game.hpp"

namespace sage
{

    void NavigationGridSystem::init(int slices, float spacing, CollisionSystem& collisionSystem)
    {
        int halfSlices = slices / 2;

        for (int i = -halfSlices; i < halfSlices; i++) 
        {
            for (int j = -halfSlices; j < halfSlices; j++) 
            {
                Vector3 v1 = {(float)i * spacing, 0, (float)j * spacing};
                Vector3 v3 = {(float)(i + 1) * spacing, 1.0f, (float)(j + 1) * spacing};

                BoundingBox boundingBox;
                boundingBox.min = v1;
                boundingBox.max = v3; // Diagonal opposite

                // Padding
                float padding = 0.01f;
                boundingBox.min.x += padding;
                boundingBox.min.z += padding;
                boundingBox.max.x -= padding;
                boundingBox.max.z -= padding;
                
                EntityID id = Registry::GetInstance().CreateEntity();
                
                // Add clickable grid to collision system
                auto collideable = std::make_unique<Collideable>(Collideable(id, boundingBox));
                collideable->collisionLayer = NAVIGATION;
                collisionSystem.AddComponent(std::move(collideable));
                
                // TODO: this needs to be stored in a systematic way for pathfinding to work
                auto gridSquare = std::make_unique<NavigationGridSquare>(id);
                AddComponent(std::move(gridSquare));

            }
        }
        const auto& test = components;

    }

    // It would make a lot more sense just to iterate over all buildings etc.
    void NavigationGridSystem::FindOccupiedGridSquares()
    {
        CollisionSystem& collisionSystem = *Game::GetInstance().collisionSystem;
        unoccupiedSquares.clear();
        for (const auto& gridSquare : components)
        {
            
//            gridSquare.second->collisionsWithSquare = collisionSystem.GetCollisions(gridSquare.second->entityId);
//            if (gridSquare.second->collisionsWithSquare.empty())
//            {
//                unoccupiedSquares.push_back(*gridSquare.second);
//            }

            gridSquare.second->occupied = collisionSystem.GetFirstCollision(gridSquare.second->entityId);
            unoccupiedSquares.push_back(*gridSquare.second);
        }
    }
    
} // sage