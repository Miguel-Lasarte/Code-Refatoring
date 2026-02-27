#pragma once
#include "raylib.h"
#include "Resources.h"

class Alien
{
private:
	Vector2 position = { 0.f, 0.f };
	bool active = true;
	bool moveRight = true;
public:

	Alien(float x, float y);

	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] Vector2 GetPosition() const noexcept { return position; }
	[[nodiscard]] bool IsActive() const noexcept { return active; }
	[[nodiscard]] Rectangle GetBounds()   const noexcept
	{
		using namespace GameConstants::Alien;
		return { position.x - HALF_WIDTH, position.y - HALF_HEIGHT,
				 HALF_WIDTH * 2.f, HALF_HEIGHT * 2.f };
	}

	void SetInactive() noexcept { active = false; }
};
