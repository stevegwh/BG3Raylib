//
// Created by Steve Wheeler on 21/02/2024.
//

#include "RenderSystem.hpp"
#include "raylib.h"
#include "GameManager.hpp"

namespace sage
{
    RenderSystem::~RenderSystem()
    {
        
    
    }
    
    void RenderSystem::Draw() const
    {
        for (const auto& renderable : components)
        {
            DrawModel(renderable.second->model, renderable.second->position, renderable.second->scale, WHITE);
        }
    }

    void RenderSystem::DeserializeComponents(const std::string& entityId, const std::unordered_map<std::string, std::string>& data)
    {
        int id = std::stoi(entityId);
        
        sage::Material mat = { .diffuse = LoadTexture(data.at("Material").c_str()), .path = data.at("Material") };
        
        std::string modelPath = data.at("Model");
        Model model = LoadModel(modelPath.c_str());
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mat.diffuse;
        
        auto renderable = std::make_unique<Renderable>(id, model, mat, modelPath);
        auto t = GM.transformSystem->GetComponent(id);
        AddComponent(std::move(renderable), t);
    }
    
    void RenderSystem::onTransformUpdate(EntityID id)
    {
        auto t = GM.transformSystem->GetComponent(id);
        auto r = components.at(id).get();
        r->position = t->position;
        r->scale = t->scale;
    }

    void RenderSystem::AddComponent(std::unique_ptr<Renderable> component, const sage::Transform* const transform)
    {
    
        const std::function<void()> f1 = [p = this, id = component->entityId] { p->onTransformUpdate(id); };
        transform->OnPositionUpdate->Subscribe(std::make_shared<EventCallback>(f1));
        component->position = transform->position;
        component->scale = transform->scale;
    
        m_addComponent(std::move(component));
    }

}

