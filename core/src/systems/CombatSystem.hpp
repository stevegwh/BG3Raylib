//
// Created by Steve Wheeler on 03/06/2024.
//

#pragma once

#include <entt/entt.hpp>

#include "Cursor.hpp"
#include "ControllableActorMovementSystem.hpp"
#include "components/Combatable.hpp"

namespace sage
{

class CombatSystem : public BaseSystem<Combatable>
{
    // Subscribe to collision with cursor on Enemy layer
    // React to click by updating HealthBar and animation
    Cursor* cursor;
    ControllableActorMovementSystem* actorMovementSystem;

    void startCombat(entt::entity entity);
    void onEnemyClick(entt::entity entity);
    void onEnemyDead(entt::entity entity);
    void destroyEnemy(entt::entity entity);
    void onAutoAttackEnd(entt::entity entity);
    void CheckInCombat(entt::entity entity);
public:
    CombatSystem(entt::registry* _registry, Cursor* _cursor, ControllableActorMovementSystem* _actorMovementSystem);
    void Update();
};

} // sage
