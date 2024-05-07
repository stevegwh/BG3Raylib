//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once


#include "Scene.hpp"
#include "../UserInput.hpp"

#include "entt/entt.hpp"
#include "Game.hpp"

#include <vector>
#include <memory>

namespace sage
{

class GameScene : public Scene
{
public:
    
    GameScene(entt::registry* _registry, Game* _game, Settings _settings);
    ~GameScene() override;
    void Update() override;
    void Draw3D() override;
    void Draw2D() override;
};

} // sage
