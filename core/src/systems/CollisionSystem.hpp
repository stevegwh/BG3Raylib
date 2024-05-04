//
// Created by Steve Wheeler on 18/02/2024.
//

#pragma once

#include "../components/Collideable.hpp"
#include "BaseSystem.hpp"

#include "raylib.h"
#include "entt/entt.hpp"

namespace sage
{
class CollisionSystem : public BaseSystem<Collideable>
{
    bool checkCollisionMatrix(const CollisionLayer& layer1, const CollisionLayer& layer2);
public:
    void OnTransformUpdate(entt::entity entity);
    explicit CollisionSystem(entt::registry* _registry);
    void UpdateWorldBoundingBox(entt::entity entityId, Matrix mat);
    [[nodiscard]] std::vector<CollisionInfo> GetCollisionsWithRay(const Ray& ray) const;
    void BoundingBoxDraw(entt::entity entityId, Color color = LIME) const;
    static bool CheckBoxCollision(const BoundingBox& col1, const BoundingBox& col2) ;
    [[nodiscard]] std::vector<CollisionInfo> GetCollisions(entt::entity entity);
    bool GetFirstCollision(entt::entity entity);
    
    const std::vector<std::vector<CollisionLayer>> collisionMatrix = 
        {
        {CollisionLayer::FLOOR, CollisionLayer::BUILDING}, // Default
        {CollisionLayer::FLOOR}, // Floor
        {CollisionLayer::BUILDING}, // Building
        {CollisionLayer::BUILDING}, // Navigation
        {CollisionLayer::FLOOR, CollisionLayer::BUILDING}, // Player
    };

};
}

