//
// Created by Steve Wheeler on 21/02/2024.
//

#pragma once

#include <map>
#include <memory>

#include "Transform.hpp"
#include "BaseSystem.hpp"

#include "raymath.h"

namespace sage
{
class TransformSystem : public BaseSystem<Transform>
{
public:
    // TODO: Overload this so you can just update one field at a time if needed
    void SetComponent(EntityID entityId, Transform newTransform);
};
}
