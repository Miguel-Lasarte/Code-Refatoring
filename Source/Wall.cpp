#include "Wall.h"

Wall::Wall(Vector2 pos) : position(pos) {
}

void Wall::Update()
{
	if (health < 1)
	{
		active = false;
	}
}

void Wall::Render(const Resources& resources) const
{
	using namespace GameConstants::Wall::Rendering;
	DrawTexturePro(resources.GetWallTexture(),
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


	DrawText(
		TextFormat("%i", health),
		static_cast<int>(position.x - HealthDisplay::TEXT_OFFSET_X),
		static_cast<int>(position.y + HealthDisplay::TEXT_OFFSET_Y),
		HealthDisplay::TEXT_SIZE,
		RED
	);
}



void Wall::TakeDamage() noexcept
{
	health--;
}
