#pragma once
#include "raylib.h"
#include "Resources.h"

class Player {
private:
	float xPos = 0.f;
	float timer = 0.f;
	size_t activeTexture = 0;
	int  lives = GameConstants::Player::INITIAL_LIVES;
public:
	

	explicit Player(float screenWidth);
	void Update();
	void Render(const Resources& resources)const;

	[[nodiscard]] int GetLives() const noexcept { return lives; }
	[[nodiscard]] Vector2 GetPosition() const noexcept {
		return { xPos,static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };
	}
	[[nodiscard]] Rectangle GetBounds() const noexcept
	{
		using namespace GameConstants::Player;
		const float y = static_cast<float>(GetScreenHeight()) - BASE_HEIGHT;
		return { xPos - HALF_WIDTH, y - HALF_HEIGHT, HALF_WIDTH * 2.f, HALF_HEIGHT * 2.f };
	}
	bool TakeDamage() noexcept;
};