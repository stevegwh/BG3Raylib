//
// Created by Steve on 05/06/24.
//

#pragma once

#include "systems/StateMachineSystem.hpp"
#include "systems/ControllableActorMovementSystem.hpp"
#include "Cursor.hpp"

#include "entt/entt.hpp"

namespace sage
{

struct PlayerCombatLogicSubSystem
{
	entt::registry* registry;
    Cursor* cursor;
    StateMachineSystem* stateMachineSystem;
	ControllableActorMovementSystem* actorMovementSystem;
    
	void onEnemyClick(entt::entity entity);

	void Update() const;
	void StartCombat(entt::entity entity);
	bool CheckInCombat(entt::entity entity) const;
	void OnDeath(entt::entity entity);
	void OnTargetDeath(entt::entity entity);
    void OnAttackCancel(entt::entity entity);
    void AutoAttack(entt::entity entity) const;
    void OnHit(entt::entity entity, entt::entity attacker);

	PlayerCombatLogicSubSystem(entt::registry* _registry,
                               StateMachineSystem* _stateMachineSystem,
                               ControllableActorMovementSystem* _actorMovementSystem,
                               Cursor* _cursor);
};

} // sage
