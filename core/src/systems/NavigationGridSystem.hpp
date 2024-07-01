#pragma once

#include "BaseSystem.hpp"
#include "../components/NavigationGridSquare.hpp"
#include "CollisionSystem.hpp"
#include "../utils/PriorityQueue.hpp"

#include "entt/entt.hpp"

namespace sage
{
	enum class AStarHeuristic
	{
		DEFAULT,
		FAVOUR_RIGHT
	};

	class NavigationGridSystem : public BaseSystem<NavigationGridSquare>
	{
		std::vector<std::pair<int, int>> directions = {
			{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
		};
		std::vector<std::vector<NavigationGridSquare*>> gridSquares;
		std::vector<Vector3> tracebackPath(const std::vector<std::vector<GridSquare>>& came_from,
		                                   const GridSquare& start,
		                                   const GridSquare& finish) const;
		static bool checkInside(GridSquare square, GridSquare minRange, GridSquare maxRange);
		bool getExtents(entt::entity entity, GridSquare& extents) const;
		bool checkExtents(GridSquare square, GridSquare extents) const;
		bool getExtents(Vector3 worldPos, GridSquare& extents) const;

	public:
		float spacing{};
		int slices{};

		explicit NavigationGridSystem(entt::registry* _registry);
		void Init(int _slices, float _spacing);
		void PopulateGrid() const;
		bool GetPathfindRange(const entt::entity& actorId, int bounds, GridSquare& minRange,
		                      GridSquare& maxRange) const;
		bool GridToWorldSpace(GridSquare gridPos, Vector3& out) const;
		bool WorldToGridSpace(Vector3 worldPos, GridSquare& out) const;
		bool WorldToGridSpace(Vector3 worldPos, GridSquare& out, const GridSquare& _minRange,
		                      const GridSquare& _maxRange) const;
		[[nodiscard]] GridSquare FindNextBestLocation(entt::entity entity, GridSquare target) const;
		[[nodiscard]] GridSquare FindNextBestLocation(GridSquare currentPos, GridSquare target, GridSquare minRange,
		                                              GridSquare maxRange, GridSquare extents) const;
		[[nodiscard]] NavigationGridSquare* CastRay(int currentRow, int currentCol, Vector2 direction,
		                                            float distance, std::vector<GridSquare>& debugLines) const;
		[[nodiscard]] std::vector<Vector3> AStarPathfind(const entt::entity& entity, const Vector3& startPos,
		                                                 const Vector3& finishPos,
		                                                 AStarHeuristic heuristicType = AStarHeuristic::DEFAULT);
		[[nodiscard]] std::vector<Vector3> AStarPathfind(const entt::entity& entity, const Vector3& startPos,
		                                                 const Vector3& finishPos, const GridSquare& minRange,
		                                                 const GridSquare& maxRange,
		                                                 AStarHeuristic heuristicType = AStarHeuristic::DEFAULT);
		const std::vector<std::vector<NavigationGridSquare*>>& GetGridSquares();
		void DrawDebugPathfinding(const GridSquare& minRange, const GridSquare& maxRange) const;

		void MarkSquareAreaOccupied(const BoundingBox& occupant, bool occupied,
		                        entt::entity occupantEntity = entt::null) const;
		void MarkSquaresOccupied(const std::vector<GridSquare>& squares, bool occupied = true) const;
		void MarkSquaresDebug(const std::vector<GridSquare>& squares, Color color, bool occupied = true) const;
		[[nodiscard]] bool CheckSingleSquareOccupied(Vector3 worldPos) const;
		[[nodiscard]] bool CheckSingleSquareOccupied(GridSquare position) const;
		[[nodiscard]] bool CheckBoundingBoxAreaUnoccupied(Vector3 worldPos, const BoundingBox& bb) const;
		[[nodiscard]] bool CheckBoundingBoxAreaUnoccupied(GridSquare square, const BoundingBox& bb) const;
		[[nodiscard]] entt::entity CheckSingleSquareOccupant(Vector3 worldPos) const;
		[[nodiscard]] entt::entity CheckSingleSquareOccupant(GridSquare position) const;
		[[nodiscard]] entt::entity CheckSquareAreaOccupant(Vector3 worldPos, const BoundingBox& bb) const;
		[[nodiscard]] entt::entity CheckSquareAreaOccupant(GridSquare square, const BoundingBox& bb) const;
		[[nodiscard]] bool CompareSquareAreaOccupant(entt::entity entity, const BoundingBox& bb) const;
		[[nodiscard]] bool CompareSingleSquareOccupant(entt::entity entity, const BoundingBox& bb) const;

		void DrawDebug() const;
	};
} // sage
