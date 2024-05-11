//
// Created by Steve Wheeler on 06/05/2024.
//

#include "Gui.hpp"
#include "windows/FloatingWindow.hpp"
#include "UserInput.hpp"

#include "raygui.h"


namespace sage::editor
{

void GUI::MarkGUIActive()
{
    focused = true;
}

void GUI::MarkGUIInactive()
{
    focused = false;
}

void GUI::Draw(const std::string& mode, Cursor* cursor)
{
    float modifier = 100;
    GuiGroupBox((Rectangle){ 0 + modifier, 8, 184, 40 }, NULL);
    if(GuiButton((Rectangle){ 8 + modifier, 8, 24, 24 }, "#002#"))
    {
        saveButtonPressed.publish();
    }
    if (GuiButton((Rectangle){ 40 + modifier, 8, 24, 24 }, "#001#"))
    {
        loadButtonPressed.publish();
    }

    for (auto& window : windows)
    {
        window->Update();
    }
    
    DrawText(TextFormat("Editor Mode: %s", mode.c_str()), screenSize.x - 150, 50, 10, BLACK);
    drawDebugCollisionText(cursor);
    
}

void GUI::drawDebugCollisionText(Cursor* cursor)
{
    // Draw some debug GUI text
    DrawText(TextFormat("Hit Object: %s", cursor->hitObjectName.c_str()), 10, 50, 10, BLACK);

    if (cursor->collision.hit)
    {
        int ypos = 70;

        DrawText(TextFormat("Distance: %3.2f", cursor->collision.distance), 10, ypos, 10, BLACK);

        DrawText(TextFormat("Hit Pos: %3.2f %3.2f %3.2f",
                            cursor->collision.point.x,
                            cursor->collision.point.y,
                            cursor->collision.point.z), 10, ypos + 15, 10, BLACK);

        DrawText(TextFormat("Hit Norm: %3.2f %3.2f %3.2f",
                            cursor->collision.normal.x,
                            cursor->collision.normal.y,
                            cursor->collision.normal.z), 10, ypos + 30, 10, BLACK);

        DrawText(TextFormat("Entity ID: %d", cursor->rayCollisionResultInfo.collidedEntityId), 10,
                 ypos + 45, 10, BLACK);

    }
}

void GUI::onWindowResize(Vector2 newScreenSize)
{
    screenSize = newScreenSize;
    toolbox->position = { 10, 135 };
    toolbox->size = { 200, 400 };
    toolbox->content_size = { 140, 320 };
    
    objectprops->position = { newScreenSize.x - 200 - 10, 100 };
    objectprops->size = { 200,  newScreenSize.y/2 - 100 };
    objectprops->content_size = { 140, 320 };

    toolprops->position = { newScreenSize.x - 200 - 10, 100 +  newScreenSize.y/2 - 100 };
    toolprops->size = { 200,  newScreenSize.y/2 - 100 };
    toolprops->content_size = { 140, 320 };
}

GUI::GUI(Settings* _settings, UserInput* _userInput, Camera* _camera) :
    settings(_settings), camera(_camera)
{
    // TODO: No hardcoded values
    // TODO: Resolution aware fonts
    // TODO: Toolbox (create mode) and toolbox properties
    // TODO: Create tool: rotate, move, scale
    screenSize = { static_cast<float>(settings->SCREEN_WIDTH), 
                   static_cast<float>(settings->SCREEN_HEIGHT) };
    {
        entt::sink windowUpdate{_userInput->onWindowUpdate};
        windowUpdate.connect<&GUI::onWindowResize>(this);
    }
    toolbox = std::make_unique<FloatingWindow>(FloatingWindow({ 10, 135 },
                                                              { 200, 400 },
                                                              { 140, 320 },
                                                              "Toolbox"));
    objectprops = std::make_unique<FloatingWindow>(FloatingWindow({ static_cast<float>(settings->SCREEN_WIDTH - 200 - 10), 100 },
                                                                  { 200,  static_cast<float>(settings->SCREEN_HEIGHT)/2 - 100 },
                                                                  { 140, 320 },
                                                                  "Object Properties"));
    toolprops = std::make_unique<FloatingWindow>(FloatingWindow({ static_cast<float>(settings->SCREEN_WIDTH - 200 - 10), 100 +  static_cast<float>(settings->SCREEN_HEIGHT)/2 - 100},
                                                              { 200,  static_cast<float>(settings->SCREEN_HEIGHT)/2 - 100 },
                                                              { 140, 320 },
                                                              "Tool Properties"));
    windows.push_back(toolbox.get());
    windows.push_back(objectprops.get());
    windows.push_back(toolprops.get());
    {
        entt::sink onWindowHover{toolbox->onWindowHover};
        entt::sink onWindowHoverStop{toolbox->onWindowHoverStop};
        onWindowHover.connect<&GUI::MarkGUIActive>(this);
        onWindowHoverStop.connect<&GUI::MarkGUIInactive>(this);
        onWindowHover.connect<&Camera::ScrollDisable>(camera);
        onWindowHoverStop.connect<&Camera::ScrollEnable>(camera);
    }
    {
        entt::sink onWindowHover{objectprops->onWindowHover};
        entt::sink onWindowHoverStop{objectprops->onWindowHoverStop};
        onWindowHover.connect<&GUI::MarkGUIActive>(this);
        onWindowHoverStop.connect<&GUI::MarkGUIInactive>(this);
        onWindowHover.connect<&Camera::ScrollDisable>(camera);
        onWindowHoverStop.connect<&Camera::ScrollEnable>(camera);
    }
    {
        entt::sink onWindowHover{toolprops->onWindowHover};
        entt::sink onWindowHoverStop{toolprops->onWindowHoverStop};
        onWindowHover.connect<&GUI::MarkGUIActive>(this);
        onWindowHoverStop.connect<&GUI::MarkGUIInactive>(this);
        onWindowHover.connect<&Camera::ScrollDisable>(camera);
        onWindowHoverStop.connect<&Camera::ScrollEnable>(camera);
    }
}
} // sage