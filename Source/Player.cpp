#include "Player.h"
#include <algorithm>

Player::Player(float screenWidth) : xPos(screenWidth / 2.0f) {
}

void Player::Update()
{
	using namespace GameConstants::Player;
	if (IsKeyDown(KEY_LEFT)) direction--;

	if (IsKeyDown(KEY_RIGHT)) direction++;

	xPos += SPEED * static_cast<float>(direction);

	const float minX = RADIUS;
	const float maxX = static_cast<float>(GetScreenWidth()) - RADIUS;
	xPos = std::clamp(xPos, minX, maxX);

	timer += GetFrameTime();
	if (timer >= Animation::INTERVAL)
	{
		activeTexture = (activeTexture + 1) % Rendering::TEXTURE_COUNT;
		timer = 0.f;
	}
}

void Player::Render(const Resources& resources) const
{
	using namespace GameConstants::Player;
	float window_height = static_cast<float>(GetScreenHeight());

	DrawTexturePro(resources.GetShipTexture(activeTexture),
		{
			0,
			0,
			Rendering::SOURCE_SIZE,
			Rendering::SOURCE_SIZE,
		},
		{
			xPos, window_height - BASE_HEIGHT,
			Rendering::TEXTURE_SIZE,
			Rendering::TEXTURE_SIZE,
		}, { Rendering::TEXTURE_OFFSET , Rendering::TEXTURE_OFFSET },
		0,
		WHITE);
}
