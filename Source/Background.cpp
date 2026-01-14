#include "game.h"
#include <algorithm>

Star::Star(Vector2 pos, float size) : initPosition(pos), position(pos), size(size) {
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

void Background::Update(float offset)
{
	std::for_each(stars.begin(), stars.end(), [offset](auto& star) {
		star.Update(offset);
		});

}

void Background::Render() const
{
	std::for_each(stars.begin(), stars.end(), [](const auto& star) {
		star.Render();
		});
}