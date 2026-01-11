#include "CollisionSystem.h"
#include "Constants.h"
#include <cmath>

namespace CollisionSystem
{
	float CalculateLineLength(Vector2 A, Vector2 B) noexcept
	{
		const float dx = B.x - A.x;
		const float dy = B.y - A.y;
		return std::sqrtf(dx * dx + dy * dy);
	}
	bool IsPointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept
	{
		return CalculateLineLength(circlePos, point) < radius;
	}
	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) noexcept
	{
		if(IsPointInCircle(circlePos, circleRadius, lineStart) || IsPointInCircle(circlePos, circleRadius, lineEnd))
		{
			return true;
		}
		const float lineLen = CalculateLineLength(lineStart, lineEnd);
		if(lineLen == 0.0f)
		{
			return false; 
		}

		const float dx = lineEnd.x - lineStart.x;
		const float dy = lineEnd.y - lineStart.y;
		const float dotP = ((circlePos.x - lineStart.x) * dx + (circlePos.y - lineStart.y) * dy) / (lineLen * lineLen);
		const Vector2 closestPoint{
			lineStart.x + (dotP * dx),
			lineStart.y + (dotP * dy)
		};

		const float distToStart = CalculateLineLength(lineStart, closestPoint);
		const float distToEnd = CalculateLineLength(lineEnd, closestPoint);
		const float totalDist = distToStart + distToEnd;

		constexpr float buffer = GameConstants::Physics::Collision::BUFFER;
		if(std::fabsf(totalDist - lineLen) > buffer)
		{
			return false;
		}
		return CalculateLineLength(circlePos, closestPoint) < circleRadius;
	}
}