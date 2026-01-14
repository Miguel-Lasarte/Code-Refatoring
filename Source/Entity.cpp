#include "game.h"

Projectile::Projectile(Vector2 startPos, EntityType projType, int speed) : position(startPos), type(projType), speed(speed) {
	lineStart = { position.x, position.y - GameConstants::Projectile::LENGTH };
	lineEnd = { position.x, position.y + GameConstants::Projectile::LENGTH };
}
Wall::Wall(Vector2 pos) : position(pos) {
}

Alien::Alien(float x, float y) : position({ x, y }) {
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

void Wall::Update()
{

	if (health < 1)
	{
		active = false;
	}


}

void Wall::takeDamage() noexcept
{
	health--;
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