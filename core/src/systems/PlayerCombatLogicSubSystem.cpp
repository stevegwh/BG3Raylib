//
// Created by Steve on 05/06/24.
//

#include "PlayerCombatLogicSubSystem.hpp"
#include "../components/CombatableActor.hpp"
#include "../components/Animation.hpp"
#include "../components/Transform.hpp"
#include "../components/HealthBar.hpp"

#include "raylib.h"
#include "raymath.h"

namespace sage
{
void PlayerCombatLogicSubSystem::Update(entt::entity entity) const
{
	auto& c = registry->get<CombatableActor>(entity);
	if (c.inCombat) CheckInCombat(entity);
	if (c.target == entt::null || !c.inCombat) return;
	// Move to target
	// Progress tick
	// Turn to look at target
	// Autoattack

	// Player is out of combat if no enemy is targetting them?
	if (c.autoAttackTick >= c.autoAttackTickThreshold) // Maybe can count time since last autoattack to time out combat?
	{
		AutoAttack(entity);
	}
	else
	{
		c.autoAttackTick += GetFrameTime();
	}
}
void PlayerCombatLogicSubSystem::CheckInCombat(entt::entity entity) const
{
    // If the entity is not the target of any other combatable.
	// If no current target
	// Have a timer for aggro and if the player is not within that range for a certain amount of time they resume their regular task (tasks TBC)
	auto& combatable = registry->get<CombatableActor>(entity);
	if (combatable.target == entt::null)
	{
		combatable.inCombat = false;
		auto& animation = registry->get<Animation>(entity);
		animation.ChangeAnimationByEnum(AnimationEnum::IDLE);
	}
}

void PlayerCombatLogicSubSystem::OnDeath(entt::entity entity)
{

}

void PlayerCombatLogicSubSystem::OnTargetInvalid(entt::entity entity)
{
	auto& enemyCombatable = registry->get<CombatableActor>(entity);
	{
		entt::sink sink{ enemyCombatable.onDeath };
		sink.disconnect<&PlayerCombatLogicSubSystem::OnTargetInvalid>(this);
	}
	auto& playerCombatable = registry->get<CombatableActor>(actorMovementSystem->GetControlledActor());
	playerCombatable.target = entt::null;
}

void PlayerCombatLogicSubSystem::StartCombat(entt::entity entity)
{
    // TODO: What is "entity"?
    {
        auto& playerTrans = registry->get<Transform>(actorMovementSystem->GetControlledActor());
        entt::sink sink { playerTrans.onFinishMovement };
        sink.disconnect<&PlayerCombatLogicSubSystem::StartCombat>(this);
    }

    auto& playerCombatable = registry->get<CombatableActor>(actorMovementSystem->GetControlledActor());
    playerCombatable.inCombat = true;

	auto& enemyCombatable = registry->get<CombatableActor>(playerCombatable.target);
	{
		entt::sink sink{ enemyCombatable.onDeath };
		sink.connect<&PlayerCombatLogicSubSystem::OnTargetInvalid>(this);
	}
}

void PlayerCombatLogicSubSystem::onEnemyClick(entt::entity entity)
{
    auto& combatable = registry->get<CombatableActor>(actorMovementSystem->GetControlledActor());
    combatable.target = entity;
    auto& playerTrans = registry->get<Transform>(actorMovementSystem->GetControlledActor());
    const auto& enemyTrans = registry->get<Transform>(entity);
    actorMovementSystem->PathfindToLocation(actorMovementSystem->GetControlledActor(), enemyTrans.position); // TODO: cannot be enemy position
    {
        entt::sink sink { playerTrans.onFinishMovement };
        sink.connect<&PlayerCombatLogicSubSystem::StartCombat>(this);
    }
}

void PlayerCombatLogicSubSystem::AutoAttack(entt::entity entity) const
{
    auto& c = registry->get<CombatableActor>(entity);
	auto& t = registry->get<Transform>(entity);
	auto& enemyPos = registry->get<Transform>(c.target).position;
	Vector3 direction = Vector3Subtract(enemyPos, t.position);
	float angle = atan2f(direction.x, direction.z) * RAD2DEG;
	t.rotation.y = angle;
	c.autoAttackTick = 0;

	auto& animation = registry->get<Animation>(entity);
	animation.ChangeAnimationByEnum(AnimationEnum::AUTOATTACK);

    auto& enemyCombatable = registry->get<CombatableActor>(c.target);
    enemyCombatable.onHit.publish(c.target, actorMovementSystem->GetControlledActor(), 10); // TODO: tmp dmg
}

void PlayerCombatLogicSubSystem::OnHit(entt::entity entity, entt::entity attacker)
{
}

PlayerCombatLogicSubSystem::PlayerCombatLogicSubSystem(entt::registry *_registry, 
                                                       ControllableActorMovementSystem* _actorMovementSystem, 
                                                       Cursor* _cursor) :
   registry(_registry), actorMovementSystem(_actorMovementSystem), cursor(_cursor)
{
    {
        entt::sink sink{ cursor->onEnemyClick };
        sink.connect<&PlayerCombatLogicSubSystem::onEnemyClick>(this);
    }
}
} // sage