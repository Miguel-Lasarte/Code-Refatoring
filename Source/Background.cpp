#include "Background.h"
#include <algorithm>


Background::Background() {
	using namespace GameConstants::Background;
	stars.reserve(STAR_COUNT);

	for (int i = 0; i < STAR_COUNT; ++i) {
		const Vector2 pos = {
			static_cast<float>(GetRandomValue(-Stars::SPAWN_OFFSET, GetScreenWidth() + GameConstants::Background::Stars::SPAWN_OFFSET)),
			static_cast<float>(GetRandomValue(0,GetRenderHeight()))
		};
		const float size = static_cast<float>(GetRandomValue(GameConstants::Background::Stars::MIN_SIZE, GameConstants::Background::Stars::MAX_SIZE)) / GameConstants::Background::Stars::SIZE_DIVISOR;
		stars.emplace_back(pos, size);
	}
}




void Background::Render(float playerX) const
{
	const float offset = playerX * GameConstants::Background::PARALLAX_SPEED;

	for (const auto& star : stars)
	{
		DrawCircle(
			static_cast<int>(star.position.x + offset),
			static_cast<int>(star.position.y),
			star.size,
			GRAY);
	}
}