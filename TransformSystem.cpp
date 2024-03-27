//
// Created by Steve Wheeler on 21/02/2024.
//

#include "TransformSystem.hpp"

#include "GameManager.hpp"
#include "Serializer.hpp"

namespace sage
{
    void TransformSystem::SetComponent(EntityID entityId, const Transform& newTransform)
    {
        components.at(entityId)->position = newTransform.position;
    }
    
    void TransformSystem::PathfindToLocation(EntityID entityId, const std::vector<Vector3>& path)
    {
        
        // Prune existing move commands
        // TODO: improve
        for (auto it = moveTowardsTransforms.begin(); it != moveTowardsTransforms.end();)
        {
            if ((*it)->entityId == entityId)
            {
                moveTowardsTransforms.erase(it);
                continue;
            }
            ++it;
        }
        auto transform = components.at(entityId).get();
        
        // Clear queue of previous commands
        std::queue<Vector3> empty;
        std::swap(transform->targets, empty);

        for (auto n : path) transform->targets.emplace(n);
        transform->direction = Vector3Normalize(Vector3Subtract(transform->targets.front(), transform->position));
        moveTowardsTransforms.push_back(transform);
    }

    void TransformSystem::DeserializeComponents(const std::string& entityId, const std::unordered_map<std::string, std::string>& data)
    {
        int id = std::stoi(entityId);
        auto transform = std::make_unique<Transform>(id);
        auto position = data.at("Position");
        transform->position = Serializer::ConvertStringToVector3(position);
        AddComponent(std::move(transform));
    }
    
    void TransformSystem::Update()
    {     
        
        for (auto it = moveTowardsTransforms.begin(); it != moveTowardsTransforms.end();) 
        {
            
            const auto& transform = *it;

            if (Vector3Distance(transform->targets.front(), transform->position) < 0.5f)
            {
                transform->targets.pop();
                if (transform->targets.empty())
                {
                    it = moveTowardsTransforms.erase(it);
                    continue;
                }
                transform->direction = Vector3Normalize(Vector3Subtract(transform->targets.front(), transform->position));
            }

            transform->position.x = transform->position.x + transform->direction.x * 0.5f;
            //transform->position.x = dy * 0.5f;
            transform->position.z = transform->position.z + transform->direction.z * 0.5f;
            transform->OnPositionUpdate->InvokeAllCallbacks();
            ++it;
        }

    }
    
    
}
