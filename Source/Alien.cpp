#include "Alien.h"

Alien::Alien(float x, float y) : position({ x, y }) {
}

void Alien::Update()
{
	using namespace GameConstants::Alien;
	const float screenWidth = static_cast<float>(GetScreenWidth());
	if (moveRight)
	{
		position.x += SPEED;

		if (position.x >= screenWidth)
		{
			moveRight = false;
			position.y += DROP_DISTANCE;
		}
	}
	else
	{
		position.x -= SPEED;

		if (position.x <= 0)
		{
			moveRight = true;
			position.y += DROP_DISTANCE;
		}
	}
}

void Alien::Render(const Resources& resources) const
{
	using namespace GameConstants::Alien::Rendering;

	DrawTexturePro(resources.GetAlienTexture(),
		{
			0,
			0,
			SOURCE_SIZE,
			SOURCE_SIZE,
		},
		{
			position.x,
			position.y,
			TEXTURE_SIZE,
			TEXTURE_SIZE,
		}, { TEXTURE_OFFSET , TEXTURE_OFFSET },
		0,
		WHITE);
}