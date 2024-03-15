//
// Created by Steve Wheeler on 29/02/2024.
//

#include "ActorMovementSystem.hpp"
#include "Game.hpp"

namespace sage
{

    void ActorMovementSystem::onCursorClick()
    {
        if (cursor->collision.hit)
        {
            switch (Game::GetInstance().collisionSystem->GetComponent(cursor->rayCollisionResultInfo.collidedEntityId)->collisionLayer)
            {
            case FLOOR:
                //Game::GetInstance().transformSystem->MoveToLocation(playerId, cursor->collision.point);
                Vector2 idx = Game::GetInstance().navigationGridSystem->WorldToGridSpace(cursor->collision.point);
                std::cout << "x: " << idx.x << " y: " << idx.y << std::endl;
                
            }
        }
    }
} // sage