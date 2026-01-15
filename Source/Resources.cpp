#include "Resources.h"
#pragma warning (push)
#pragma warning(disable : 26446)

std::optional<Resources> Resources::TryCreate() {
	Resources res;

	res.alienTexture = ResourceHandle<Texture2D>("./Assets/Alien.png");
	if (!res.alienTexture.IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Alien.png");
		return std::nullopt;
	}

	res.wallTexture = ResourceHandle<Texture2D>("./Assets/Barrier.png");
	if (!res.wallTexture.IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Barrier.png");
		return std::nullopt;
	}

	res.projectileTexture = ResourceHandle<Texture2D>("./Assets/Laser.png");
	if (!res.projectileTexture.IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Laser.png");
		return std::nullopt;
	}

	res.shipTextures.reserve(GameConstants::Player::Rendering::TEXTURE_COUNT);
	res.shipTextures.emplace_back("./Assets/Ship1.png");
	if (!res.shipTextures.back().IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Ship1.png");
		return std::nullopt;
	}

	res.shipTextures.emplace_back("./Assets/Ship2.png");
	if (!res.shipTextures.back().IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Ship2.png");
		return std::nullopt;
	}

	res.shipTextures.emplace_back("./Assets/Ship3.png");
	if (!res.shipTextures.back().IsLoaded()) {
		TraceLog(LOG_ERROR, "Failed to load Ship3.png");
		return std::nullopt;
	}

	return res;
}

Texture2D Resources::GetShipTexture(size_t index) const  noexcept{
	if (index >= shipTextures.size()) {
		TraceLog(LOG_ERROR, "Ship texture index out of bounds: %zu", index);
		return shipTextures[0].Get();
	}
	return shipTextures[index].Get();
}

#pragma warning (pop)