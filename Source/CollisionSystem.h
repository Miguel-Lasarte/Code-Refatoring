#pragma once
#include "raylib.h"

namespace CollisionSystem
{
	[[nodiscard]] float CalculateLineLength(Vector2 A, Vector2 B) noexcept;
	[[nodiscard]] bool IsPointInCircle(Vector2 circlePos, float radius, Vector2 point) noexcept;
	[[nodiscard]] bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineStart, Vector2 lineEnd) noexcept;
}

