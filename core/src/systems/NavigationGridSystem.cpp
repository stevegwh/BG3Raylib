//
// Created by Steve Wheeler on 25/02/2024.
//

#include "NavigationGridSystem.hpp"

#include <queue>
#include <unordered_map>
#include <utility>
#include <iostream>

Vector3 calculateGridsquareCentre(Vector3 min, Vector3 max)
{
    Vector3 size = { 0 };

    size.x = fabsf(max.x - min.x);
    size.y = fabsf(max.y - min.y);
    size.z = fabsf(max.z - min.z);

    return { min.x + size.x/2.0f, min.y + size.y/2.0f, min.z + size.z/2.0f };
}

namespace sage
{

void NavigationGridSystem::DrawDebugPathfinding(const Vector2& minRange, const Vector2& maxRange)
{

    for (int i = 0; i  < gridSquares.size(); i++)
    {
        for (int j = 0; j < gridSquares.at(0).size(); j++)
        {
            gridSquares[i][j]->debugColor = false;
        }
    }
    for (int i = minRange.y; i  < maxRange.y; i++)
    {
        for (int j = minRange.x; j < maxRange.x; j++)
        {
            gridSquares[i][j]->debugColor = true;
        }
    }
}

bool NavigationGridSystem::GetPathfindRange(const entt::entity& actorId, int bounds, Vector2& minRange, Vector2& maxRange)
{
    auto bb = registry->get<Collideable>(actorId).worldBoundingBox;
    Vector3 center = { (bb.min.x + bb.max.x) / 2.0f, (bb.min.y + bb.max.y) / 2.0f, (bb.min.z + bb.max.z) / 2.0f };

    // Calculate the top-left and bottom-right corners of the square grid
    Vector3 topLeft = { center.x - bounds * spacing, center.y, center.z - bounds * spacing };
    Vector3 bottomRight = { center.x + bounds * spacing, center.y, center.z + bounds * spacing };

    // Get the grid indices for the top-left and bottom-right corners
    Vector2 topLeftIndex;
    Vector2 bottomRightIndex;

    bool topLeftValid = WorldToGridSpace(topLeft, topLeftIndex);
    bool bottomRightValid = WorldToGridSpace(bottomRight, bottomRightIndex);

    // Clip the top-left and bottom-right indices to the grid boundaries
    topLeftIndex.x = std::max(topLeftIndex.x, 0.0f);
    topLeftIndex.y = std::max(topLeftIndex.y, 0.0f);
    bottomRightIndex.x = std::min(bottomRightIndex.x, static_cast<float>(gridSquares.at(0).size() - 1));
    bottomRightIndex.y = std::min(bottomRightIndex.y, static_cast<float>(gridSquares.size() - 1));

    minRange = { static_cast<float>(topLeftIndex.x), static_cast<float>(topLeftIndex.y) };
    maxRange = { static_cast<float>(bottomRightIndex.x), static_cast<float>(bottomRightIndex.y) };

    return true;
}


/**
 * Translates a world position to a corresponding index on a grid.
 * Checks if the passed position is valid based on the entire grid.
 * @param worldPos The position in world space
 * @out (Out param) The resulting index of the corresponding grid square.
 * @return Whether the move is valid
 */
bool NavigationGridSystem::WorldToGridSpace(Vector3 worldPos, Vector2& out)
{
    return WorldToGridSpace(worldPos, 
                            out, 
                            {0,0}, 
                            {static_cast<float>(gridSquares.at(0).size()), static_cast<float>(gridSquares.size())});
}

/**
 * Translates a world position to a corresponding index on a grid.
 * Checks if the passed position is valid based on a grid range
 * @param worldPos The position in world space
 * @out (Out param) The resulting index of the corresponding grid square.
 * @return Whether the move is valid
 */
bool NavigationGridSystem::WorldToGridSpace(Vector3 worldPos, Vector2& out, const Vector2& minRange, const Vector2& maxRange)
{
    // Calculate the grid indices for the given world position
    int x = std::floor(worldPos.x / spacing) + (slices / 2);
    int y = std::floor(worldPos.z / spacing) + (slices / 2);
    out = {static_cast<float>(x), static_cast<float>(y)};

    return out.y < maxRange.y && out.x < maxRange.x
    && out.x >= minRange.x && out.y >= minRange.y;
}

void NavigationGridSystem::Init(int _slices, float _spacing)
{
    slices = _slices;
    spacing = _spacing;
    
    int halfSlices = slices / 2;

    // Resize gridSquares to the appropriate size
    gridSquares.clear();
    gridSquares.resize(slices);
    for (int i = 0; i < slices; ++i) 
    {
        gridSquares[i].resize(slices);
    }

    for (int j = -halfSlices; j < halfSlices; j++)
    {
        for (int i = -halfSlices; i < halfSlices; i++)
        {
            Vector3 v1 = {(float)i * spacing, 0, (float)j * spacing};
            Vector3 v3 = {(float)(i + 1) * spacing, 1.0f, (float)(j + 1) * spacing};

            //EntityID id = Registry::GetInstance().CreateEntity();
            entt::entity id = registry->create();

            
            // Store grid square in the 2D array

            auto& gridSquare = registry->emplace<NavigationGridSquare>(id,
                                                                       Vector2{ .x = static_cast<float>(i + halfSlices),
                                                                         .y = static_cast<float>(j + halfSlices)},
                                                                        v1,
                                                                        v3,
                                                                        calculateGridsquareCentre(v1, v3));
            gridSquares[j + halfSlices][i + halfSlices] = &gridSquare;
        }
    }
}

/**
 * Generates a sequence of nodes that should be the "optimal" route from point A to point B.
 * Checks entire grid.
 * @return A vector of "nodes" to travel to in sequential order. Empty if path is invalid (OOB or no path available).
 */
std::vector<Vector3> NavigationGridSystem::Pathfind(const Vector3& startPos, const Vector3& finishPos)
{
    return Pathfind(startPos, finishPos, {0,0},
                    {static_cast<float>(gridSquares.at(0).size()), static_cast<float>(gridSquares.size())});
}

/**
 * Generates a sequence of nodes that should be the "optimal" route from point A to point B.
 * Checks path within a range.
 * @minRange The minimum grid index in the pathfinding range.
 * @maxRange The maximum grid index in the pathfinding range.
 * @return A vector of "nodes" to travel to in sequential order. Empty if path is invalid (OOB or no path available).
 */
std::vector<Vector3> NavigationGridSystem::Pathfind(const Vector3& startPos, const Vector3& finishPos, const Vector2& minRange, const Vector2& maxRange)
{
    Vector2 startGridSquare = {0};
    Vector2 finishGridSquare = {0};
    if (!WorldToGridSpace(startPos, startGridSquare) || !WorldToGridSpace(finishPos, finishGridSquare)) return {};
    int startrow = startGridSquare.y;
    int startcol = startGridSquare.x;

    int finishrow = finishGridSquare.y;
    int finishcol = finishGridSquare.x;

    auto inside = [&](int row, int col) { return minRange.y <= row && row < maxRange.y && minRange.x <= col && col < maxRange.x; };

    std::vector<std::vector<bool>> visited(maxRange.y, std::vector<bool>(maxRange.x, false));
    std::vector<std::vector<std::pair<int, int>>> came_from(maxRange.y, std::vector<std::pair<int, int>>(maxRange.x, std::pair<int, int>(-1, -1)));

    std::vector<std::pair<int, int>> directions = { {1,0}, {0,1}, {-1,0}, {0,-1}, {1,1}, {-1,1}, {-1,-1}, {1,-1} };
    std::queue<std::pair<int,int>> frontier;

    frontier.emplace(startrow, startcol);
    visited[startrow][startcol] = true;

    bool pathFound = false;

    while (!frontier.empty())
    {
        auto current = frontier.front();
        frontier.pop();

        if (current.first == finishrow && current.second == finishcol) 
        {
            pathFound = true;
            break;
        }

        for (const auto& dir : directions)
        {
            int next_row = current.first + dir.first;
            int next_col = current.second + dir.second;

            if (inside(next_row, next_col) && 
            !visited[next_row][next_col] && 
            !gridSquares[next_row][next_col]->occupied)
            {
                frontier.emplace(next_row, next_col);
                visited[next_row][next_col] = true;
                came_from[next_row][next_col] = current;
            }
        }
    }
    
    if (!pathFound) {
        return {}; 
    }

    // Trace path back from finish to start
    std::vector<Vector3> path;
    std::pair<int, int> current = {finishrow, finishcol};
    while (current.first != startrow || current.second != startcol)
    {
        auto node = gridSquares[current.first][current.second];
        path.push_back(node->worldPosMin);
        current = came_from[current.first][current.second];
    }
    std::reverse(path.begin(), path.end());

    return path;
}


void NavigationGridSystem::PopulateGrid()
{
    for (auto& row : gridSquares)
    {
        for (auto& gridSquare : row)
        {
            gridSquare->occupied = false;
        }
    }
    
    const auto& view = registry->view<Collideable>();
    for (const auto& ent : view)
    {
        const auto& bb = view.get<Collideable>(ent);
        if (bb.collisionLayer != CollisionLayer::BUILDING) continue;

        // Get the grid indices for the bounding box
        Vector2 topLeftIndex;
        Vector2 bottomRightIndex;
        if (!WorldToGridSpace(bb.worldBoundingBox.min, topLeftIndex) ||
        !WorldToGridSpace(bb.worldBoundingBox.max, bottomRightIndex))
        {
            continue;
        }

        int min_col = std::min((int)topLeftIndex.x, (int)bottomRightIndex.x);
        int max_col = std::max((int)topLeftIndex.x, (int)bottomRightIndex.x);
        int min_row = std::min((int)topLeftIndex.y, (int)bottomRightIndex.y);
        int max_row = std::max((int)topLeftIndex.y, (int)bottomRightIndex.y);


        for (int row = min_row; row <= max_row; ++row)
        {
            for (int col = min_col; col <= max_col; ++col)
            {
                // Access grid square from the 2D array
                gridSquares[row][col]->occupied = true;
            }
        }

    }
}

const std::vector<std::vector<NavigationGridSquare*>>& NavigationGridSystem::GetGridSquares()
{
    return gridSquares;
}

NavigationGridSystem::NavigationGridSystem(entt::registry* _registry) :
    BaseSystem<NavigationGridSquare>(_registry)
{
}

} // sage