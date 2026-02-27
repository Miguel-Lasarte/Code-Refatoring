#pragma once
#include "raylib.h"
#include <array>
#include "Constants.h"
#include "ResourceHandle.h"



class Resources
{
	std::array<ResourceHandle<Texture2D>, GameConstants::Player::Rendering::TEXTURE_COUNT> shipTextures;
	ResourceHandle<Texture2D> alienTexture;
	ResourceHandle<Texture2D> wallTexture;
	ResourceHandle<Texture2D> projectileTexture;
	
public:
	Resources();
	Resources(const Resources&) = delete;
	Resources& operator=(const Resources&) = delete;
	Resources(Resources&&) = delete;  
	Resources& operator=(Resources&&) = delete;  

	[[nodiscard]] const Texture2D& GetAlienTexture() const noexcept {
		return alienTexture.Get();
	}
	[[nodiscard]] const Texture2D& GetWallTexture() const noexcept {
		return wallTexture.Get();
	}
	[[nodiscard]] const Texture2D& GetProjectileTexture() const noexcept {
		return projectileTexture.Get();
	}
    [[nodiscard]] const Texture2D& GetShipTexture(std::size_t index) const noexcept;

};