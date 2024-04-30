//
// Created by Steve Wheeler on 27/03/2024.
//

#include "Game.hpp"

#include "raylib.h"

#include <memory>

// Components
#include "Collideable.hpp"

// Misc
#include "GameManager.hpp"
#include "GameObjectFactory.hpp"

namespace sage
{
Game::Game(UserInput* _cursor) : cursor(_cursor), eventManager(std::make_unique<EventManager>())
{
    lightSubSystem->lights[0] = CreateLight(LIGHT_POINT, (Vector3){ 0, 25, 0 }, Vector3Zero(), WHITE, lightSubSystem->shader);
    auto playerId = GameObjectFactory::createPlayer({20.0f, 0, 20.0f}, "Player");
    ECS->actorMovementSystem->SetControlledActor(playerId);
    
    //ECS->DeserializeMap(); // TODO: Should specify path to saved map of scene
    GameObjectFactory::loadBlenderLevel(this);
    
    // This should also be based on scene parameters
    ECS->navigationGridSystem->Init(100, 1.0f);
    ECS->navigationGridSystem->PopulateGrid();
    
    eventManager->Subscribe( [p = this] { p->onEditorModePressed(); }, *cursor->OnRunModePressedEvent);
}

Game::~Game()
{
}

void Game::Update()
{
    ECS->transformSystem->Update();
    ECS->animationSystem->Update();
    ECS->renderSystem->Update();
}

void Game::Draw3D()
{
    ECS->renderSystem->Draw();

    //DrawGrid(100, 1.0f);

//    for (const auto& gridSquareRow : ECS->navigationGridSystem->GetGridSquares())
//    {
//        for (const auto& gridSquare : gridSquareRow)
//        {
//            BoundingBox bb;
//            bb.min = gridSquare->worldPosMin;
//            bb.max = gridSquare->worldPosMax;
//            bb.max.y = 0.1f;
//            Color color = gridSquare->occupied ? RED : GREEN;
//            DrawBoundingBox(bb, color);
//        }
//    }
}

void Game::Draw2D()
{
    
}

void Game::onEditorModePressed()
{
    GM.SetStateEditor();
}

} // sage