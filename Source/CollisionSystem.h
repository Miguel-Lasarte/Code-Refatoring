#pragma once
#include "raylib.h"

namespace CollisionSystem
{
	[[nodiscard]] inline bool Check(Rectangle a, Rectangle b) noexcept {
		return CheckCollisionRecs(a, b);
	}
}






