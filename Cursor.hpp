//
// Created by Steve Wheeler on 18/02/2024.
//

#pragma once

#include "raylib.h"

#include "Camera.hpp"
#include "Collideable.hpp"
#include "CollisionSystem.hpp"
#include "RenderSystem.hpp"

#include <string>

namespace sage
{
    class Cursor
    {
        Ray ray {0};
        RayCollision collision {0};
        CollisionInfo boxHitInfo;
        std::string hitObjectName{};
        Color defaultColor = WHITE;
        Color hoverColor = LIME;
    public:
        void GetMouseRayCollision(Camera3D raylibCamera, const CollisionSystem& colSystem, const RenderSystem& renderSystem);
        void Draw(const CollisionSystem& colSystem);
        void DrawDebugText() const;
    
    };
}
