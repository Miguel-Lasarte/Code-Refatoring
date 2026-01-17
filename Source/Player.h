#pragma once
#include "raylib.h"
#include "Resources.h"

class Player {
private:
	float xPos = 0.f;
	float timer = 0.f;
	size_t activeTexture = 0;
	int direction = 0;
public:
	int  lives = GameConstants::Player::INITIAL_LIVES;

	explicit Player(float screenWidth);
	void Update();
	void Render(const Resources& resources)const;
	[[nodiscard]] float GetXPos() const noexcept { return xPos; }
	[[nodiscard]] Vector2 GetPosition() const noexcept {
		return { xPos,static_cast<float>(GetScreenHeight()) - GameConstants::Player::BASE_HEIGHT };
	}
	[[nodiscard]] size_t GetActiveTextureIndex() const noexcept { return activeTexture; }
};