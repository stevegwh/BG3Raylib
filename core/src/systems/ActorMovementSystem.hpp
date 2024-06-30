//
// Created by Steve Wheeler on 21/02/2024.
//

#pragma once

#include "../components/Transform.hpp"
#include "../components/MovableActor.hpp"
#include "BaseSystem.hpp"
#include "CollisionSystem.hpp"
#include "NavigationGridSystem.hpp"

#include <vector>

namespace sage
{
	class ActorMovementSystem : public BaseSystem<MoveableActor>
	{
		std::vector<Ray> debugRays;
		CollisionSystem* collisionSystem;
		NavigationGridSystem* navigationGridSystem;

	public:
		ActorMovementSystem(entt::registry* _registry, CollisionSystem* _collisionSystem,
		                    NavigationGridSystem* _navigationGridSystem);
		void PruneMoveCommands(const entt::entity& entity) const;
		// TODO: Overload this so you can just update one field at a time if needed
		void PathfindToLocation(const entt::entity& entity, const Vector3& destination, bool initialMove = true);
		void MoveToLocation(const entt::entity& entity, Vector3 location);
		void CancelMovement(const entt::entity& entity) const;
		void Update();
		void DrawDebug() const;
	};
}
