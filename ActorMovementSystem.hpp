//
// Created by Steve Wheeler on 29/02/2024.
//

#pragma once

#include "Actor.hpp"

#include "raylib.h"
#include "raymath.h"
#include "BaseSystem.hpp"
#include "UserInput.hpp"

namespace sage
{

class ActorMovementSystem : public BaseSystem<Actor>
{
    UserInput* cursor;
    EntityID actorId; // Temporary for now.
    
    void onCursorClick();
public:
    
    void SetControlledActor(EntityID id)
    {
        actorId = id;
        // Unsubscribe needed?
        const std::function<void()> f1 = [p = this] { p->onCursorClick(); };
        cursor->OnClickEvent->Subscribe(std::make_shared<EventCallback>(f1));
    }

    explicit ActorMovementSystem(UserInput* _cursor) : cursor(_cursor) {}
    
};

} // sage
