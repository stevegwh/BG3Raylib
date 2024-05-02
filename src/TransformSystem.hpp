//
// Created by Steve Wheeler on 21/02/2024.
//

#pragma once

#include "Transform.hpp"
#include "BaseSystem.hpp"

#include "raymath.h"
#include <entt/entt.hpp>

#include <map>
#include <memory>
#include <utility>
#include <queue>
#include <vector>

namespace sage
{
struct Transform; // Forward declaration
class TransformSystem : public BaseSystem<Transform>
{
    friend class Transform;
    std::vector<std::pair<entt::entity, Transform*>> moveTowardsTransforms;
public:
    TransformSystem(entt::registry* _registry);
    // TODO: Overload this so you can just update one field at a time if needed
    void PathfindToLocation(const entt::entity& entityId, const std::vector<Vector3>& path);
    void MoveToLocation(const entt::entity& entityId, Vector3 location);
    void DeserializeComponents(const std::string& entityId, const std::unordered_map<std::string, std::string>& data);
    void Update();
    Matrix GetMatrix(const entt::entity& id);
    Matrix GetMatrixNoRot(const entt::entity& id);
};
}
