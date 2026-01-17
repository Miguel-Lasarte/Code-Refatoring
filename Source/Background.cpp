#include "game.h"
#include <algorithm>

Star::Star(Vector2 pos, float size) : initPosition(pos), position(pos), size(size) {
}
void Star::Update(float offset)
{
	position.x = initPosition.x + offset;
	position.y = initPosition.y;

}

void Star::Render() const
{
	DrawCircle(
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		size,
		color
	);
}
Background::Background() {
	stars.reserve(GameConstants::Background::STAR_COUNT);

	for (int i = 0; i < GameConstants::Background::STAR_COUNT; ++i) {
		const Vector2 pos = {
			static_cast<float>(GetRandomValue(-GameConstants::Background::Stars::SPAWN_OFFSET, GetScreenWidth() + GameConstants::Background::Stars::SPAWN_OFFSET)),
			static_cast<float>(GetRandomValue(0,GetRenderHeight()))
		};
		const float size = static_cast<float>(GetRandomValue(GameConstants::Background::Stars::MIN_SIZE, GameConstants::Background::Stars::MAX_SIZE)) / GameConstants::Background::Stars::SIZE_DIVISOR;
		stars.emplace_back(pos, size);
	}
}


void Background::Update(float offset)
{
	for(auto& star : stars) {
		star.Update(offset);
	}
}

void Background::Render() const
{
	for (const auto& star : stars) {
		star.Render();
	}
}