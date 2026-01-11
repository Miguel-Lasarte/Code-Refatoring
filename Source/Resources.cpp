#include "Resources.h"
#pragma warning (push)
#pragma warning(disable : 26446)

Resources::Resources() {
	alienTexture = ResourceHandle<Texture2D>("./Assets/Alien.png");
	wallTexture = ResourceHandle<Texture2D>("./Assets/Barrier.png");
	projectileTexture = ResourceHandle<Texture2D>("./Assets/Laser.png");

	shipTextures.reserve(GameConstants::Player::Rendering::TEXTURE_COUNT);
	shipTextures.emplace_back("./Assets/Ship1.png");
	shipTextures.emplace_back("./Assets/Ship2.png");
	shipTextures.emplace_back("./Assets/Ship3.png");

	if(!alienTexture.IsLoaded() || !wallTexture.IsLoaded() || !projectileTexture.IsLoaded()) {
		throw std::runtime_error("Failed to load one or more textures.");
	}

	for (const auto& shipTexture : shipTextures) {
		if(!shipTexture.IsLoaded()) {
			throw std::runtime_error("Failed to load ship texture.");
		}
	}
}

Texture2D Resources::GetShipTexture(size_t index) {
	if (index >= shipTextures.size()) {
		throw std::out_of_range("Ship texture index out of range.");
	}
	return shipTextures[index].Get();
}

#pragma warning (pop)