//
// Created by Steve on 05/06/24.
//

#pragma once

#include "systems/StateMachineSystem.hpp"
#include "systems/ControllableActorSystem.hpp"
#include "Cursor.hpp"

#include "entt/entt.hpp"

namespace sage
{

struct PlayerCombatLogicSubSystem
{
	entt::registry* registry;
    Cursor* cursor;
    StateMachineSystem* stateMachineSystem;
	ControllableActorSystem* actorMovementSystem;
    
	void onEnemyClick(entt::entity entity);

	void Update() const;
	void StartCombat(entt::entity entity);
	[[nodiscard]] bool CheckInCombat(entt::entity entity) const;
	void OnDeath(entt::entity entity);
	void OnTargetDeath(entt::entity entity);
    void OnAttackCancel(entt::entity entity);
    void AutoAttack(entt::entity entity) const;
    void OnHit(entt::entity entity, entt::entity attacker);
    void Enable();
    void Disable();

	PlayerCombatLogicSubSystem(entt::registry* _registry,
                               StateMachineSystem* _stateMachineSystem,
                               ControllableActorSystem* _actorMovementSystem,
                               Cursor* _cursor);
};

} // sage
