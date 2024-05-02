//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once


#include "Scene.hpp"
#include "UserInput.hpp"

#include <entt/entt.hpp>

#include <vector>
#include <memory>

namespace sage
{

class Game : public Scene
{
    entt::registry* registry;
    UserInput* cursor;
    void onEditorModePressed();
public:
    
    explicit Game(entt::registry* _registry, sage::UserInput* _cursor);
    ~Game() override;
    void Update() override;
    void Draw3D() override;
    void Draw2D() override;
};

} // sage
