#pragma once
#include "raylib.h"
#include "Resources.h"

class Projectile
{
private:
	Vector2 position = { 0.f,0.f };
	bool active = true;
	int speed;
	

public:

	Projectile(Vector2 startPos, int speed = GameConstants::Projectile::SPEED);
	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] bool IsActive() const noexcept { return active; }
	[[nodiscard]] Vector2 GetPosition() const noexcept { return position; }


	void SetInactive() noexcept { active = false; }
};
