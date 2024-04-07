//
// Created by Steve Wheeler on 21/03/2024.
//

#include "GameObjectFactory.hpp"

#include "raymath.h"

#include "GameManager.hpp"
#include "Registry.hpp"

#include "Transform.hpp"
#include "Renderable.hpp"
#include "Collideable.hpp"
#include "WorldObject.hpp"
#include "Animation.hpp"

namespace sage
{
BoundingBox createRectangularBoundingBox(float length, float height) 
{
    BoundingBox bb;
    // Calculate half dimensions
    float halfLength = length / 2.0f;
    float halfHeight = height / 2.0f;

    // Set minimum bounds
    bb.min.x = -halfLength;
    bb.min.y = 0.0f;
    bb.min.z = -halfLength;

    // Set maximum bounds
    bb.max.x = halfLength;
    bb.max.y = height;
    bb.max.z = halfLength;

    return bb;
}

EntityID GameObjectFactory::createPlayer(Vector3 position, const char* name) 
{
    EntityID id = Registry::GetInstance().CreateEntity(false);
    const char* modelPath = "resources/models/gltf/hero.glb";
    //sage::Material mat = { LoadTexture("resources/models/obj/cube_diffuse.png"), std::string("resources/models/obj/cube_diffuse.png") };

    auto transform = std::make_unique<Transform>(id);
    transform->position = position;
    transform->scale = 1.0f;
    transform->rotation = { 0, 0, 0 };
    Transform* const transform_ptr = transform.get();
    ECS->transformSystem->AddComponent(std::move(transform));
    auto model = LoadModel(modelPath);
    
    // Set animation hooks
    auto animation = std::make_unique<Animation>(id, modelPath, &model);
    animation->eventManager->Subscribe( [p = animation.get()] { p->ChangeAnimation(3); }, *transform_ptr->OnStartMovement);
    animation->eventManager->Subscribe( [p = animation.get()] { p->ChangeAnimation(0); }, *transform_ptr->OnFinishMovement);
    animation->eventManager->Subscribe( [p = animation.get()] {
        if (p->animIndex == 1)
        {
            p->ChangeAnimation(0);
        }
        else if (p->animIndex == 0)
        {
            p->ChangeAnimation(1);
        }
    }, *GM.userInput->OnCreateKeyPressedEvent);
    
    ECS->animationSystem->AddComponent(std::move(animation));
    
    Matrix modelTransform = MatrixMultiply(MatrixScale(0.035f, 0.035f, 0.035f) , MatrixRotateX(DEG2RAD*90));
    auto renderable = std::make_unique<Renderable>(id, 
                                                   model,
                                                   std::string(modelPath),
                                                   modelTransform,
                                                   transform_ptr);
    renderable->name = name;
    
    
    ECS->renderSystem->AddComponent(std::move(renderable));
    BoundingBox bb = createRectangularBoundingBox(3.0f, 7.0f); // Manually set bounding box dimensions
    auto collideable = std::make_unique<Collideable>(id, bb);
    collideable->collisionLayer = PLAYER;

    auto worldObject = std::make_unique<WorldObject>(id);

    ECS->collisionSystem->AddComponent(std::move(collideable));
    ECS->collisionSystem->UpdateWorldBoundingBox(id, ECS->transformSystem->GetMatrix(id));
    ECS->worldSystem->AddComponent(std::move(worldObject));
    return id;
}

void GameObjectFactory::createTower(Vector3 position, const char* name) 
{
    EntityID id = Registry::GetInstance().CreateEntity();
    
    auto transform = std::make_unique<Transform>(id);
    transform->position = position;
    transform->scale = 1.0f;
    Transform* const transform_ptr = transform.get();
    ECS->transformSystem->AddComponent(std::move(transform));
    
    sage::Material mat = { LoadTexture("resources/models/obj/turret_diffuse.png"), "resources/models/obj/turret_diffuse.png" };
    Model model = LoadModel("resources/models/obj/turret.obj");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mat.diffuse;
    auto renderable = std::make_unique<Renderable>(id, model, mat, "resources/models/obj/turret.obj", MatrixIdentity(), transform_ptr);
    renderable->name = name;
    ECS->renderSystem->AddComponent(std::move(renderable));
    auto collideable = std::make_unique<Collideable>(id, ECS->renderSystem->GetComponent(id)->CalculateModelBoundingBox());
    collideable->collisionLayer = BUILDING;
    ECS->collisionSystem->AddComponent(std::move(collideable));
    ECS->collisionSystem->UpdateWorldBoundingBox(id, ECS->transformSystem->GetMatrix(id));

    auto worldObject = std::make_unique<WorldObject>(id);
    ECS->worldSystem->AddComponent(std::move(worldObject));
}
void GameObjectFactory::loadBlenderLevel()
{
    EntityID id = Registry::GetInstance().CreateEntity();
    
    const char* modelPath = "resources/models/m3d/seagull.m3d";
    Model model = LoadModel(modelPath);
    
    auto transform = std::make_unique<Transform>(id);
    transform->position = {0, 0, 0};
    transform->scale = 1.0f;
    Transform* const transform_ptr = transform.get();
    ECS->transformSystem->AddComponent(std::move(transform));

    Matrix modelTransform = MatrixIdentity();
    auto renderable = std::make_unique<Renderable>(id,
                                                   model,
                                                   std::string(modelPath),
                                                   modelTransform,
                                                   transform_ptr);
    renderable->name = "Level";


    ECS->renderSystem->AddComponent(std::move(renderable));

}
} // sage