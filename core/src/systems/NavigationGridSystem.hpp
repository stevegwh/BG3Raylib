//
// Created by Steve Wheeler on 25/02/2024.
//

#pragma once

#include "BaseSystem.hpp"
#include "../components/NavigationGridSquare.hpp"
#include "CollisionSystem.hpp"

#include "entt/entt.hpp"

namespace sage
{

class NavigationGridSystem : public BaseSystem<NavigationGridSquare>
{


public:
    float spacing{};
    int slices{};
    std::vector<std::vector<NavigationGridSquare*>> gridSquares; // TODO: make this private again
    NavigationGridSystem(entt::registry* _registry);
    void Init(int _slices, float _spacing);
    void PopulateGrid();
    bool GetPathfindRange(const entt::entity& actorId, int bounds, Vector2& minRange, Vector2& maxRange);
    bool WorldToGridSpace(Vector3 worldPos, Vector2& out);
    bool WorldToGridSpace(Vector3 worldPos, Vector2& out, const Vector2& _minRange, const Vector2& _maxRange);
    std::vector<Vector3> Pathfind(const Vector3& startPos, const Vector3& finishPos);
    std::vector<Vector3> Pathfind(const Vector3& startPos, const Vector3& finishPos, const Vector2& minRange, const Vector2& maxRange);
    const std::vector<std::vector<NavigationGridSquare*>>& GetGridSquares();
};

} // sage
