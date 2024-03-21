//
// Created by Steve Wheeler on 18/02/2024.
//

#pragma once
#include "raylib.h"
#include <string>
#include <iostream>

#include "Material.hpp"
#include "Component.hpp"


namespace sage
{
    struct Renderable : public Component<Renderable>
    {
        sage::Material material;
        const Model model;
        const BoundingBox meshBoundingBox;
        std::string name = "Default";
        
        Renderable(EntityID entityId, Model _model, sage::Material _material)
        : Component(entityId), model(_model), material(_material), meshBoundingBox(GetMeshBoundingBox(model.meshes[0]))
        {
            model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = material.diffuse;
        }
        
        ~Renderable()
        {
            UnloadModel(model);
            UnloadTexture(material.diffuse);
        }

    };
}

