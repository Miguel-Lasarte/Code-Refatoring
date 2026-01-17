#include "Projectiles.h"

Projectile::Projectile(Vector2 startPos, int speed) : position(startPos), speed(speed) {
	lineStart = { position.x, position.y - GameConstants::Projectile::LENGTH };
	lineEnd = { position.x, position.y + GameConstants::Projectile::LENGTH };
}



void Projectile::Update()
{
	using namespace GameConstants::Projectile;
	position.y -= static_cast<float>(speed);

	lineStart = { position.x, position.y - LENGTH };
	lineEnd = { position.x, position.y + LENGTH };

	if (position.y < 0.f || position.y > OUT_OF_BOUNDS)
	{
		active = false;
	}
}

void Projectile::Render(const Resources& resources) const
{
	using namespace GameConstants::Projectile::Rendering;
	DrawTexturePro(resources.GetProjectileTexture(),
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


