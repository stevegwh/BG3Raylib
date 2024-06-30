//
// Created by Steve Wheeler on 27/03/2024.
//

#include "ExampleScene.hpp"
#include "../GameObjectFactory.hpp"

#include "raylib.h"

namespace sage
{
void ExampleScene::Update()
{
    Scene::Update();
    data->actorMovementSystem->Update();
    data->controllableActorSystem->Update();
    data->animationSystem->Update();
    data->dialogueSystem->Update();
    data->healthBarSystem->Update();
    // State systems
    data->defaultStateSystem->Update();
    data->combatStateSystem->Update();
}

void ExampleScene::Draw2D()
{
    data->dialogueSystem->Draw2D();
    Scene::Draw2D();
}

void ExampleScene::Draw3D()
{
    data->healthBarSystem->Draw3D();
    data->combatStateSystem->Draw3D();
    Scene::Draw3D();
}

void ExampleScene::DrawDebug()
{
    data->collisionSystem->DrawDebug();
    data->navigationGridSystem->DrawDebug();
    data->actorMovementSystem->DrawDebug();
}

ExampleScene::~ExampleScene()
{
    
}

ExampleScene::ExampleScene(entt::registry* _registry, std::unique_ptr<GameData> _data) :
Scene(_registry, std::move(_data))
{
	data->navigationGridSystem->Init(1000, 1.0f);
    lightSubSystem->lights[0] = CreateLight(LIGHT_POINT, { 0, 25, 0 }, Vector3Zero(), WHITE, lightSubSystem->shader);
    auto playerId = GameObjectFactory::createPlayer(registry, data.get(), {20.0f, 0, 20.0f}, "Player");
    
    auto knight = GameObjectFactory::createKnight(registry, data.get(), {0.0f, 0, 20.0f}, "Knight");
    auto enemy2 = GameObjectFactory::createEnemy(registry, data.get(), {10.0f, 0, 0.0f}, "Enemy");
    auto enemy3 = GameObjectFactory::createEnemy(registry, data.get(), {20.0f, 0, 0.0f}, "Enemy");
    auto enemy4 = GameObjectFactory::createEnemy(registry, data.get(), {30.0f, 0, 0.0f}, "Enemy");
    auto enemy5 = GameObjectFactory::createEnemy(registry, data.get(), {40.0f, 0, 0.0f}, "Enemy");

    
    data->Load();
    
    BoundingBox bb = {
        .min = { -1000.0f, 0.1f, -1000.0f },
        .max = {  1000.0f, 0.1f,  1000.0f }
    };
    GameObjectFactory::createFloor(registry, this, bb);
    //GameObjectFactory::loadBlenderLevel(registry, this);


    data->navigationGridSystem->PopulateGrid();

    // TODO: tmp
	const auto& col = registry->get<Collideable>(knight);
    data->navigationGridSystem->MarkSquareOccupied(col.worldBoundingBox, true, knight);

    // TODO: Not sure if I like this
    data->combatStateSystem->playerCombatLogicSubSystem->Enable();
}

} // sage