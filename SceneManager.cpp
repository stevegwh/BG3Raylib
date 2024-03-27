//
// Created by Steve Wheeler on 27/03/2024.
//

#include "SceneManager.hpp"
#include "Game.hpp"
#include "Editor.hpp"

namespace sage
{

SceneManager::SceneManager()
{
    // Make unordered_map's values lambdas that return unique_ptr's.
    auto gameScene = []() -> std::unique_ptr<Scene> { return std::make_unique<Game>(); };
    scenes.emplace(0, gameScene);

    auto editorScene = []() -> std::unique_ptr<Scene> { return std::make_unique<Editor>(); };
    scenes.emplace(1, editorScene);
}


void SceneManager::BufferSceneChange(SceneID id)
{
    buffer = id;
}

void SceneManager::TriggerSceneChange()
{
    if (buffer < 0) return;
    head = scenes.at(buffer)();
    buffer = -1;
}

} // sage