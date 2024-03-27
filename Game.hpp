//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once


#include "Scene.hpp"

namespace sage
{

class Game : public Scene
{
public:
    Game();
    ~Game() override;
    void Update() override;
    void Draw3D() override;
    void Draw2D() override;
};

} // sage
