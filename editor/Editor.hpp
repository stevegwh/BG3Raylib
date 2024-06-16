//
// Created by Steve Wheeler on 04/05/2024.
//

#pragma once

#include "KeyMapping.hpp"
#include "Application.hpp"
#include "Gui.hpp"

namespace sage
{

class Editor : public Application
{
    bool debugMode = false;
    void init() override;
    void draw() override;
    void drawGrid();
    void enablePlayMode();
    void enableEditMode();
    void manageScenes();
    void initEditorScene();
public:
    void Update() override;
    void initGameScene();
};

} // sage
