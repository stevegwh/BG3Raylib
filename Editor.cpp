﻿//
// Created by steve on 22/02/2024.
//

#include "Editor.hpp"

#include "GameObjectFactory.hpp"

// NB: "GameManager" is friend
#include "GameManager.hpp"

#include <iostream>

namespace sage
{
    Editor::Editor(UserInput* _cursor) : cursor(_cursor)
    {
        const std::function<void()> f1 = [p = this] { p->OnCursorClick(); };
        auto e1 = std::make_shared<EventCallback>(f1);
        cursor->OnClickEvent->Subscribe(e1);
        eventCallbacks["OnCursorClick"] = e1;

        const std::function<void()> f2 = [p = this] { p->OnCollisionHit(); };
        auto e2 = std::make_shared<EventCallback>(f2);
        cursor->OnCollisionHitEvent->Subscribe(e2);
        eventCallbacks["OnCollisionHit"] = e2;

        const std::function<void()> f3 = [p = this] { p->OnDeleteModeKeyPressed(); };
        auto e3 = std::make_shared<EventCallback>(f3);
        cursor->OnDeleteKeyPressedEvent->Subscribe(e3);
        eventCallbacks["OnDeleteModeKeyPressed"] = e3;

        const std::function<void()> f4 = [p = this] { p->OnCreateModeKeyPressed(); };
        auto e4 = std::make_shared<EventCallback>(f4);
        cursor->OnCreateKeyPressedEvent->Subscribe(e4);
        eventCallbacks["OnCreateModeKeyPressed"] = e4;

        const std::function<void()> f5 = [p = this] { p->OnGenGridKeyPressed(); };
        auto e5 = std::make_shared<EventCallback>(f5);
        cursor->OnGenGridKeyPressedEvent->Subscribe(std::make_shared<EventCallback>(f5));
        eventCallbacks["OnGenGridKeyPressed"] = e5;

        const std::function<void()> f6 = [p = this] { p->OnSerializeButton(); };
        auto e6 = std::make_shared<EventCallback>(f6);
        cursor->OnSerializeKeyPressedEvent->Subscribe(e6);
        eventCallbacks["OnSerializeButton"] = e6;

        EntityID floor = Registry::GetInstance().CreateEntity();
        Vector3 g0 = (Vector3){ -50.0f, 0.1f, -50.0f };
        Vector3 g2 = (Vector3){  50.0f, 0.1f,  50.0f };
        BoundingBox bb = {
            .min = g0,
            .max = g2
        };
        auto floorCollidable = std::make_unique<Collideable>(floor, bb);
        floorCollidable->collisionLayer = FLOOR;
        GM.collisionSystem->AddComponent(std::move(floorCollidable));

//        auto floorWorldObject = std::make_unique<WorldObject>(floor);
//        worldSystem->AddComponent(std::move(floorWorldObject));

        GM.DeserializeMap(); // TODO: Should specify path to saved map of scene
        // This should also be based on scene parameters
        GM.navigationGridSystem->Init(100, 1.0f);
        GM.navigationGridSystem->PopulateGrid();
    }
    
    Editor::~Editor()
    {
        // Unsubscribe from all events
        cursor->OnClickEvent->Unsubscribe(eventCallbacks.at("OnClick"));
        cursor->OnCollisionHitEvent->Unsubscribe(eventCallbacks.at("OnCollisionHit"));
        cursor->OnDeleteKeyPressedEvent->Unsubscribe(eventCallbacks.at("OnDeleteModeKeyPressed"));
        cursor->OnCreateKeyPressedEvent->Unsubscribe(eventCallbacks.at("OnCreateModeKeyPressed"));
        cursor->OnGenGridKeyPressedEvent->Unsubscribe(eventCallbacks.at("OnGenGridKeyPressed"));
        cursor->OnSerializeKeyPressedEvent->Unsubscribe(eventCallbacks.at("OnSerializeButton"));
    }
    
    
    void Editor::moveSelectedObjectToCursorHit()
    {
        Transform newTransform(selectedObject);
        newTransform.position = cursor->collision.point;

        const Renderable* renderable = GM.renderSystem->GetComponent(selectedObject);

        GM.transformSystem->SetComponent(selectedObject, newTransform);
        GM.collisionSystem->UpdateWorldBoundingBox(selectedObject, newTransform.position);
        
    }
    
    void Editor::OnCursorClick()
    {
        if (cursor->collision.hit)
        {
            switch (GM.collisionSystem->GetComponent(cursor->rayCollisionResultInfo.collidedEntityId)->collisionLayer)
            {
            case DEFAULT:
                break;
            case FLOOR:
                if (currentEditorMode == CREATE)
                {
                    GameObjectFactory::createTower(cursor->collision.point, "Tower Instance");
                }
                else if (currentEditorMode == SELECT)
                {
                    moveSelectedObjectToCursorHit();
                    selectedObject = 0;
                    currentEditorMode = IDLE;
                }
                break;
            case BUILDING:
                currentEditorMode = SELECT;
                selectedObject = cursor->rayCollisionResultInfo.collidedEntityId;
                break;
            }
        }
        else
        {
            selectedObject = 0;
        }
    }
    
    void Editor::OnSerializeButton()
    {
        GM.SerializeMap();
    }

    void Editor::OnDeleteModeKeyPressed()
    {
        if (currentEditorMode != SELECT) return;
        Registry::DeleteEntity(selectedObject);
        selectedObject = 0;
        currentEditorMode = IDLE;
    }

    void Editor::OnCreateModeKeyPressed()
    {
        if (currentEditorMode == CREATE) currentEditorMode = IDLE;
        else currentEditorMode = CREATE;
    }
    
    void Editor::OnGenGridKeyPressed()
    {
        GM.navigationGridSystem->PopulateGrid();
    }

    void Editor::OnCollisionHit()
    {
        //std::cout << "Collision detected. \n";
        //    if (colSystem.GetComponent(rayCollisionResultInfo.collidedEntityId).collisionLayer == FLOOR)
        //    {
        //        // Place model
        //    }
        //    else
        //    {
        //        // Select model
        //        // Store entityID of selected model
        //        // Change bounding box colour
        //    }
    }
    
    void Editor::Update()
    {
        
    } 

    void Editor::Draw3D()
    {
        if (currentEditorMode == SELECT)
        {
            GM.collisionSystem->BoundingBoxDraw(selectedObject, ORANGE);
        }

        GM.renderSystem->Draw();

        DrawGrid(100, 1.0f);

        for (const auto& gridSquareRow : GM.navigationGridSystem->GetGridSquares())
        {
            for (const auto& gridSquare : gridSquareRow)
            {
                BoundingBox bb;
                bb.min = gridSquare->worldPosMin;
                bb.max = gridSquare->worldPosMax;
                bb.max.y = 0.1f;
                Color color = gridSquare->occupied ? RED : GREEN;
                DrawBoundingBox(bb, color);
            }
        }
    }


    void Editor::Draw2D()
    {
        std::string mode = "NONE";
        if (currentEditorMode == IDLE) mode = "IDLE";
        else if (currentEditorMode == SELECT) mode = "SELECT";
        else if (currentEditorMode == MOVE) mode = "MOVE";
        else if (currentEditorMode == CREATE) mode = "CREATE";

        DrawText(TextFormat("Editor Mode: %s", mode.c_str()), SCREEN_WIDTH - 150, 50, 10, BLACK);
    }
}
