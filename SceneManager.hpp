//
// Created by Steve Wheeler on 27/03/2024.
//

#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include "Scene.hpp"

namespace sage
{

typedef int SceneID;
class SceneManager
{
    std::unordered_map<SceneID, std::function<std::unique_ptr<Scene>()>> scenes;
public:
    SceneID buffer = -1;
    std::unique_ptr<Scene> head;
    void BufferSceneChange(SceneID id);
    void TriggerSceneChange();
    SceneManager();
};

} // sage
