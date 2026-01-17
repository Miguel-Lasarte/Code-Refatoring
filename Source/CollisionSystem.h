#pragma once
#include "raylib.h"

namespace CollisionSystem
{
	[[nodiscard]] inline float CalculateLineLength(Vector2 point1, Vector2 point2) noexcept
	{
		float dx = point2.x - point1.x;
		float dy = point2.y - point1.y;
		return sqrtf(dx * dx + dy * dy);
	}
	[[nodiscard]] inline bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2) noexcept
	{
		float dx = center2.x - center1.x;
		float dy = center2.y - center1.y;
		float distanceSquared = dx * dx + dy * dy;
		float radiusSum = radius1 + radius2;
		return distanceSquared <= radiusSum * radiusSum;
	}

	[[nodiscard]] inline bool CheckCirclePointCollision(Vector2 circleCenter, float radius, Vector2 point) noexcept
	{
		float dx = point.x - circleCenter.x;
		float dy = point.y - circleCenter.y;
		float distanceSquared = dx * dx + dy * dy;
		return distanceSquared <= radius * radius;
	}
}

