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

	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] Vector2 GetPosition() const noexcept { return position; }
	[[nodiscard]] bool IsActive() const noexcept { return active; }

	void TakeDamage() noexcept;
};
