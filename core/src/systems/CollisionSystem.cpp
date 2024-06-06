//
// Created by Steve Wheeler on 18/02/2024.
//

#include <algorithm>

#include "CollisionSystem.hpp"

#include "../../utils/Serializer.hpp"
#include "../Application.hpp"

bool compareRayCollisionDistances(const sage::CollisionInfo& a, const sage::CollisionInfo& b)
{
    return a.rlCollision.distance < b.rlCollision.distance;
}


namespace sage
{

std::vector<CollisionInfo> CollisionSystem::GetCollisionsWithRay(const Ray& ray) const
{
    // TODO: You can define component groups with entt. Do this for collisions based on their layers.
    std::vector<CollisionInfo> collisions;
    
    auto view = registry->view<Collideable>();

    view.each([&collisions, ray](auto entity, const auto& c)
    {
        if (c.collisionLayer != CollisionLayer::NAVIGATION) // TODO: Need to define a collision matrix
        {
            auto col = GetRayCollisionBox(ray, c.worldBoundingBox);
            if (col.hit)
            {
                CollisionInfo info = {
                    .collidedEntityId = entity,
                    .collidedBB = c.worldBoundingBox,
                    .rlCollision = col,
                    .collisionLayer = c.collisionLayer
                };
                collisions.push_back(info);
            }
        }
    });

    std::sort(collisions.begin(), collisions.end(), compareRayCollisionDistances);

    return collisions;
}

/**
 * Responsible for updating the collideable when its corresponding transform changes
 * @param entity 
 **/
void CollisionSystem::OnTransformUpdate(entt::entity entity)
{
    auto& trans = registry->get<Transform>(entity);
    auto& col = registry->get<Collideable>(entity);
    Matrix mat = trans.GetMatrixNoRot();
    auto bb = col.localBoundingBox;
    bb.min = Vector3Transform(bb.min, mat);
    bb.max = Vector3Transform(bb.max, mat);
    col.worldBoundingBox = bb;
}

void CollisionSystem::BoundingBoxDraw(entt::entity entityId, Color color) const
{
    DrawBoundingBox(registry->get<Collideable>(entityId).worldBoundingBox, color);
}

/**
 * Calculates worldBoundingBox by multiplying localBoundingBox with the passed transform matrix
 * @param entityId The id of the entity
 * @param mat The transform matrix for the local bounding box
 */
void CollisionSystem::UpdateWorldBoundingBox(entt::entity entityId, Matrix mat) // TODO: I don't like the name
{
    registry->patch<Collideable>(entityId, [mat](auto& col) {
        auto bb = col.localBoundingBox;
        bb.min = Vector3Transform(bb.min, mat);
        bb.max = Vector3Transform(bb.max, mat);
        col.worldBoundingBox = bb;
    });
}

bool CollisionSystem::CheckBoxCollision(const BoundingBox& col1, const BoundingBox& col2) 
{
    return CheckCollisionBoxes(col1, col2);
}

bool CollisionSystem::GetFirstCollision(entt::entity entity) // TODO: Terrible name. Should be "CheckAnyBoxCollision"
{
    const auto& targetCol = registry->get<Collideable>(entity);

    auto view = registry->view<Collideable>();

    for (const auto& ent: view)
    {
        const auto& c = view.get<Collideable>(ent);
        if (c.collisionLayer != CollisionLayer::BUILDING) continue; // TODO: Wanted to query a collision matrix but is far too slow
        bool colHit = CheckBoxCollision(targetCol.worldBoundingBox, c.worldBoundingBox);
        if (colHit) return true;
    }
    return false;
}

CollisionSystem::CollisionSystem(entt::registry *_registry) :
    BaseSystem<Collideable>(_registry)
{
}

}