//
// Created by Steve on 05/06/24.
//

#pragma once

#include "ControllableActorMovementSystem.hpp"
#include "Cursor.hpp"

#include <entt/entt.hpp>

namespace sage
{

struct PlayerCombatLogicSubSystem
{
	entt::registry* registry;
    Cursor* cursor;
	ControllableActorMovementSystem* actorMovementSystem;
    
	void onEnemyClick(entt::entity entity);

	void Update(entt::entity entity) const;
	void StartCombat(entt::entity entity);
	void CheckInCombat(entt::entity entity) const;
	void OnDeath(entt::entity entity);
	void OnTargetInvalid(entt::entity entity);
    void AutoAttack(entt::entity entity) const;
    void OnHit(entt::entity entity, entt::entity attacker);

	PlayerCombatLogicSubSystem(entt::registry* _registry, ControllableActorMovementSystem* _actorMovementSystem,
                               Cursor* _cursor);
};

} // sage
