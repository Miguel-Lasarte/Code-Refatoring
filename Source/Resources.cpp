#include "Resources.h"


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

const Texture2D& Resources::GetShipTexture(size_t index) const  noexcept{
	if (index >= shipTextures.size()) {
		TraceLog(LOG_ERROR, "Ship texture index out of bounds: %zu", index);
		return shipTextures.front().Get();
	}
    return shipTextures[index].Get();
}

