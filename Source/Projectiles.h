#pragma once
#include "raylib.h"
#include "Resources.h"

class Projectile
{
private:
	Vector2 position = { 0.f,0.f };
	bool active = true;
	int speed;
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

public:

	Projectile(Vector2 startPos, int speed = GameConstants::Projectile::SPEED);
	void Update();
	void Render(const Resources& resources) const;

	[[nodiscard]] bool IsActive() const noexcept { return active; }
	[[nodiscard]] Vector2 GetLineStart() const noexcept { return lineStart; }
	[[nodiscard]] Vector2 GetLineEnd() const noexcept { return lineEnd; }

	void SetInactive() noexcept { active = false; }
};
