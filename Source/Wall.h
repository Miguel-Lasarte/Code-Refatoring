#pragma once
#include "raylib.h"
#include "Resources.h"
class Wall
{
private:
	Vector2 position = { 0.f,0.f };
	int health = GameConstants::Wall::INITIAL_HEALTH;
	bool active = true;

public:

	explicit Wall(Vector2 pos);

	void Render(const Resources& resources) const;

	[[nodiscard]] bool IsActive() const noexcept { return active; }
	[[nodiscard]] Rectangle GetBounds() const noexcept
	{
		using namespace GameConstants::Wall;
		return { position.x - HALF_WIDTH, position.y - HALF_HEIGHT,
				 HALF_WIDTH * 2.f, HALF_HEIGHT * 2.f };
	}

	void TakeDamage() noexcept;
};
