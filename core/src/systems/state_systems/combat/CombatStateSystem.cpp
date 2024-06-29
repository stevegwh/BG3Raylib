//
// Created by Steve Wheeler on 03/06/2024.
//

#include "CombatStateSystem.hpp"
#include "components/Animation.hpp"
#include "components/HealthBar.hpp"

#include <iostream>

#include "raymath.h"

namespace sage
{

void CombatStateSystem::Draw3D()
{
    auto view = registry->view<CombatableActor>();
    for (auto& entity : view)
    {
        auto& c = registry->get<CombatableActor>(entity);
        if (c.actorType == CombatableActorType::WAVEMOB)
        {
            waveMobCombatLogicSubSystem->Draw3D(entity);
        }
    }
}


void CombatStateSystem::Update()
{
    playerCombatLogicSubSystem->Update();
    waveMobCombatLogicSubSystem->Update();
}

CombatStateSystem::CombatStateSystem(entt::registry *_registry,
	Cursor *_cursor,
	StateMachineSystem* _stateMachineSystem,
	ControllableActorSystem* _actorMovementSystem,
	ActorMovementSystem* _transformSystem,
	CollisionSystem* _collisionSystem,
    NavigationGridSystem* _navigationGridSystem) :
	BaseSystem<CombatableActor>(_registry),
	stateMachineSystem(_stateMachineSystem),
	cursor(_cursor),
	actorMovementSystem(_actorMovementSystem),
	playerCombatLogicSubSystem(std::make_unique<PlayerCombatLogicSubSystem>(_registry, _stateMachineSystem, _actorMovementSystem, _cursor)),
	waveMobCombatLogicSubSystem(std::make_unique<WaveMobCombatLogicSubSystem>(_registry, _stateMachineSystem, _transformSystem, _collisionSystem, _navigationGridSystem))
{
}
} // sage