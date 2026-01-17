#include "Resources.h"
#pragma warning (push)
#pragma warning(disable : 26446)

 Resources::Resources()
    : alienTexture("./Assets/Alien.png"),
      wallTexture("./Assets/Barrier.png"),
      projectileTexture("./Assets/Laser.png")
{
    shipTextures.reserve(GameConstants::Player::Rendering::TEXTURE_COUNT);
    shipTextures.emplace_back("./Assets/Ship1.png");
    shipTextures.emplace_back("./Assets/Ship2.png");
    shipTextures.emplace_back("./Assets/Ship3.png");
}

Texture2D Resources::GetShipTexture(size_t index) const  noexcept{
	if (index >= shipTextures.size()) {
		TraceLog(LOG_ERROR, "Ship texture index out of bounds: %zu", index);
		return shipTextures[0].Get();
	}
	return shipTextures[index].Get();
}

#pragma warning (pop)